/*************************************************************************************
    garlic-daemon: Linux daemon for garlic-player
    Copyright (C) 2023 Nikolaos Sagiadinos <ns@smil-control.com>
    This file is part of the garlic-daemon source code

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License, version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************************/

#include <gtest/gtest.h>
#include <chrono>
#include <cstring>
#include <csignal>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "daemon.hpp"
#include "fake_system_actions.hpp"
#include "fake_usb_actions.hpp"

using namespace std::chrono_literals;

// Tests the Daemon with the real classes, only the system actions are faked.
// Every test uses its own socket.
class DaemonTest : public ::testing::Test
{
    protected:
        FakeSystemActions system_actions;
        Configuration configuration;
        Watchdog watchdog{configuration, system_actions};
        MessageDispatcher dispatcher{system_actions, watchdog};
        LocalSocketServer socket_server{socketPath(), getuid()};
        FakeUsbActions usb_actions;
        UsbContentHandler usb_content_handler{configuration, usb_actions};
        UsbMonitor usb_monitor{configuration, system_actions, usb_content_handler};

        // The watchdog would try to start the real player, these tests are about the messages
        DaemonTest() { watchdog.setActivity(false); }

        static std::string socketPath()
        {
            return "/tmp/garlic-test-" + std::to_string(getpid()) + "-" +
                   ::testing::UnitTest::GetInstance()->current_test_info()->name() + ".sock";
        }

        // Sends a message like the player would do
        static void send(const std::string &message)
        {
            int fd = socket(AF_UNIX, SOCK_STREAM, 0);
            sockaddr_un address{};
            address.sun_family = AF_UNIX;
            std::strcpy(address.sun_path, socketPath().c_str());
            ASSERT_EQ(connect(fd, reinterpret_cast<sockaddr *>(&address), sizeof(address)), 0) << strerror(errno);
            std::string line = message + "\n";
            ASSERT_EQ(write(fd, line.data(), line.size()), line.size());
            close(fd);
        }

        // Waits until reboot() was called the expected number of times, but at most 2 seconds.
        bool waitForRebootCalls(int expected)
        {
            auto deadline = std::chrono::steady_clock::now() + 2s;
            while (system_actions.reboot_calls < expected && std::chrono::steady_clock::now() < deadline)
                std::this_thread::sleep_for(5ms);
            return system_actions.reboot_calls == expected;
        }
};

TEST_F(DaemonTest, ReceivedMessageIsDispatched)
{
    Daemon daemon(watchdog, socket_server, dispatcher, usb_monitor);
    daemon.start();

    send("reboot");

    EXPECT_TRUE(waitForRebootCalls(1));
}

TEST_F(DaemonTest, SeveralMessagesAreDispatched)
{
    Daemon daemon(watchdog, socket_server, dispatcher, usb_monitor);
    daemon.start();

    send("reboot");
    send("unknown");
    send("reboot");

    EXPECT_TRUE(waitForRebootCalls(2));
}

TEST_F(DaemonTest, NoMessagesAreDispatchedAfterStop)
{
    Daemon daemon(watchdog, socket_server, dispatcher, usb_monitor);
    daemon.start();
    daemon.stop();

    send("reboot");
    std::this_thread::sleep_for(300ms); // longer than one receive timeout

    EXPECT_EQ(system_actions.reboot_calls, 0);
}

TEST_F(DaemonTest, StopEndsAllThreadsQuickly)
{
    auto begin = std::chrono::steady_clock::now();
    {
        Daemon daemon(watchdog, socket_server, dispatcher, usb_monitor);
        daemon.start();
        std::this_thread::sleep_for(50ms); // the watchdog thread is now waiting for its next check
        daemon.stop();
    } // the destructor joins the threads

    // The watchdog waits 5 seconds between two checks, so this only passes if stop() wakes it up
    EXPECT_LT(std::chrono::steady_clock::now() - begin, 1s);
}

TEST_F(DaemonTest, StopWithoutStartIsHarmless)
{
    Daemon daemon(watchdog, socket_server, dispatcher, usb_monitor);
    daemon.stop();
    daemon.stop();
}

TEST_F(DaemonTest, SighupReloadsAndSigtermStops)
{
    // Block the signals only during this test and restore the old mask afterwards
    sigset_t old_mask;
    pthread_sigmask(SIG_SETMASK, nullptr, &old_mask);
    Daemon::blockSignals();

    Daemon daemon(watchdog, socket_server, dispatcher, usb_monitor);
    daemon.start();

    // raise() sends the signal to this thread, where it stays pending until sigwait() takes it
    raise(SIGHUP);
    raise(SIGTERM);
    daemon.waitForTerminationSignal(); // returns only after SIGTERM, SIGHUP must not stop it

    // If SIGHUP had stopped the daemon, SIGTERM would still be pending here
    sigset_t pending;
    sigpending(&pending);
    bool sigterm_pending = sigismember(&pending, SIGTERM);
    if (sigterm_pending)
    {
        int signal_number = 0;
        sigset_t sigterm;
        sigemptyset(&sigterm);
        sigaddset(&sigterm, SIGTERM);
        sigwait(&sigterm, &signal_number); // remove it, otherwise it would kill the test program
    }
    pthread_sigmask(SIG_SETMASK, &old_mask, nullptr);

    EXPECT_FALSE(sigterm_pending);

    // The daemon is stopped, so messages are no longer dispatched
    send("reboot");
    std::this_thread::sleep_for(300ms);
    EXPECT_EQ(system_actions.reboot_calls, 0);
}
