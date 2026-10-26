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

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <iostream>
#include <string>
#include <vector>
#include <libconfig.h++>

/**
 * Holds the settings of the daemon. Every value has a default,
 * which is replaced by the value from the config file if it exists there.
 */
class Configuration
{
    public:
        Configuration();
        void readLibConfigValues(const libconfig::Config *config);

        [[nodiscard]] std::string getAppName() const;
        [[nodiscard]] std::string getSocketPath() const;
        [[nodiscard]] std::string getStateDir() const;
        [[nodiscard]] std::string getPlayerName() const;
        [[nodiscard]] std::string getPlayerCommand() const;
        [[nodiscard]] std::string getPlayerUser() const;
        [[nodiscard]] std::vector<std::string> getPlayerEnvironment() const;
        [[nodiscard]] int getHeartbeatTimeout() const;
        [[nodiscard]] int getRebootAfterCrashes() const;
        [[nodiscard]] std::string getUsbMountDir() const;
        [[nodiscard]] std::string getUsbFirmwareFile() const;

    private:
        const std::string APP_NAME          = "garlic-daemon";
        const std::string SOCKET_PATH       = "/run/garlic-daemon/garlic-daemon.sock";
        const std::string STATE_DIR         = "/var/lib/garlic-daemon";
        const std::string PLAYER_NAME       = "garlic-player";
        const std::string PLAYER_COMMAND    = "/opt/garlic-player/garlic.sh";
        const std::string PLAYER_USER       = "digitalsignage";
        const int HEARTBEAT_TIMEOUT         = 0; // disabled
        const int REBOOT_AFTER_CRASHES      = 7; // when the backoff has reached its maximum, 0 = never
        const std::string USB_MOUNT_DIR     = "/media/garlic";
        const std::string USB_FIRMWARE_FILE = "garlic-player.AppImage";

        std::string app_name       = APP_NAME;
        std::string socket_path    = SOCKET_PATH;
        std::string state_dir      = STATE_DIR;
        std::string player_name    = PLAYER_NAME;
        std::string player_command = PLAYER_COMMAND;
        std::string player_user    = PLAYER_USER;
        std::vector<std::string> player_environment;
        int heartbeat_timeout         = HEARTBEAT_TIMEOUT;
        int reboot_after_crashes      = REBOOT_AFTER_CRASHES;
        std::string usb_mount_dir     = USB_MOUNT_DIR;
        std::string usb_firmware_file = USB_FIRMWARE_FILE;
};

#endif // CONFIGURATION_HPP
