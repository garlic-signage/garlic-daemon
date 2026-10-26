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

#include "cmd_parser.hpp"

CmdParser::CmdParser(const int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        this->m_tokens.emplace_back(argv[i]);
    }
}

bool CmdParser::checkForConfigOption()
{
    if (!cmdOptionExist("--config"))
        return false;

    configFileName = getCmdOptionValue("--config");
    LOG_INFO("Config file name = ", configFileName);

    return true;
}

std::string CmdParser::getConfigFilePath()
{
    return configFileName;
}

const std::string &CmdParser::getCmdOptionValue(const std::string &option) const
{
    auto it = std::ranges::find(this->m_tokens, option);
    if (it != this->m_tokens.end() && ++it != this->m_tokens.end())
    {
        return *it;
    }
    static const std::string emptyString;
    return emptyString;
}

bool CmdParser::cmdOptionExist(const std::string &option) const
{
    const auto it = std::ranges::find(this->m_tokens, option);
    return (it != this->m_tokens.end());
}
