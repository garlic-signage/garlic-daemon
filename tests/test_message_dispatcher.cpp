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
#include "message_dispatcher.hpp"
#include "fake_system_actions.hpp"
#include <thread>

using namespace std::chrono_literals;

TEST(MessageDispatcher, RebootMessageCallsReboot)
{
    FakeSystemActions system_actions;
    Watchdog watchdog{Configuration(), system_actions};
    MessageDispatcher dispatcher(system_actions, watchdog);

    dispatcher.dispatch("reboot");

    EXPECT_EQ(system_actions.reboot_calls, 1);
}

TEST(MessageDispatcher, InstallAndRemoveAppDoNotReboot)
{
    FakeSystemActions system_actions;
    Watchdog watchdog{Configuration(), system_actions};
    MessageDispatcher dispatcher(system_actions, watchdog);

    dispatcher.dispatch("install_app");
    dispatcher.dispatch("remove_app");

    EXPECT_EQ(system_actions.reboot_calls, 0);
}

TEST(MessageDispatcher, UnknownMessagesDoNotReboot)
{
    FakeSystemActions system_actions;
    Watchdog watchdog{Configuration(), system_actions};
    MessageDispatcher dispatcher(system_actions, watchdog);

    dispatcher.dispatch("");
    dispatcher.dispatch("unknown");
    dispatcher.dispatch("Reboot");  // commands are case-sensitive
    dispatcher.dispatch("reboot "); // no trimming of whitespace

    EXPECT_EQ(system_actions.reboot_calls, 0);
}

TEST(MessageDispatcher, HeartbeatMessageResetsWatchdogTimeout)
{
    libconfig::Config config;
    config.readString("Player = { command = \"sleep 30\"; heartbeat_timeout = 1; };");
    Configuration configuration;
    configuration.readLibConfigValues(&config);

    FakeSystemActions system_actions;
    Watchdog watchdog(configuration, system_actions);
    MessageDispatcher dispatcher(system_actions, watchdog);
    watchdog.check();
    pid_t pid = watchdog.getPlayerPid();

    // Without the heartbeat the watchdog would restart the player after 1 second
    std::this_thread::sleep_for(700ms);
    dispatcher.dispatch("heartbeat");
    std::this_thread::sleep_for(700ms);
    watchdog.check();

    EXPECT_EQ(watchdog.getPlayerPid(), pid);
}
