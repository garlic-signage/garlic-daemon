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

#ifndef WATCHDOG_HPP
#define WATCHDOG_HPP

#include <chrono>
#include <mutex>
#include <string>
#include <vector>
#include <sys/types.h>
#include "configuration.hpp"

/**
 * Starts the player as child process and restarts it when it has ended
 * or when it sent no heartbeat for heartbeat_timeout seconds.
 *
 * check() is called periodically by the Daemon, heartbeat() whenever the
 * player sends a "heartbeat" message. All methods are thread safe.
 *
 * The player gets its own process group, so stopping it also stops
 * all processes a start script has started.
 */
class Watchdog
{
    public:
        explicit Watchdog(const Configuration &configuration);

        /// Stops the player.
        ~Watchdog();

        /// Starts the player if it is not running and restarts it if it hangs.
        void check();

        /// The player is alive. Resets the hang detection.
        void heartbeat();

        /// false pauses the watchdog, e.g. during an update. A running player is not stopped.
        void setActivity(bool is_active);

        /// Stops the player: SIGTERM, after STOP_TIMEOUT SIGKILL.
        void stopPlayer();

        bool isPlayerRunning();

        /// Process id of the player or -1 if it is not running.
        pid_t getPlayerPid();

    private:
        static constexpr auto STOP_TIMEOUT = std::chrono::seconds(5);

        std::string player_name;
        std::string player_command;
        std::string player_user;
        std::vector<std::string> player_environment;
        std::chrono::seconds heartbeat_timeout;

        std::mutex mtx;
        bool is_active   = true;
        pid_t player_pid = -1;
        std::chrono::steady_clock::time_point last_heartbeat;

        // The following methods expect that mtx is locked
        void start();
        void stop();
        bool isRunning();
        bool isHanging() const;
};

#endif // WATCHDOG_HPP
