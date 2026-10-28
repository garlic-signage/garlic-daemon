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

#include "cmd_parser.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <string_view>

namespace
{
struct Option
{
        std::string_view name;
        std::string_view argument; // empty for options without argument
        std::string_view description;
};

// All options of the daemon. The values override the config file.
constexpr auto OPTIONS = std::to_array<Option>({
    {"--config", "<file>", "read the configuration from file"},
    {"--socket", "<path>", "Unix socket for the player (config: socket)"},
    {"--player-command", "<command>", "command that starts the player (config: Player.command)"},
    {"--heartbeat-timeout", "<seconds>",
     "restart the player without heartbeat, 0 = off (config: Player.heartbeat_timeout)"},
    {"--state-dir", "<dir>", "directory for files that survive a reboot (config: state_dir)"},
    {"--usb-mount-dir", "<dir>",
     "USB sticks are mounted below this directory, it belongs to the daemon alone (config: Usb.mount_dir)"},
    {"--help", "", "show this help and exit"},
    {"--version", "", "show the version and exit"},
});

const Option *findOption(const std::string &name)
{
    auto option = std::ranges::find(OPTIONS, name, &Option::name);
    return option != OPTIONS.end() ? &*option : nullptr;
}
} // namespace

CmdParser::CmdParser(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) // argv[0] is the program name
    {
        std::string token    = argv[i];
        const Option *option = findOption(token);
        if (option == nullptr)
        {
            error_ = "unknown option " + token;
            return;
        }

        if (option->argument.empty())
        {
            values_[token] = "";
            continue;
        }

        if (i + 1 >= argc)
        {
            error_ = std::format("option {} needs a value {}", token, option->argument);
            return;
        }
        values_[token] = argv[++i];
    }
}

std::string CmdParser::getError() const
{
    return error_;
}

bool CmdParser::has(const std::string &option) const
{
    return values_.contains(option);
}

std::string CmdParser::value(const std::string &option) const
{
    auto entry = values_.find(option);
    return entry != values_.end() ? entry->second : "";
}

std::string CmdParser::helpText()
{
    std::string text = "Usage: garlic-daemon [options]\n\nOptions:\n";
    for (const Option &option : OPTIONS)
    {
        std::string name =
            option.argument.empty() ? std::string(option.name) : std::format("{} {}", option.name, option.argument);
        // The descriptions start in one column, a longer name gets at least one space
        text += std::format("  {:<31} {}\n", name, option.description);
    }
    text += "\nOptions override the values of the config file.\n";
    return text;
}
