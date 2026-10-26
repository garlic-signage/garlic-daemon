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

#ifndef DAEMON_HPP
#define DAEMON_HPP

#include <condition_variable>
#include <mutex>
#include <stop_token>
#include <thread>
#include "watchdog.hpp"
#include "local_socket_server.hpp"
#include "message_dispatcher.hpp"
#include "usb_monitor.hpp"

/**
 * Runs the daemon with three threads:
 *  - watchdog: checks the player periodically
 *  - listener: passes messages from the socket to the dispatcher
 *  - usb: mounts USB sticks and handles their content
 *
 * Signals are not handled with a signal handler. Instead blockSignals()
 * blocks them before any thread is started and waitForTerminationSignal()
 * receives them synchronously with sigwait().
 */
class Daemon
{
    public:
        Daemon(Watchdog &watchdog, LocalSocketServer &socket_server, MessageDispatcher &dispatcher,
               UsbMonitor &usb_monitor);
        ~Daemon();

        /// Starts the socket server and the three threads.
        void start();

        /// Stops all threads. Calling it more than once is harmless.
        void stop();

        /// Called on SIGHUP.
        void reload();

        /// Blocks SIGINT, SIGTERM and SIGHUP for the calling thread and all threads started afterwards.
        /// Must be called in main() before any thread is started.
        static void blockSignals();

        /// Waits for signals: SIGHUP calls reload(), SIGINT and SIGTERM call stop() and return.
        void waitForTerminationSignal();

    private:
        static constexpr int WATCHDOG_TIMER = 5; // seconds between two watchdog checks

        Watchdog &watchdog;
        LocalSocketServer &socket_server;
        MessageDispatcher &dispatcher;
        UsbMonitor &usb_monitor;

        std::mutex watchdog_mutex;
        std::condition_variable_any watchdog_cv;
        std::jthread watchdog_thread; // threads are the last members, so they are destroyed first
        std::jthread listener_thread;
        std::jthread usb_thread;

        void loopWatchdog(std::stop_token stop_token);
        void loopMessageListener();
};

#endif // DAEMON_HPP
