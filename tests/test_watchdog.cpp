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
#include "watchdog.hpp"

namespace fs = std::filesystem;
using namespace std::string_literals; // "..."s keeps embedded '\0'

// Every test gets its own empty fake /proc directory, which is removed afterwards.
class WatchdogTest : public ::testing::Test
{
    protected:
        fs::path proc_dir;

        void SetUp() override
        {
            char dir_template[] = "/tmp/garlic-test-proc-XXXXXX";
            proc_dir            = mkdtemp(dir_template);
        }

        void TearDown() override { fs::remove_all(proc_dir); }

        // Creates <proc_dir>/<entry>/cmdline. Like in the real /proc,
        // arguments in cmdline are separated by '\0'.
        void addProcess(const std::string &entry, const std::string &cmdline)
        {
            fs::create_directory(proc_dir / entry);
            std::ofstream(proc_dir / entry / "cmdline") << cmdline;
        }
};

TEST_F(WatchdogTest, EmptyProcMeansNotRunning)
{
    Watchdog watchdog(Configuration(), proc_dir);

    EXPECT_FALSE(watchdog.isPlayerRunning());
}

TEST_F(WatchdogTest, MissingProcMeansNotRunning)
{
    Watchdog watchdog(Configuration(), proc_dir / "does-not-exist");

    EXPECT_FALSE(watchdog.isPlayerRunning());
}

TEST_F(WatchdogTest, FindsPlayerWithPathAndArguments)
{
    addProcess("1", "/sbin/init\0"s);
    addProcess("42", "/opt/garlic-player/garlic-player\0--fullscreen\0"s);
    Watchdog watchdog(Configuration(), proc_dir);

    EXPECT_TRUE(watchdog.isPlayerRunning());
}

TEST_F(WatchdogTest, FindsPlayerWithoutPath)
{
    addProcess("42", "garlic-player");
    Watchdog watchdog(Configuration(), proc_dir);

    EXPECT_TRUE(watchdog.isPlayerRunning());
}

TEST_F(WatchdogTest, OtherProcessesAreIgnored)
{
    addProcess("1", "/sbin/init");
    addProcess("2", "/usr/bin/garlic-player-helper"); // only the exact name counts
    addProcess("3", "");                              // kernel threads have an empty cmdline
    Watchdog watchdog(Configuration(), proc_dir);

    EXPECT_FALSE(watchdog.isPlayerRunning());
}

TEST_F(WatchdogTest, NonNumericEntriesAreIgnored)
{
    addProcess("self", "/opt/garlic-player/garlic-player");
    Watchdog watchdog(Configuration(), proc_dir);

    EXPECT_FALSE(watchdog.isPlayerRunning());
}

TEST_F(WatchdogTest, PlayerNameIsTakenFromConfiguration)
{
    libconfig::Config config;
    config.readString("Player = { name = \"my-player\"; };");
    Configuration configuration;
    configuration.readLibConfigValues(&config);

    addProcess("42", "/opt/garlic-player/garlic-player");
    Watchdog watchdog(configuration, proc_dir);
    EXPECT_FALSE(watchdog.isPlayerRunning());

    addProcess("43", "/usr/bin/my-player");
    EXPECT_TRUE(watchdog.isPlayerRunning());
}

TEST_F(WatchdogTest, InactiveWatchdogAlwaysReportsRunning)
{
    Watchdog watchdog(Configuration(), proc_dir);
    watchdog.setActivity(false);

    EXPECT_TRUE(watchdog.isPlayerRunning());
}
