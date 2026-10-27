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

#ifndef CURRENT_USER_HPP
#define CURRENT_USER_HPP

#include <string>
#include <pwd.h>
#include <unistd.h>

// Name of the user running the tests. Tests use it as Player.user, so the
// watchdog can start the test processes also when the tests run as root
// (e.g. in a container), where it would otherwise switch to "digitalsignage".
inline std::string currentUser()
{
    passwd *pw = getpwuid(geteuid());
    return pw != nullptr ? pw->pw_name : "";
}

#endif // CURRENT_USER_HPP
