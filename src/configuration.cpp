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

#include "configuration.hpp"

Configuration::Configuration() = default;

void Configuration::readLibConfigValues(const libconfig::Config *config)
{
    try
    {

        std::cout << "Start read values from config file" << std::endl;

        if (config->exists("app_name"))
            config->lookupValue("app_name", app_name);

        if (config->exists("message_queue"))
            config->lookupValue("message_queue", message_queue);

        if (config->exists("state_dir"))
            config->lookupValue("state_dir", state_dir);

        if (config->exists("Usb"))
        {
            const libconfig::Setting &usb = config->lookup("Usb");

            if (usb.exists("mount_dir"))
                usb.lookupValue("mount_dir", usb_mount_dir);

            if (usb.exists("firmware_file"))
                usb.lookupValue("firmware_file", usb_firmware_file);
        }

        if (!config->exists("Player"))
            return;

        libconfig::Setting &player = config->lookup("Player");

        if (player.exists("name"))
            player.lookupValue("name", player_name);

        if (player.exists("command"))
            player.lookupValue("command", player_command);

        if (player.exists("user"))
            player.lookupValue("user", player_user);

        if (player.exists("heartbeat_timeout"))
            player.lookupValue("heartbeat_timeout", heartbeat_timeout);

        if (player.exists("reboot_after_crashes"))
            player.lookupValue("reboot_after_crashes", reboot_after_crashes);

        // List of "KEY=VALUE" strings, e.g. environment = ["DISPLAY=:0"];
        if (player.exists("environment"))
        {
            const libconfig::Setting &environment = player["environment"];
            for (int i = 0; i < environment.getLength(); i++)
                player_environment.emplace_back(environment[i].c_str());
        }
    }
    catch (const libconfig::ParseException &e)
    {
        std::cerr << "Parse error at " << e.getFile() << ":" << e.getLine() << " - " << e.getError() << std::endl;
    }
}

std::string Configuration::getAppName() const
{
    return app_name;
}

std::string Configuration::getMessageQueue() const
{
    return message_queue;
}

std::string Configuration::getStateDir() const
{
    return state_dir;
}

std::string Configuration::getPlayerName() const
{
    return player_name;
}

std::string Configuration::getPlayerCommand() const
{
    return player_command;
}

std::string Configuration::getPlayerUser() const
{
    return player_user;
}

std::vector<std::string> Configuration::getPlayerEnvironment() const
{
    return player_environment;
}

int Configuration::getHeartbeatTimeout() const
{
    return heartbeat_timeout;
}

int Configuration::getRebootAfterCrashes() const
{
    return reboot_after_crashes;
}

std::string Configuration::getUsbMountDir() const
{
    return usb_mount_dir;
}

std::string Configuration::getUsbFirmwareFile() const
{
    return usb_firmware_file;
}
