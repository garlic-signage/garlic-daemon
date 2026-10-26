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

#ifndef FAKE_USB_ACTIONS_HPP
#define FAKE_USB_ACTIONS_HPP

#include <string>
#include <vector>
#include "usb_actions.hpp"

// Records the calls in their order, e.g. "applyConfig /media/garlic/sda1/config.xml"
class FakeUsbActions : public UsbActions
{
    public:
        std::vector<std::string> calls;

        void applyConfig(const std::string &config_file) override { calls.push_back("applyConfig " + config_file); }

        void updatePlayer(const std::string &firmware_file) override
        {
            calls.push_back("updatePlayer " + firmware_file);
        }

        void playSmil(const std::string &smil_index) override { calls.push_back("playSmil " + smil_index); }
};

#endif // FAKE_USB_ACTIONS_HPP
