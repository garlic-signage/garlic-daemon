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

#ifndef MESSAGEDISPATCHER_H
#define MESSAGEDISPATCHER_H

#include <string>
#include <iostream>
#include "system_actions.hpp"

/**
 * Maps a received message (e.g. "reboot") to the matching action.
 */
class MessageDispatcher
{

    public:
        explicit MessageDispatcher(SystemActions &system_actions);
        void dispatch(const std::string& message);

    private:
        const std::string REBOOT      = "reboot";
        const std::string INSTALL_APP = "install_app";
        const std::string REMOVE_APP  = "remove_app";

        SystemActions &MySystemActions;

        static void installApp();
        void removeApp();
};

#endif // MESSAGEDISPATCHER_H
