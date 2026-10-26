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
#include <iomanip>
#include <iostream>
#include <sstream>
#include <type_traits>

#define LOG_INFO(...) Log(std::clog, "INFO").write(__VA_ARGS__)
#define LOG_WARNING(...) Log(std::clog, "WARNING", __FUNCTION__, __LINE__).write(__VA_ARGS__)
#define LOG_ERROR(...) Log(std::clog, "ERROR", __FUNCTION__, __LINE__).write(__VA_ARGS__)
#define LOG_DEBUG(...) Log(std::clog, "DEBUG", __FUNCTION__, __LINE__).write(__VA_ARGS__)

inline std::string getLogTime(std::chrono::time_point<std::chrono::system_clock> time)
{
    auto epoch_seconds = std::chrono::system_clock::to_time_t(time);
    std::stringstream stream;
    stream << std::put_time(gmtime(&epoch_seconds), "[%F_%T");
    auto truncated = std::chrono::system_clock::from_time_t(epoch_seconds);
    auto delta_us  = std::chrono::duration_cast<std::chrono::milliseconds>(time - truncated).count();
    stream << "." << std::fixed << std::setw(3) << std::setfill('0') << delta_us << "]";
    return stream.str();
}

template <typename TF> void writeLog(std::ostream &out, TF const &f)
{
    out << f;
}

struct Log
{
        std::ostream &out;

        Log(std::ostream &out, const char *type, const char *functionName, int line) : out(out)
        {
            out << getLogTime(std::chrono::system_clock::now()) << " " << type << " " << line << ":" << functionName
                << "()-> ";
        }

        Log(std::ostream &out, const char *type) : out(out)
        {
            out << getLogTime(std::chrono::system_clock::now()) << " " << type << "-> ";
        }

        ~Log() { out << std::endl; }

        template <typename TF, typename... TR> void write(TF const &f, TR const &...rest)
        {
            writeLog(out, f);
            write(rest...);
        }

        template <typename TF> void write(TF const &f) { writeLog(out, f); }

        void write() {} // Handle the empty params case
};

#endif // LOG_HPP_
