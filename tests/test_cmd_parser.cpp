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
#include "cmd_parser.hpp"

// CmdParser expects argv like main() gets it: argv[0] is the program name.

TEST(CmdParser, NoArgumentsHasNoConfigOption)
{
    char *argv[] = {(char *)"garlic-daemon"};
    CmdParser parser(1, argv);

    EXPECT_FALSE(parser.checkForConfigOption());
    EXPECT_EQ(parser.getConfigFilePath(), "");
}

TEST(CmdParser, ConfigOptionWithValue)
{
    char *argv[] = {(char *)"garlic-daemon", (char *)"--config", (char *)"/etc/garlic-daemon/garlic-daemon.conf"};
    CmdParser parser(3, argv);

    EXPECT_TRUE(parser.checkForConfigOption());
    EXPECT_EQ(parser.getConfigFilePath(), "/etc/garlic-daemon/garlic-daemon.conf");
}

TEST(CmdParser, ConfigOptionBetweenOtherArguments)
{
    char *argv[] = {(char *)"garlic-daemon", (char *)"--foo", (char *)"--config", (char *)"my.conf", (char *)"--bar"};
    CmdParser parser(5, argv);

    EXPECT_TRUE(parser.checkForConfigOption());
    EXPECT_EQ(parser.getConfigFilePath(), "my.conf");
}

TEST(CmdParser, ConfigOptionWithoutValueGivesEmptyPath)
{
    char *argv[] = {(char *)"garlic-daemon", (char *)"--config"};
    CmdParser parser(2, argv);

    EXPECT_TRUE(parser.checkForConfigOption());
    EXPECT_EQ(parser.getConfigFilePath(), "");
}

TEST(CmdParser, ProgramNameIsNotParsedAsOption)
{
    char *argv[] = {(char *)"--config", (char *)"other"};
    CmdParser parser(2, argv);

    // argv[0] is skipped, so only "other" is a token
    EXPECT_FALSE(parser.checkForConfigOption());
}
