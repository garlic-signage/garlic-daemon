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
        [[nodiscard]] std::string getMessageQueue() const;
        [[nodiscard]] std::string getPlayerName() const;
        [[nodiscard]] std::string getPlayerCommand() const;
        [[nodiscard]] std::string getPlayerUser() const;
        [[nodiscard]] std::vector<std::string> getPlayerEnvironment() const;
        [[nodiscard]] int getHeartbeatTimeout() const;

    private:
        const std::string APP_NAME       = "garlic-daemon";
        const std::string MESSAGE_QUEUE  = "/garlic-daemon";
        const std::string PLAYER_NAME    = "garlic-player";
        const std::string PLAYER_COMMAND = "/opt/garlic-player/garlic.sh";
        const std::string PLAYER_USER    = "digitalsignage";
        const int HEARTBEAT_TIMEOUT      = 0; // disabled

        std::string app_name       = APP_NAME;
        std::string message_queue  = MESSAGE_QUEUE;
        std::string player_name    = PLAYER_NAME;
        std::string player_command = PLAYER_COMMAND;
        std::string player_user    = PLAYER_USER;
        std::vector<std::string> player_environment;
        int heartbeat_timeout = HEARTBEAT_TIMEOUT;
};

#endif // CONFIGURATION_HPP
