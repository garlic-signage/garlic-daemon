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

#include <string>
#include <vector>
#include "log.hpp"

class CmdParser
{
    public:
        CmdParser(int argc, char **argv);
        bool checkForConfigOption();
        std::string getConfigFilePath();

    private:
        std::string config_file_name_;
        std::vector<std::string> tokens_;
        [[nodiscard]] const std::string &getCmdOptionValue(const std::string &option) const;
        [[nodiscard]] bool cmdOptionExist(const std::string &option) const;
};

#endif // CMD_PARSER_HPP
