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

#include "message_dispatcher.hpp"

MessageDispatcher::MessageDispatcher(SystemActions &system_actions) : MySystemActions(system_actions) {}

void MessageDispatcher::dispatch(const std::string& message)
{
    if (message == REBOOT)
    {
        MySystemActions.reboot();
    }
    else if (message == INSTALL_APP)
    {
        installApp();
    }
    else if (message == REMOVE_APP)
    {
        removeApp();
    }
    else
    {
        std::cerr << "Signale " + message + " is not supported." << std::endl;
    }
}

void MessageDispatcher::installApp()
{
    std::cerr << "install apps not yet implemented." << std::endl;
}

void MessageDispatcher::removeApp()
{
    std::cerr << "install apps not yet implemented." << std::endl;
}
