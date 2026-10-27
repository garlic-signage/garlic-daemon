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

#include <charconv>
#include <stdexcept>
#include "log.hpp"

Configuration::Configuration() = default;

void Configuration::readLibConfigValues(const libconfig::Config *config)
{
    try
    {

        logInfo("Start read values from config file");

        if (config->exists("app_name"))
            config->lookupValue("app_name", app_name_);

        if (config->exists("socket"))
            config->lookupValue("socket", socket_path_);

        if (config->exists("state_dir"))
            config->lookupValue("state_dir", state_dir_);

        if (config->exists("Usb"))
        {
            const libconfig::Setting &usb = config->lookup("Usb");

            if (usb.exists("mount_dir"))
                usb.lookupValue("mount_dir", usb_mount_dir_);

            if (usb.exists("firmware_file"))
                usb.lookupValue("firmware_file", usb_firmware_file_);
        }

        if (!config->exists("Player"))
            return;

        libconfig::Setting &player = config->lookup("Player");

        if (player.exists("name"))
            player.lookupValue("name", player_name_);

        if (player.exists("command"))
            player.lookupValue("command", player_command_);

        if (player.exists("user"))
            player.lookupValue("user", player_user_);

        if (player.exists("heartbeat_timeout"))
            player.lookupValue("heartbeat_timeout", heartbeat_timeout_);

        if (player.exists("reboot_after_crashes"))
            player.lookupValue("reboot_after_crashes", reboot_after_crashes_);

        // List of "KEY=VALUE" strings, e.g. environment = ["DISPLAY=:0"];
        if (player.exists("environment"))
        {
            const libconfig::Setting &environment = player["environment"];
            for (int i = 0; i < environment.getLength(); i++)
                player_environment_.emplace_back(environment[i].c_str());
        }
    }
    catch (const libconfig::ParseException &e)
    {
        logError("Parse error at {}:{} - {}", e.getFile(), e.getLine(), e.getError());
    }
}

void Configuration::applyCommandLine(const CmdParser &cmd_parser)
{
    if (cmd_parser.has("--socket"))
        socket_path_ = cmd_parser.value("--socket");

    if (cmd_parser.has("--player-command"))
        player_command_ = cmd_parser.value("--player-command");

    if (cmd_parser.has("--state-dir"))
        state_dir_ = cmd_parser.value("--state-dir");

    if (cmd_parser.has("--usb-mount-dir"))
        usb_mount_dir_ = cmd_parser.value("--usb-mount-dir");

    if (cmd_parser.has("--heartbeat-timeout"))
    {
        // Only a whole, not negative number, e.g. "10s" or "-1" are errors
        std::string text  = cmd_parser.value("--heartbeat-timeout");
        int seconds       = 0;
        auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), seconds);
        if (error != std::errc() || end != text.data() + text.size() || seconds < 0)
            throw std::runtime_error("--heartbeat-timeout needs a number of seconds, not '" + text + "'");
        heartbeat_timeout_ = seconds;
    }
}

std::string Configuration::getAppName() const
{
    return app_name_;
}

std::string Configuration::getSocketPath() const
{
    return socket_path_;
}

std::string Configuration::getStateDir() const
{
    return state_dir_;
}

std::string Configuration::getPlayerName() const
{
    return player_name_;
}

std::string Configuration::getPlayerCommand() const
{
    return player_command_;
}

std::string Configuration::getPlayerUser() const
{
    return player_user_;
}

std::vector<std::string> Configuration::getPlayerEnvironment() const
{
    return player_environment_;
}

int Configuration::getHeartbeatTimeout() const
{
    return heartbeat_timeout_;
}

int Configuration::getRebootAfterCrashes() const
{
    return reboot_after_crashes_;
}

std::string Configuration::getUsbMountDir() const
{
    return usb_mount_dir_;
}

std::string Configuration::getUsbFirmwareFile() const
{
    return usb_firmware_file_;
}
