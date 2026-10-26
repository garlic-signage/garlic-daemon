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

#ifndef USB_ACTIONS_HPP
#define USB_ACTIONS_HPP

#include <string>

/**
 * Stub: the reactions to the files found on a USB stick.
 *
 * The methods only log for now. They will be replaced by the real
 * action classes later. They are virtual, so tests can record the calls.
 */
class UsbActions
{
    public:
        virtual ~UsbActions() = default;

        /// config.xml: the daemon applies the WiFi settings itself,
        /// the rest (player name, content url ...) is passed to the player.
        virtual void applyConfig(const std::string &config_file);

        /// Firmware file: updates the player.
        virtual void updatePlayer(const std::string &firmware_file);

        /// SMIL/index.smil: passes the SMIL index to the player.
        virtual void playSmil(const std::string &smil_index);
};

#endif // USB_ACTIONS_HPP
