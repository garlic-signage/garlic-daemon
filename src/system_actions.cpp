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

#include "system_actions.hpp"

#include <cerrno>
#include <system_error>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include "log.hpp"

void SystemActions::reboot()
{
    sync();
    ::reboot(RB_AUTOBOOT);
}

bool SystemActions::mount(const std::string &device, const std::string &mount_point, const std::string &fs_type)
{
    // Read-only, so pulling the stick without unmounting cannot damage it
    unsigned long flags = MS_RDONLY | MS_NOSUID | MS_NODEV | MS_NOEXEC;
    if (::mount(device.c_str(), mount_point.c_str(), fs_type.c_str(), flags, nullptr) != 0)
    {
        logError("Cannot mount {} to {}: {}", device, mount_point, std::system_category().message(errno));
        return false;
    }
    return true;
}

void SystemActions::unmount(const std::string &mount_point)
{
    if (umount2(mount_point.c_str(), MNT_DETACH) != 0)
        logError("Cannot unmount {}: {}", mount_point, std::system_category().message(errno));
}
