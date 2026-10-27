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

#ifndef LOG_HPP
#define LOG_HPP

#include <chrono>
#include <filesystem>
#include <format>
#include <iostream>
#include <print>
#include <source_location>
#include <string>
#include <type_traits>

/**
 * Log functions for the daemon, they write one line per call to std::clog:
 *
 *     logInfo("Started {} with pid {}", player_name_, pid);
 *     -> <6>[2026-10-27_15:48:52.064] INFO-> Started garlic-player with pid 4774
 *
 * Warnings, errors and debug messages also show where they were logged:
 *     <4>[2026-10-27_15:48:52.064] WARNING watchdog.cpp:57-> ...
 *
 * The prefix <N> is the syslog priority. journald removes it and stores the line
 * with this priority, so e.g. "journalctl -u garlic-daemon -p warning" works.
 *
 * The format string is checked at compile time like with std::format().
 */

/// Format string that also remembers the source location of the log call
template <typename... Args> struct LogFormat
{
        std::format_string<Args...> format;
        std::source_location location;

        template <typename Text>
        consteval LogFormat(const Text &text, std::source_location call = std::source_location::current())
            : format(text), location(call)
        {
        }
};

/// Time stamp in UTC with milliseconds, e.g. [2026-10-27_15:48:52.064]
inline std::string getLogTime(std::chrono::system_clock::time_point time)
{
    return std::format("[{:%F_%T}]", std::chrono::floor<std::chrono::milliseconds>(time));
}

// Syslog priorities, see "man sd-daemon"
constexpr int LOG_PRIORITY_ERROR   = 3;
constexpr int LOG_PRIORITY_WARNING = 4;
constexpr int LOG_PRIORITY_INFO    = 6;
constexpr int LOG_PRIORITY_DEBUG   = 7;

/// Writes the whole line at once, so lines from different threads do not mix
inline void writeLog(int priority, const char *type, const std::string &message)
{
    std::println(std::clog, "<{}>{} {}-> {}", priority, getLogTime(std::chrono::system_clock::now()), type, message);
}

inline void writeLog(int priority, const char *type, const std::source_location &location, const std::string &message)
{
    std::string file = std::filesystem::path(location.file_name()).filename().string();
    std::println(std::clog, "<{}>{} {} {}:{}-> {}", priority, getLogTime(std::chrono::system_clock::now()), type, file,
                 location.line(), message);
}

// std::type_identity_t: the types of the arguments are deduced from args only, not from the format string
template <typename... Args> void logInfo(LogFormat<std::type_identity_t<Args>...> format, Args &&...args)
{
    writeLog(LOG_PRIORITY_INFO, "INFO", std::format(format.format, std::forward<Args>(args)...));
}

template <typename... Args> void logWarning(LogFormat<std::type_identity_t<Args>...> format, Args &&...args)
{
    writeLog(LOG_PRIORITY_WARNING, "WARNING", format.location, std::format(format.format, std::forward<Args>(args)...));
}

template <typename... Args> void logError(LogFormat<std::type_identity_t<Args>...> format, Args &&...args)
{
    writeLog(LOG_PRIORITY_ERROR, "ERROR", format.location, std::format(format.format, std::forward<Args>(args)...));
}

template <typename... Args> void logDebug(LogFormat<std::type_identity_t<Args>...> format, Args &&...args)
{
    writeLog(LOG_PRIORITY_DEBUG, "DEBUG", format.location, std::format(format.format, std::forward<Args>(args)...));
}

#endif // LOG_HPP
