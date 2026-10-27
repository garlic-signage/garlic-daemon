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
#include "log.hpp"

MessageDispatcher::MessageDispatcher(SystemActions &system_actions, Watchdog &watchdog)
    : system_actions_(system_actions), watchdog_(watchdog)
{
}

void MessageDispatcher::dispatch(const std::string &message)
{
    if (message == HEARTBEAT)
    {
        watchdog_.heartbeat();
    }
    else if (message == REBOOT)
    {
        system_actions_.reboot();
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
        logWarning("Signal {} is not supported.", message);
    }
}

void MessageDispatcher::installApp()
{
    logInfo("Not implemented yet: install app");
}

void MessageDispatcher::removeApp()
{
    logInfo("Not implemented yet: remove app");
}
