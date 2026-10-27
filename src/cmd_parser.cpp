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
#include <sstream>

namespace
{
struct Option
{
        const char *name;
        const char *argument; // nullptr for options without argument
        const char *description;
};

// All options of the daemon. The values override the config file.
constexpr Option OPTIONS[] = {
    {"--config", "<file>", "read the configuration from file"},
    {"--socket", "<path>", "Unix socket for the player (config: socket)"},
    {"--player-command", "<command>", "command that starts the player (config: Player.command)"},
    {"--heartbeat-timeout", "<seconds>",
     "restart the player without heartbeat, 0 = off (config: Player.heartbeat_timeout)"},
    {"--state-dir", "<dir>", "directory for files that survive a reboot (config: state_dir)"},
    {"--usb-mount-dir", "<dir>", "USB sticks are mounted below this directory (config: Usb.mount_dir)"},
    {"--help", nullptr, "show this help and exit"},
    {"--version", nullptr, "show the version and exit"},
};

const Option *findOption(const std::string &name)
{
    auto option = std::ranges::find(OPTIONS, name, &Option::name);
    return option != std::end(OPTIONS) ? option : nullptr;
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

        if (option->argument == nullptr)
        {
            values_[token] = "";
            continue;
        }

        if (i + 1 >= argc)
        {
            error_ = "option " + token + " needs a value " + option->argument;
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
    std::ostringstream text;
    text << "Usage: garlic-daemon [options]\n\nOptions:\n";
    for (const Option &option : OPTIONS)
    {
        std::string name =
            std::string(option.name) + (option.argument != nullptr ? std::string(" ") + option.argument : "");
        text << "  " << name << std::string(name.size() < 32 ? 32 - name.size() : 1, ' ') << option.description << "\n";
    }
    text << "\nOptions override the values of the config file.\n";
    return text.str();
}
