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

#include "daemon.hpp"

#include <csignal>
#include "log.hpp"

namespace
{
/// The signals the daemon reacts to
sigset_t daemonSignals()
{
    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGINT);
    sigaddset(&signals, SIGTERM);
    sigaddset(&signals, SIGHUP);
    return signals;
}
} // namespace

Daemon::Daemon(Watchdog &watchdog, IPCMessageReceiver &receiver, MessageDispatcher &dispatcher)
    : watchdog(watchdog), receiver(receiver), dispatcher(dispatcher)
{
}

/**
 * Stops the threads. They are joined afterwards by the std::jthread destructors.
 */
Daemon::~Daemon()
{
    stop();
}

void Daemon::start()
{
    receiver.startIPCReceiver();
    watchdog_thread = std::jthread([this](std::stop_token stop_token) { loopWatchdog(stop_token); });
    listener_thread = std::jthread([this] { loopMessageListener(); });
}

/**
 * The listener thread ends because waitForMessage() returns false
 * after the receiver was stopped.
 */
void Daemon::stop()
{
    receiver.stopIPCReceiver();
    watchdog_thread.request_stop();
}

void Daemon::reload()
{
    // ToDo: reread the configuration
    LOG_INFO("Reload called.");
}

void Daemon::blockSignals()
{
    sigset_t signals = daemonSignals();
    pthread_sigmask(SIG_BLOCK, &signals, nullptr);
}

void Daemon::waitForTerminationSignal()
{
    sigset_t signals = daemonSignals();
    while (true)
    {
        int signal_number = 0;
        if (sigwait(&signals, &signal_number) != 0)
            continue;

        LOG_INFO("Signal number [", signal_number, "] received.");
        if (signal_number == SIGHUP)
        {
            reload();
            continue;
        }

        stop();
        return;
    }
}

void Daemon::loopWatchdog(std::stop_token stop_token)
{
    std::unique_lock<std::mutex> lock(watchdog_mutex);
    while (!stop_token.stop_requested())
    {
        watchdog.check();

        // Like sleep_for(), but returns immediately when a stop is requested
        watchdog_cv.wait_for(lock, stop_token, std::chrono::seconds(WATCHDOG_TIMER), [] { return false; });
    }
    std::cout << "Finish watchdog thread." << std::endl;
}

void Daemon::loopMessageListener()
{
    std::string message;
    while (receiver.waitForMessage(message))
    {
        std::cout << "Received message from queue: " << message << std::endl;
        dispatcher.dispatch(message);
    }
    std::cout << "Finish message queue listener thread." << std::endl;
}
