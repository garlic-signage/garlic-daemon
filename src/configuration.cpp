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
#include <format>
#include <stdexcept>
#include "log.hpp"

Configuration::Configuration() = default;

namespace
{
/// Reads a value if it exists. A wrong type, e.g. heartbeat_timeout = "60", is an error,
/// otherwise the default would be used without notice.
template <typename T> void readValue(const libconfig::Config *config, const char *path, T &value)
{
    if (config->exists(path) && !config->lookupValue(path, value))
        throw std::runtime_error(std::format("Config value {} has the wrong type", path));
}
} // namespace

void Configuration::readLibConfigValues(const libconfig::Config *config)
{
    logInfo("Start read values from config file");

    readValue(config, "app_name", app_name_);
    readValue(config, "socket", socket_path_);
    readValue(config, "state_dir", state_dir_);

    readValue(config, "Usb.mount_dir", usb_mount_dir_);
    readValue(config, "Usb.firmware_file", usb_firmware_file_);

    readValue(config, "Player.name", player_name_);
    readValue(config, "Player.command", player_command_);
    readValue(config, "Player.user", player_user_);
    readValue(config, "Player.heartbeat_timeout", heartbeat_timeout_);
    readValue(config, "Player.reboot_after_crashes", reboot_after_crashes_);

    // List of "KEY=VALUE" strings, e.g. environment = ["DISPLAY=:0"];
    if (!config->exists("Player.environment"))
        return;

    const libconfig::Setting &environment = config->lookup("Player.environment");
    if (!environment.isArray() && !environment.isList())
        throw std::runtime_error("Player.environment needs a list like [\"KEY=VALUE\"]");

    for (int i = 0; i < environment.getLength(); i++)
    {
        // Without a name before "=" the watchdog could not replace a variable of the same name
        const libconfig::Setting &setting = environment[i];
        std::string entry                 = setting.getType() == libconfig::Setting::TypeString ? setting.c_str() : "";
        if (entry.find('=') == std::string::npos || entry.starts_with('='))
            throw std::runtime_error("Player.environment needs entries like \"KEY=VALUE\", not '" + entry + "'");
        player_environment_.push_back(entry);
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
