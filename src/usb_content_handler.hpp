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

#ifndef USB_CONTENT_HANDLER_HPP
#define USB_CONTENT_HANDLER_HPP

#include <filesystem>
#include <string>
#include "configuration.hpp"
#include "usb_actions.hpp"

/**
 * Looks into the root directory of a mounted USB stick and starts the
 * matching action for every file it finds, in this order:
 *
 *  1. config.xml        -> UsbActions::applyConfig()
 *  2. firmware file     -> UsbActions::updatePlayer()  (name from Usb.firmware_file)
 *  3. SMIL/index.smil   -> UsbActions::playSmil()
 *
 * The configuration comes first, so one stick can set up a device completely:
 * configure it, update the player and then play the content.
 */
class UsbContentHandler
{
    public:
        UsbContentHandler(const Configuration &configuration, UsbActions &actions);

        void handle(const std::filesystem::path &root);

    private:
        static constexpr auto CONFIG_XML = "config.xml";
        static constexpr auto SMIL_INDEX = "SMIL/index.smil";

        std::string firmware_file;
        UsbActions &actions;
};

#endif // USB_CONTENT_HANDLER_HPP
