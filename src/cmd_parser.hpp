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

#ifndef CMD_PARSER_HPP
#define CMD_PARSER_HPP

#include <map>
#include <string>

/**
 * Parses the command line options, see OPTIONS in cmd_parser.cpp and helpText().
 *
 * An option with an argument takes the next token as value, e.g. --socket /tmp/garlic.sock.
 * Unknown options, arguments without option and a missing value are errors,
 * so a typo is not silently ignored. When an option is given twice, the last value wins.
 */
class CmdParser
{
    public:
        CmdParser(int argc, char **argv);

        /// Empty if the command line is valid, otherwise a message for the user.
        [[nodiscard]] std::string getError() const;

        [[nodiscard]] bool has(const std::string &option) const;

        /// Value of an option with argument, "" if it is not given.
        [[nodiscard]] std::string value(const std::string &option) const;

        /// Usage and all options with description.
        static std::string helpText();

    private:
        std::map<std::string, std::string> values_; // option -> value ("" for options without argument)
        std::string error_;
};

#endif // CMD_PARSER_HPP
