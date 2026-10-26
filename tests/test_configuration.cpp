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

namespace
{
Configuration readConfiguration(const std::string &content)
{
    libconfig::Config config;
    config.readString(content);
    Configuration configuration;
    configuration.readLibConfigValues(&config);
    return configuration;
}

void expectPlayerDefaults(const Configuration &configuration)
{
    EXPECT_EQ(configuration.getPlayerName(), "garlic-player");
    EXPECT_EQ(configuration.getPlayerCommand(), "/opt/garlic-player/garlic.sh");
    EXPECT_EQ(configuration.getPlayerUser(), "digitalsignage");
    EXPECT_TRUE(configuration.getPlayerEnvironment().empty());
    EXPECT_EQ(configuration.getHeartbeatTimeout(), 0);
}
} // namespace

TEST(Configuration, DefaultValues)
{
    Configuration configuration;

    EXPECT_EQ(configuration.getAppName(), "garlic-daemon");
    EXPECT_EQ(configuration.getMessageQueue(), "/garlic-daemon");
    expectPlayerDefaults(configuration);
}

TEST(Configuration, ReadsAllValues)
{
    Configuration configuration = readConfiguration("app_name = \"my-daemon\";"
                                                    "message_queue = \"/my-queue\";"
                                                    "Player = {"
                                                    "  name = \"my-player\";"
                                                    "  command = \"/opt/player/start.sh --fullscreen\";"
                                                    "  user = \"tester\";"
                                                    "  environment = [\"DISPLAY=:0\", \"LANG=de_DE.UTF-8\"];"
                                                    "  heartbeat_timeout = 60;"
                                                    "};");

    EXPECT_EQ(configuration.getAppName(), "my-daemon");
    EXPECT_EQ(configuration.getMessageQueue(), "/my-queue");
    EXPECT_EQ(configuration.getPlayerName(), "my-player");
    EXPECT_EQ(configuration.getPlayerCommand(), "/opt/player/start.sh --fullscreen");
    EXPECT_EQ(configuration.getPlayerUser(), "tester");
    EXPECT_EQ(configuration.getPlayerEnvironment(), (std::vector<std::string>{"DISPLAY=:0", "LANG=de_DE.UTF-8"}));
    EXPECT_EQ(configuration.getHeartbeatTimeout(), 60);
}

TEST(Configuration, MissingPlayerGroupKeepsPlayerDefaults)
{
    Configuration configuration = readConfiguration("app_name = \"my-daemon\";");

    EXPECT_EQ(configuration.getAppName(), "my-daemon");
    expectPlayerDefaults(configuration);
}

TEST(Configuration, MissingSingleValueKeepsDefault)
{
    Configuration configuration = readConfiguration("Player = { name = \"my-player\"; };");

    EXPECT_EQ(configuration.getAppName(), "garlic-daemon");
    EXPECT_EQ(configuration.getPlayerName(), "my-player");
    EXPECT_EQ(configuration.getPlayerCommand(), "/opt/garlic-player/garlic.sh");
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
    EXPECT_EQ(configuration.getPlayerCommand(), "/opt/garlic-player/garlic.sh");
    EXPECT_EQ(configuration.getPlayerUser(), "digitalsignage");
    EXPECT_EQ(configuration.getPlayerEnvironment(), (std::vector<std::string>{"DISPLAY=:0"}));
    EXPECT_EQ(configuration.getHeartbeatTimeout(), 0);
}
