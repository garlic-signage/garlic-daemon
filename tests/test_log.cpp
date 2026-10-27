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
#include <algorithm>
#include <iostream>
#include <sstream>
#include "log.hpp"

// The log functions write to std::clog, the tests catch it in a string.
// The prefix <N> must come first in the line, otherwise journald does not see the priority.
class LogTest : public ::testing::Test
{
    protected:
        std::ostringstream output;
        std::streambuf *original = nullptr;

        void SetUp() override { original = std::clog.rdbuf(output.rdbuf()); }
        void TearDown() override { std::clog.rdbuf(original); }
};

TEST_F(LogTest, InfoHasPriority6WithoutLocation)
{
    logInfo("Started {} with pid {}", "player", 42);

    EXPECT_EQ(output.str(), "<6>INFO-> Started player with pid 42\n");
}

TEST_F(LogTest, WarningHasPriority4WithLocation)
{
    int line = __LINE__ + 1;
    logWarning("careful");

    EXPECT_EQ(output.str(), "<4>WARNING test_log.cpp:" + std::to_string(line) + "-> careful\n");
}

TEST_F(LogTest, ErrorHasPriority3WithLocation)
{
    int line = __LINE__ + 1;
    logError("Cannot {}", "mount");

    EXPECT_EQ(output.str(), "<3>ERROR test_log.cpp:" + std::to_string(line) + "-> Cannot mount\n");
}

TEST_F(LogTest, DebugHasPriority7WithLocation)
{
    int line = __LINE__ + 1;
    logDebug("value {}", 1);

    EXPECT_EQ(output.str(), "<7>DEBUG test_log.cpp:" + std::to_string(line) + "-> value 1\n");
}

TEST_F(LogTest, EveryCallIsOneLine)
{
    logInfo("first");
    logError("second");

    std::string text = output.str();
    EXPECT_EQ(std::ranges::count(text, '\n'), 2);
    EXPECT_TRUE(text.starts_with("<6>"));
    EXPECT_NE(text.find("\n<3>"), std::string::npos);
}
