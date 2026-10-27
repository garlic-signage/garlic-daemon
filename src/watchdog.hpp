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
#include "system_actions.hpp"

/**
 * Starts the player as child process and restarts it when it has ended
 * or when it sent no heartbeat for heartbeat_timeout seconds.
 *
 * check() is called periodically by the Daemon, heartbeat() whenever the
 * player sends a "heartbeat" message. All methods are thread safe.
 *
 * Backoff: when the player ends again within STABLE_RUNTIME after its start,
 * the next start is delayed. The delay doubles with every crash in a row
 * (5, 10, 20 ... seconds) up to BACKOFF_MAX. A player that ran longer than
 * STABLE_RUNTIME is restarted without delay.
 *
 * Reboot: after reboot_after_crashes crashes in a row the system is rebooted,
 * because the cause may be the system (e.g. graphics driver) and not the player.
 * To avoid a reboot loop with a broken player, this happens at most once per
 * REBOOT_INTERVAL. The time of the last reboot is stored in <state_dir>/last_reboot,
 * so it survives the reboot. If the file cannot be written, there is no reboot.
 *
 * The player gets its own process group, so stopping it also stops
 * all processes a start script has started.
 */
class Watchdog
{
    public:
        Watchdog(const Configuration &configuration, SystemActions &system_actions);

        /// Stops the player.
        ~Watchdog();

        /// Starts the player if it is not running and restarts it if it hangs.
        void check();

        /// The player is alive. Resets the hang detection.
        void heartbeat();

        /// false pauses the watchdog, e.g. during an update. A running player is not stopped.
        void setActivity(bool is_active);

        /// Stops the player: SIGTERM, after STOP_TIMEOUT SIGKILL.
        /// This is no crash, so the next check() starts the player without delay.
        void stopPlayer();

        bool isPlayerRunning();

        /// Process id of the player or -1 if it is not running.
        pid_t getPlayerPid();

        /// Delay before the next start after crash_count crashes in a row.
        static std::chrono::seconds restartDelay(int crash_count);

    private:
        static constexpr auto STOP_TIMEOUT    = std::chrono::seconds(5);
        static constexpr auto STABLE_RUNTIME  = std::chrono::seconds(60);
        static constexpr auto BACKOFF_START   = std::chrono::seconds(5);
        static constexpr auto BACKOFF_MAX     = std::chrono::seconds(300);
        static constexpr auto REBOOT_INTERVAL = std::chrono::hours(6);

        std::string player_name_;
        std::string player_command_;
        std::string player_user_;
        std::vector<std::string> player_environment_;
        std::chrono::seconds heartbeat_timeout_;
        int reboot_after_crashes_;
        std::string reboot_stamp_file_;
        SystemActions &system_actions_;

        std::mutex mtx_;
        bool is_active_   = true;
        pid_t player_pid_ = -1;
        std::chrono::steady_clock::time_point last_heartbeat_;
        std::chrono::steady_clock::time_point started_at_;
        std::chrono::steady_clock::time_point next_start_; // backoff: no start before this time
        int crash_count_ = 0;                              // crashes in a row

        // The following methods expect that mtx is locked
        void start();
        void stop();
        bool isRunning();
        bool isHanging() const;
        void playerEnded();
        void rebootIfAllowed();
};

#endif // WATCHDOG_HPP
