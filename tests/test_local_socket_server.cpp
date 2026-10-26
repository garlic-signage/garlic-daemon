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
#include <filesystem>
#include <thread>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "local_socket_server.hpp"

namespace fs = std::filesystem;
using namespace std::chrono_literals;

// Integration tests with a real Unix socket in a temporary directory.
// The clients run as the user of the test, which is the allowed user unless stated otherwise.
class LocalSocketServerTest : public ::testing::Test
{
    protected:
        fs::path tmp_dir;
        std::string socket_path;
        std::vector<int> client_fds;

        void SetUp() override
        {
            char dir_template[] = "/tmp/garlic-test-socket-XXXXXX";
            tmp_dir             = mkdtemp(dir_template);
            socket_path         = (tmp_dir / "test.sock").string();
        }

        void TearDown() override
        {
            for (int fd : client_fds)
                close(fd);
            fs::remove_all(tmp_dir);
        }

        // Connects like the player would do
        int connectClient()
        {
            int fd = socket(AF_UNIX, SOCK_STREAM, 0);
            sockaddr_un address{};
            address.sun_family = AF_UNIX;
            std::strcpy(address.sun_path, socket_path.c_str());
            EXPECT_EQ(connect(fd, reinterpret_cast<sockaddr *>(&address), sizeof(address)), 0) << strerror(errno);
            client_fds.push_back(fd);
            return fd;
        }

        static void write(int fd, const std::string &data)
        {
            ASSERT_EQ(::write(fd, data.data(), data.size()), data.size());
        }

        // Reads what the server sent, waits at most 2 seconds
        static std::string read(int fd)
        {
            timeval timeout{2, 0};
            setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            char buffer[1024];
            ssize_t bytes_read = recv(fd, buffer, sizeof(buffer), 0);
            return bytes_read > 0 ? std::string(buffer, bytes_read) : "";
        }

        // Polls until a message arrives, but at most 2 seconds, so a broken test fails instead of hanging
        static bool pollWithTimeout(LocalSocketServer &server, std::string &message)
        {
            auto deadline = std::chrono::steady_clock::now() + 2s;
            while (std::chrono::steady_clock::now() < deadline)
            {
                if (server.pollingQueue(message))
                    return true;
                std::this_thread::sleep_for(5ms);
            }
            return false;
        }
};

TEST_F(LocalSocketServerTest, ThrowsWhenSocketCannotBeCreated)
{
    EXPECT_THROW(LocalSocketServer server((tmp_dir / "missing-dir/test.sock").string(), getuid()), std::runtime_error);
}

TEST_F(LocalSocketServerTest, SocketFileIsCreatedAndRemoved)
{
    {
        LocalSocketServer server(socket_path, getuid());
        EXPECT_TRUE(fs::is_socket(socket_path));
    }
    EXPECT_FALSE(fs::exists(socket_path));
}

TEST_F(LocalSocketServerTest, OldSocketFileIsReplaced)
{
    {
        LocalSocketServer server(socket_path, getuid());
    }
    fs::copy_file("/etc/hostname", socket_path); // e.g. a file left after a crash

    LocalSocketServer server(socket_path, getuid());

    EXPECT_TRUE(fs::is_socket(socket_path));
}

TEST_F(LocalSocketServerTest, PollingEmptyQueueReturnsFalse)
{
    LocalSocketServer server(socket_path, getuid());
    std::string message;

    EXPECT_FALSE(server.pollingQueue(message));
}

TEST_F(LocalSocketServerTest, ReceivesLine)
{
    LocalSocketServer server(socket_path, getuid());
    server.start();

    write(connectClient(), "reboot\n");

    std::string message;
    EXPECT_TRUE(server.waitForMessage(message));
    EXPECT_EQ(message, "reboot");
}

TEST_F(LocalSocketServerTest, SeveralLinesInOneWriteKeepOrder)
{
    LocalSocketServer server(socket_path, getuid());
    server.start();

    write(connectClient(), "first\nsecond\nthird\n");

    std::string message;
    EXPECT_TRUE(server.waitForMessage(message));
    EXPECT_EQ(message, "first");
    EXPECT_TRUE(server.waitForMessage(message));
    EXPECT_EQ(message, "second");
    EXPECT_TRUE(server.waitForMessage(message));
    EXPECT_EQ(message, "third");
}

TEST_F(LocalSocketServerTest, LineSplitOverSeveralWrites)
{
    LocalSocketServer server(socket_path, getuid());
    server.start();
    int fd = connectClient();

    write(fd, "reb");
    std::this_thread::sleep_for(50ms);
    write(fd, "oot\n");

    std::string message;
    EXPECT_TRUE(pollWithTimeout(server, message));
    EXPECT_EQ(message, "reboot");
}

TEST_F(LocalSocketServerTest, CarriageReturnAndEmptyLinesAreIgnored)
{
    LocalSocketServer server(socket_path, getuid());
    server.start();

    write(connectClient(), "\n\r\nreboot\r\n");

    std::string message;
    EXPECT_TRUE(server.waitForMessage(message));
    EXPECT_EQ(message, "reboot");
    std::this_thread::sleep_for(50ms);
    EXPECT_FALSE(server.pollingQueue(message));
}

TEST_F(LocalSocketServerTest, LineWithoutNewlineIsNotReceived)
{
    LocalSocketServer server(socket_path, getuid());
    server.start();

    write(connectClient(), "reboot");

    std::string message;
    std::this_thread::sleep_for(300ms);
    EXPECT_FALSE(server.pollingQueue(message));
}

TEST_F(LocalSocketServerTest, TooLongLineClosesConnection)
{
    LocalSocketServer server(socket_path, getuid());
    server.start();
    int fd = connectClient();

    write(fd, std::string(10000, 'x'));

    EXPECT_EQ(read(fd), ""); // the server closed the connection
}

TEST_F(LocalSocketServerTest, OtherUserIsRejected)
{
    // The test runs as getuid(), so pretend that only another user is allowed.
    // Root is always allowed, so this test only works as normal user.
    if (getuid() == 0)
        GTEST_SKIP() << "runs as root, which is always allowed";

    LocalSocketServer server(socket_path, getuid() + 1);
    server.start();
    int fd = connectClient();

    write(fd, "reboot\n");

    EXPECT_EQ(read(fd), ""); // the server closed the connection
    std::string message;
    EXPECT_FALSE(server.pollingQueue(message));
}

TEST_F(LocalSocketServerTest, SendReachesAllClients)
{
    LocalSocketServer server(socket_path, getuid());
    server.start();
    int player   = connectClient();
    int debugger = connectClient();     // e.g. socat
    std::this_thread::sleep_for(300ms); // until the server has accepted both

    server.send("play_smil /media/garlic/sda1/SMIL/index.smil");

    EXPECT_EQ(read(player), "play_smil /media/garlic/sda1/SMIL/index.smil\n");
    EXPECT_EQ(read(debugger), "play_smil /media/garlic/sda1/SMIL/index.smil\n");
}

TEST_F(LocalSocketServerTest, ClientCanReconnect)
{
    LocalSocketServer server(socket_path, getuid());
    server.start();

    int first = connectClient();
    write(first, "first\n");
    close(first);
    client_fds.clear();
    write(connectClient(), "second\n");

    std::string message;
    EXPECT_TRUE(server.waitForMessage(message));
    EXPECT_EQ(message, "first");
    EXPECT_TRUE(server.waitForMessage(message));
    EXPECT_EQ(message, "second");
}

TEST_F(LocalSocketServerTest, StopWakesUpWaitForMessage)
{
    LocalSocketServer server(socket_path, getuid());
    server.start();

    std::jthread stopper(
        [&server]
        {
            std::this_thread::sleep_for(100ms);
            server.stop();
        });

    std::string message;
    EXPECT_FALSE(server.waitForMessage(message));
}

TEST_F(LocalSocketServerTest, WaitForMessageAfterStopIgnoresQueuedMessages)
{
    LocalSocketServer server(socket_path, getuid());
    server.start();
    write(connectClient(), "reboot\n");
    std::this_thread::sleep_for(300ms); // message is now queued

    server.stop();

    std::string message;
    EXPECT_FALSE(server.waitForMessage(message));
}

TEST_F(LocalSocketServerTest, StopRightAfterStartIsSafe)
{
    auto begin = std::chrono::steady_clock::now();
    {
        LocalSocketServer server(socket_path, getuid());
        server.start();
        server.stop();
        server.stop(); // calling stop twice is harmless
    }

    // The destructor joins the thread, which needs at most one poll timeout
    EXPECT_LT(std::chrono::steady_clock::now() - begin, 1s);
}
