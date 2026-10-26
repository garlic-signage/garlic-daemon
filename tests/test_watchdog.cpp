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
#include <chrono>
#include <csignal>
#include <filesystem>
#include <fstream>
#include <thread>
#include "watchdog.hpp"

namespace fs = std::filesystem;
using namespace std::chrono_literals;

// The tests start real processes like "sleep 30" instead of the player.
// They run as normal user, so the player is started as the same user.
class WatchdogTest : public ::testing::Test
{
    protected:
        fs::path tmp_dir;

        void SetUp() override
        {
            char dir_template[] = "/tmp/garlic-test-watchdog-XXXXXX";
            tmp_dir             = mkdtemp(dir_template);
        }

        void TearDown() override { fs::remove_all(tmp_dir); }

        // Creates a Configuration with the given Player settings in libconfig syntax
        static Configuration makeConfiguration(const std::string &player_settings)
        {
            libconfig::Config config;
            config.readString("Player = { " + player_settings + " };");
            Configuration configuration;
            configuration.readLibConfigValues(&config);
            return configuration;
        }

        // true if a process with this pid exists
        static bool processExists(pid_t pid) { return kill(pid, 0) == 0; }

        // Reads a file written by the test command, waits at most 2 seconds for it
        static std::string readFileWithTimeout(const fs::path &file)
        {
            auto deadline = std::chrono::steady_clock::now() + 2s;
            std::string content;
            while (content.empty() && std::chrono::steady_clock::now() < deadline)
            {
                std::this_thread::sleep_for(10ms);
                std::ifstream(file) >> content;
            }
            return content;
        }
};

TEST_F(WatchdogTest, CheckStartsPlayer)
{
    Watchdog watchdog(makeConfiguration("command = \"sleep 30\";"));
    EXPECT_FALSE(watchdog.isPlayerRunning());

    watchdog.check();

    EXPECT_TRUE(watchdog.isPlayerRunning());
    EXPECT_TRUE(processExists(watchdog.getPlayerPid()));
}

TEST_F(WatchdogTest, CheckDoesNotStartSecondPlayer)
{
    Watchdog watchdog(makeConfiguration("command = \"sleep 30\";"));
    watchdog.check();
    pid_t pid = watchdog.getPlayerPid();

    watchdog.check();

    EXPECT_EQ(watchdog.getPlayerPid(), pid);
}

TEST_F(WatchdogTest, CrashedPlayerIsRestartedAfterBackoff)
{
    // Takes about 5 seconds, the first backoff delay
    Watchdog watchdog(makeConfiguration("command = \"sleep 30\";"));
    watchdog.check();
    pid_t first_pid = watchdog.getPlayerPid();

    kill(first_pid, SIGKILL); // simulates a crash shortly after the start
    std::this_thread::sleep_for(100ms);
    watchdog.check();
    EXPECT_FALSE(watchdog.isPlayerRunning()); // backoff, no immediate restart

    std::this_thread::sleep_for(5100ms);
    watchdog.check();

    EXPECT_TRUE(watchdog.isPlayerRunning());
    EXPECT_NE(watchdog.getPlayerPid(), first_pid);
}

TEST_F(WatchdogTest, HangingPlayerIsStopped)
{
    Watchdog watchdog(makeConfiguration("command = \"sleep 30\"; heartbeat_timeout = 1;"));
    watchdog.check();
    pid_t first_pid = watchdog.getPlayerPid();

    std::this_thread::sleep_for(1200ms); // no heartbeat
    watchdog.check();

    // The hang counts like a crash, so the restart comes after the backoff, see test above
    EXPECT_FALSE(processExists(first_pid));
    EXPECT_FALSE(watchdog.isPlayerRunning());
}

TEST_F(WatchdogTest, StoppedPlayerIsStartedWithoutDelay)
{
    Watchdog watchdog(makeConfiguration("command = \"sleep 30\";"));
    watchdog.check();

    watchdog.stopPlayer(); // e.g. for an update, this is no crash
    watchdog.check();

    EXPECT_TRUE(watchdog.isPlayerRunning());
}

TEST(Watchdog, RestartDelayDoublesUpToMaximum)
{
    EXPECT_EQ(Watchdog::restartDelay(0), 0s);
    EXPECT_EQ(Watchdog::restartDelay(1), 5s);
    EXPECT_EQ(Watchdog::restartDelay(2), 10s);
    EXPECT_EQ(Watchdog::restartDelay(3), 20s);
    EXPECT_EQ(Watchdog::restartDelay(4), 40s);
    EXPECT_EQ(Watchdog::restartDelay(5), 80s);
    EXPECT_EQ(Watchdog::restartDelay(6), 160s);
    EXPECT_EQ(Watchdog::restartDelay(7), 300s); // maximum
    EXPECT_EQ(Watchdog::restartDelay(1000), 300s);
}

TEST_F(WatchdogTest, HeartbeatKeepsPlayerRunning)
{
    Watchdog watchdog(makeConfiguration("command = \"sleep 30\"; heartbeat_timeout = 1;"));
    watchdog.check();
    pid_t pid = watchdog.getPlayerPid();

    for (int i = 0; i < 3; i++) // 1.5 seconds in total, longer than the timeout
    {
        std::this_thread::sleep_for(500ms);
        watchdog.heartbeat();
        watchdog.check();
    }

    EXPECT_EQ(watchdog.getPlayerPid(), pid);
}

TEST_F(WatchdogTest, NoHeartbeatTimeoutMeansNoHangDetection)
{
    Watchdog watchdog(makeConfiguration("command = \"sleep 30\"; heartbeat_timeout = 0;"));
    watchdog.check();
    pid_t pid = watchdog.getPlayerPid();

    std::this_thread::sleep_for(1200ms);
    watchdog.check();

    EXPECT_EQ(watchdog.getPlayerPid(), pid);
}

TEST_F(WatchdogTest, InactiveWatchdogDoesNotStartPlayer)
{
    Watchdog watchdog(makeConfiguration("command = \"sleep 30\";"));
    watchdog.setActivity(false);

    watchdog.check();

    EXPECT_FALSE(watchdog.isPlayerRunning());
}

TEST_F(WatchdogTest, StopPlayerStopsAllProcessesOfStartScript)
{
    // Like a start script: starts a child process and waits for it
    fs::path pid_file = tmp_dir / "child.pid";
    Watchdog watchdog(makeConfiguration("command = \"sleep 30 & echo $! > " + pid_file.string() + "; wait\";"));
    watchdog.check();
    pid_t child_pid = std::stoi(readFileWithTimeout(pid_file));
    ASSERT_TRUE(processExists(child_pid));

    watchdog.stopPlayer();

    EXPECT_FALSE(watchdog.isPlayerRunning());
    std::this_thread::sleep_for(100ms); // the child is reaped by init
    EXPECT_FALSE(processExists(child_pid));
}

TEST_F(WatchdogTest, PlayerIgnoringSigtermIsKilled)
{
    // Takes STOP_TIMEOUT (5 seconds), because the watchdog waits before it sends SIGKILL
    fs::path ready_file = tmp_dir / "ready";
    Watchdog watchdog(
        makeConfiguration("command = \"trap '' TERM; echo ready > " + ready_file.string() + "; sleep 30\";"));
    watchdog.check();
    pid_t pid = watchdog.getPlayerPid();
    ASSERT_EQ(readFileWithTimeout(ready_file), "ready"); // SIGTERM is ignored from now on

    auto begin = std::chrono::steady_clock::now();
    watchdog.stopPlayer();

    EXPECT_FALSE(processExists(pid));
    EXPECT_GE(std::chrono::steady_clock::now() - begin, 5s); // SIGTERM had no effect, SIGKILL was needed
}

TEST_F(WatchdogTest, DestructorStopsPlayer)
{
    pid_t pid = -1;
    {
        Watchdog watchdog(makeConfiguration("command = \"sleep 30\";"));
        watchdog.check();
        pid = watchdog.getPlayerPid();
    }

    EXPECT_FALSE(processExists(pid));
}

TEST_F(WatchdogTest, EnvironmentFromConfigurationIsPassed)
{
    fs::path out_file = tmp_dir / "env.txt";
    Watchdog watchdog(makeConfiguration("command = \"echo $GARLIC_TEST > " + out_file.string() +
                                        "\"; environment = [\"GARLIC_TEST=hello\"];"));
    watchdog.check();

    EXPECT_EQ(readFileWithTimeout(out_file), "hello");
}
