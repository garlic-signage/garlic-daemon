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

#include <gtest/gtest.h>
#include <vector>
#include "cmd_parser.hpp"

namespace
{
// Builds argc/argv like main() gets them: argv[0] is the program name
CmdParser parse(std::vector<std::string> arguments)
{
    arguments.insert(arguments.begin(), "garlic-daemon");
    std::vector<char *> argv;
    for (std::string &argument : arguments)
        argv.push_back(argument.data());
    return CmdParser(static_cast<int>(argv.size()), argv.data());
}
} // namespace

TEST(CmdParser, NoArguments)
{
    CmdParser parser = parse({});

    EXPECT_EQ(parser.getError(), "");
    EXPECT_FALSE(parser.has("--config"));
    EXPECT_EQ(parser.value("--config"), "");
}

TEST(CmdParser, OptionWithValue)
{
    CmdParser parser = parse({"--config", "/etc/garlic-daemon/garlic-daemon.conf"});

    EXPECT_EQ(parser.getError(), "");
    EXPECT_TRUE(parser.has("--config"));
    EXPECT_EQ(parser.value("--config"), "/etc/garlic-daemon/garlic-daemon.conf");
}

TEST(CmdParser, SeveralOptions)
{
    CmdParser parser = parse({"--socket", "/tmp/garlic.sock", "--player-command", "sleep 1000", "--heartbeat-timeout",
                              "60", "--state-dir", "/tmp/state", "--usb-mount-dir", "/tmp/usb"});

    EXPECT_EQ(parser.getError(), "");
    EXPECT_EQ(parser.value("--socket"), "/tmp/garlic.sock");
    EXPECT_EQ(parser.value("--player-command"), "sleep 1000"); // one token, the shell passes it quoted
    EXPECT_EQ(parser.value("--heartbeat-timeout"), "60");
    EXPECT_EQ(parser.value("--state-dir"), "/tmp/state");
    EXPECT_EQ(parser.value("--usb-mount-dir"), "/tmp/usb");
}

TEST(CmdParser, OptionsWithoutValue)
{
    CmdParser parser = parse({"--help", "--version"});

    EXPECT_EQ(parser.getError(), "");
    EXPECT_TRUE(parser.has("--help"));
    EXPECT_TRUE(parser.has("--version"));
    EXPECT_FALSE(parser.has("--config"));
}

TEST(CmdParser, LastValueWins)
{
    CmdParser parser = parse({"--socket", "/tmp/first.sock", "--socket", "/tmp/second.sock"});

    EXPECT_EQ(parser.value("--socket"), "/tmp/second.sock");
}

TEST(CmdParser, MissingValueIsError)
{
    CmdParser parser = parse({"--config"});

    EXPECT_EQ(parser.getError(), "option --config needs a value <file>");
}

TEST(CmdParser, UnknownOptionIsError)
{
    CmdParser parser = parse({"--player-comand", "sleep 1000"}); // typo

    EXPECT_EQ(parser.getError(), "unknown option --player-comand");
}

TEST(CmdParser, ArgumentWithoutOptionIsError)
{
    CmdParser parser = parse({"/etc/garlic-daemon/garlic-daemon.conf"});

    EXPECT_EQ(parser.getError(), "unknown option /etc/garlic-daemon/garlic-daemon.conf");
}

TEST(CmdParser, ProgramNameIsNotParsed)
{
    char *argv[] = {(char *)"--config", (char *)"--help"};
    CmdParser parser(2, argv);

    // argv[0] is skipped, so only --help is parsed
    EXPECT_EQ(parser.getError(), "");
    EXPECT_FALSE(parser.has("--config"));
    EXPECT_TRUE(parser.has("--help"));
}

TEST(CmdParser, HelpTextListsAllOptions)
{
    std::string help = CmdParser::helpText();

    for (const char *option :
         {"--config <file>", "--socket <path>", "--player-command <command>", "--heartbeat-timeout <seconds>",
          "--state-dir <dir>", "--usb-mount-dir <dir>", "--help", "--version"})
        EXPECT_NE(help.find(option), std::string::npos) << option;
}
