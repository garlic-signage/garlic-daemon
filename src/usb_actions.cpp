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

#include "usb_actions.hpp"

#include "log.hpp"

void UsbActions::applyConfig(const std::string &config_file)
{
    logInfo("Not implemented yet: apply config {}", config_file);
}

void UsbActions::updatePlayer(const std::string &firmware_file)
{
    logInfo("Not implemented yet: update player with {}", firmware_file);
}

void UsbActions::playSmil(const std::string &smil_index)
{
    logInfo("Not implemented yet: pass {} to the player", smil_index);
}
