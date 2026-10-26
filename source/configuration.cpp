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

#include "configuration.hpp"

Configuration::Configuration() {}

void Configuration::readLibConfigValues(libconfig::Config *config)
{
    try
    {

        std::cout << "Start read values from config file" << std::endl;

        if (config->exists("app_name"))
            config->lookupValue("app_name", app_name);

        if (!config->exists("Player"))
            return;

        libconfig::Setting &player = config->lookup("Player");

        if (player.exists("name"))
            player.lookupValue("name", player_name);

        if (player.exists("path"))
            player.lookupValue("path", player_path);

        if (player.exists("user"))
            player.lookupValue("user", player_user);
    }
    catch (const libconfig::ParseException &e)
    {
        std::cerr << "Parse error at " << e.getFile() << ":" << e.getLine() << " - " << e.getError() << std::endl;
    }
}

std::string Configuration::getAppName() const
{
    return app_name;
}

std::string Configuration::getPlayerName() const
{
    return player_name;
}

std::string Configuration::getPlayerPath() const
{
    return player_path;
}

std::string Configuration::getPlayerUser() const
{
    return player_user;
}
