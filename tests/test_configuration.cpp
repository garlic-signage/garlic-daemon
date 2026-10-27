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
    EXPECT_EQ(configuration.getRebootAfterCrashes(), 7);
}
} // namespace

TEST(Configuration, DefaultValues)
{
    Configuration configuration;

    EXPECT_EQ(configuration.getAppName(), "garlic-daemon");
    EXPECT_EQ(configuration.getSocketPath(), "/run/garlic-daemon/garlic-daemon.sock");
    EXPECT_EQ(configuration.getStateDir(), "/var/lib/garlic-daemon");
    EXPECT_EQ(configuration.getUsbMountDir(), "/media/garlic");
    EXPECT_EQ(configuration.getUsbFirmwareFile(), "garlic-player.AppImage");
    expectPlayerDefaults(configuration);
}

TEST(Configuration, ReadsAllValues)
{
    Configuration configuration = readConfiguration("app_name = \"my-daemon\";"
                                                    "socket = \"/tmp/my.sock\";"
                                                    "state_dir = \"/tmp/state\";"
                                                    "Usb = {"
                                                    "  mount_dir = \"/mnt/usb\";"
                                                    "  firmware_file = \"firmware.zip\";"
                                                    "};"
                                                    "Player = {"
                                                    "  name = \"my-player\";"
                                                    "  command = \"/opt/player/start.sh --fullscreen\";"
                                                    "  user = \"tester\";"
                                                    "  environment = [\"DISPLAY=:0\", \"LANG=de_DE.UTF-8\"];"
                                                    "  heartbeat_timeout = 60;"
                                                    "  reboot_after_crashes = 3;"
                                                    "};");

    EXPECT_EQ(configuration.getAppName(), "my-daemon");
    EXPECT_EQ(configuration.getSocketPath(), "/tmp/my.sock");
    EXPECT_EQ(configuration.getPlayerName(), "my-player");
    EXPECT_EQ(configuration.getPlayerCommand(), "/opt/player/start.sh --fullscreen");
    EXPECT_EQ(configuration.getPlayerUser(), "tester");
    EXPECT_EQ(configuration.getPlayerEnvironment(), (std::vector<std::string>{"DISPLAY=:0", "LANG=de_DE.UTF-8"}));
    EXPECT_EQ(configuration.getHeartbeatTimeout(), 60);
    EXPECT_EQ(configuration.getRebootAfterCrashes(), 3);
    EXPECT_EQ(configuration.getStateDir(), "/tmp/state");
    EXPECT_EQ(configuration.getUsbMountDir(), "/mnt/usb");
    EXPECT_EQ(configuration.getUsbFirmwareFile(), "firmware.zip");
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
    EXPECT_EQ(configuration.getSocketPath(), "/run/garlic-daemon/garlic-daemon.sock");
    EXPECT_EQ(configuration.getPlayerName(), "garlic-player");
    EXPECT_EQ(configuration.getPlayerCommand(), "/opt/garlic-player/garlic.sh");
    EXPECT_EQ(configuration.getPlayerUser(), "digitalsignage");
    EXPECT_EQ(configuration.getPlayerEnvironment(), (std::vector<std::string>{"DISPLAY=:0"}));
    EXPECT_EQ(configuration.getHeartbeatTimeout(), 0);
    EXPECT_EQ(configuration.getRebootAfterCrashes(), 7);
    EXPECT_EQ(configuration.getStateDir(), "/var/lib/garlic-daemon");
    EXPECT_EQ(configuration.getUsbMountDir(), "/media/garlic");
    EXPECT_EQ(configuration.getUsbFirmwareFile(), "garlic-player.AppImage");
}

namespace
{
CmdParser parseCommandLine(std::vector<std::string> arguments)
{
    arguments.insert(arguments.begin(), "garlic-daemon");
    std::vector<char *> argv;
    for (std::string &argument : arguments)
        argv.push_back(argument.data());
    return CmdParser(static_cast<int>(argv.size()), argv.data());
}
} // namespace

TEST(Configuration, CommandLineOverridesConfigFile)
{
    Configuration configuration = readConfiguration("socket = \"/from/file.sock\";"
                                                    "state_dir = \"/from/file\";"
                                                    "Usb = { mount_dir = \"/from/file/usb\"; };"
                                                    "Player = { command = \"from-file\"; heartbeat_timeout = 60; "
                                                    "user = \"file-user\"; };");

    configuration.applyCommandLine(
        parseCommandLine({"--socket", "/tmp/garlic.sock", "--player-command", "sleep 1000", "--heartbeat-timeout", "5",
                          "--state-dir", "/tmp/state", "--usb-mount-dir", "/tmp/usb"}));

    EXPECT_EQ(configuration.getSocketPath(), "/tmp/garlic.sock");
    EXPECT_EQ(configuration.getPlayerCommand(), "sleep 1000");
    EXPECT_EQ(configuration.getHeartbeatTimeout(), 5);
    EXPECT_EQ(configuration.getStateDir(), "/tmp/state");
    EXPECT_EQ(configuration.getUsbMountDir(), "/tmp/usb");
    EXPECT_EQ(configuration.getPlayerUser(), "file-user"); // no option for it, the file value stays
}

TEST(Configuration, CommandLineWithoutOptionsKeepsValues)
{
    Configuration configuration = readConfiguration("Player = { command = \"from-file\"; };");

    configuration.applyCommandLine(parseCommandLine({"--config", "/etc/garlic-daemon/garlic-daemon.conf"}));

    EXPECT_EQ(configuration.getPlayerCommand(), "from-file");
    EXPECT_EQ(configuration.getSocketPath(), "/run/garlic-daemon/garlic-daemon.sock");
}

TEST(Configuration, HeartbeatTimeoutZeroIsAllowed)
{
    Configuration configuration;

    configuration.applyCommandLine(parseCommandLine({"--heartbeat-timeout", "0"}));

    EXPECT_EQ(configuration.getHeartbeatTimeout(), 0);
}

TEST(Configuration, InvalidHeartbeatTimeoutThrows)
{
    for (const char *value : {"abc", "-5", "10s", ""})
    {
        Configuration configuration;
        EXPECT_THROW(configuration.applyCommandLine(parseCommandLine({"--heartbeat-timeout", value})),
                     std::runtime_error)
            << value;
    }
}
