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
#include <filesystem>
#include <fstream>
#include "usb_content_handler.hpp"
#include "fake_usb_actions.hpp"

namespace fs = std::filesystem;

// A temporary directory plays the root directory of a mounted USB stick
class UsbContentHandlerTest : public ::testing::Test
{
    protected:
        fs::path stick;
        FakeUsbActions actions;

        void SetUp() override
        {
            char dir_template[] = "/tmp/garlic-test-usb-XXXXXX";
            stick               = mkdtemp(dir_template);
        }

        void TearDown() override { fs::remove_all(stick); }

        void addFile(const fs::path &relative)
        {
            fs::create_directories((stick / relative).parent_path());
            std::ofstream(stick / relative) << "content";
        }

        void handle(const Configuration &configuration = Configuration())
        {
            UsbContentHandler handler(configuration, actions);
            handler.handle(stick);
        }
};

TEST_F(UsbContentHandlerTest, EmptyStickDoesNothing)
{
    handle();

    EXPECT_TRUE(actions.calls.empty());
}

TEST_F(UsbContentHandlerTest, ConfigXmlIsApplied)
{
    addFile("config.xml");

    handle();

    EXPECT_EQ(actions.calls, std::vector<std::string>{"applyConfig " + (stick / "config.xml").string()});
}

TEST_F(UsbContentHandlerTest, FirmwareUpdatesPlayer)
{
    addFile("garlic-player.zip");

    handle();

    EXPECT_EQ(actions.calls, std::vector<std::string>{"updatePlayer " + (stick / "garlic-player.zip").string()});
}

TEST_F(UsbContentHandlerTest, FirmwareFileNameFromConfiguration)
{
    addFile("garlic-player.zip"); // default name, ignored
    addFile("firmware-2.0.tar.gz");
    libconfig::Config config;
    config.readString("Usb = { firmware_file = \"firmware-2.0.tar.gz\"; };");
    Configuration configuration;
    configuration.readLibConfigValues(&config);

    handle(configuration);

    EXPECT_EQ(actions.calls, std::vector<std::string>{"updatePlayer " + (stick / "firmware-2.0.tar.gz").string()});
}

TEST_F(UsbContentHandlerTest, SmilIndexIsPlayed)
{
    addFile("SMIL/index.smil");

    handle();

    EXPECT_EQ(actions.calls, std::vector<std::string>{"playSmil " + (stick / "SMIL/index.smil").string()});
}

TEST_F(UsbContentHandlerTest, IndexSmilInRootIsIgnored)
{
    addFile("index.smil");

    handle();

    EXPECT_TRUE(actions.calls.empty());
}

TEST_F(UsbContentHandlerTest, AllFilesInFixedOrder)
{
    addFile("SMIL/index.smil");
    addFile("garlic-player.zip");
    addFile("config.xml");

    handle();

    EXPECT_EQ(actions.calls, (std::vector<std::string>{"applyConfig " + (stick / "config.xml").string(),
                                                       "updatePlayer " + (stick / "garlic-player.zip").string(),
                                                       "playSmil " + (stick / "SMIL/index.smil").string()}));
}

TEST_F(UsbContentHandlerTest, DirectoryWithFileNameIsIgnored)
{
    fs::create_directories(stick / "config.xml");

    handle();

    EXPECT_TRUE(actions.calls.empty());
}

TEST_F(UsbContentHandlerTest, SymlinksAreIgnored)
{
    // A stick with ext4 could link to files of the system
    fs::create_symlink("/etc/hostname", stick / "config.xml");
    fs::create_directory_symlink("/etc", stick / "SMIL");

    handle();

    EXPECT_TRUE(actions.calls.empty());
}
