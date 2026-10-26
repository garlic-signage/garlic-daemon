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
#include "configuration.hpp"

// The config content is read from a string, so no files are needed.

TEST(Configuration, DefaultValues)
{
    Configuration configuration;

    EXPECT_EQ(configuration.getAppName(), "garlic-daemon");
    EXPECT_EQ(configuration.getMessageQueue(), "/garlic-daemon");
    EXPECT_EQ(configuration.getPlayerName(), "garlic-player");
    EXPECT_EQ(configuration.getPlayerPath(), "/usr/local/bin/");
    EXPECT_EQ(configuration.getPlayerUser(), "digitalsignage");
}

TEST(Configuration, ReadsAllValues)
{
    libconfig::Config config;
    config.readString("app_name = \"my-daemon\";"
                      "message_queue = \"/my-queue\";"
                      "Player = {"
                      "  name = \"my-player\";"
                      "  path = \"/opt/player/\";"
                      "  user = \"tester\";"
                      "};");

    Configuration configuration;
    configuration.readLibConfigValues(&config);

    EXPECT_EQ(configuration.getAppName(), "my-daemon");
    EXPECT_EQ(configuration.getMessageQueue(), "/my-queue");
    EXPECT_EQ(configuration.getPlayerName(), "my-player");
    EXPECT_EQ(configuration.getPlayerPath(), "/opt/player/");
    EXPECT_EQ(configuration.getPlayerUser(), "tester");
}

TEST(Configuration, MissingPlayerGroupKeepsPlayerDefaults)
{
    libconfig::Config config;
    config.readString("app_name = \"my-daemon\";");

    Configuration configuration;
    configuration.readLibConfigValues(&config);

    EXPECT_EQ(configuration.getAppName(), "my-daemon");
    EXPECT_EQ(configuration.getPlayerName(), "garlic-player");
    EXPECT_EQ(configuration.getPlayerPath(), "/usr/local/bin/");
    EXPECT_EQ(configuration.getPlayerUser(), "digitalsignage");
}

TEST(Configuration, MissingSingleValueKeepsDefault)
{
    libconfig::Config config;
    config.readString("Player = { name = \"my-player\"; };");

    Configuration configuration;
    configuration.readLibConfigValues(&config);

    EXPECT_EQ(configuration.getAppName(), "garlic-daemon");
    EXPECT_EQ(configuration.getPlayerName(), "my-player");
    EXPECT_EQ(configuration.getPlayerPath(), "/usr/local/bin/");
    EXPECT_EQ(configuration.getPlayerUser(), "digitalsignage");
}

// CONFIG_FILE points to resource/config/garlic-daemon.conf, see tests/CMakeLists.txt
TEST(Configuration, ShippedConfigFileIsValid)
{
    libconfig::Config config;
    ASSERT_NO_THROW(config.readFile(CONFIG_FILE));

    Configuration configuration;
    configuration.readLibConfigValues(&config);

    EXPECT_EQ(configuration.getAppName(), "garlic-daemon");
    EXPECT_EQ(configuration.getMessageQueue(), "/garlic-daemon");
    EXPECT_EQ(configuration.getPlayerName(), "garlic-player");
    EXPECT_EQ(configuration.getPlayerPath(), "/usr/local/bin/");
    EXPECT_EQ(configuration.getPlayerUser(), "digitalsignage");
}
