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

#ifndef SYSTEM_ACTIONS_HPP
#define SYSTEM_ACTIONS_HPP

#include <string>

/**
 * Bundles the calls that change the operating system (e.g. reboot, mount).
 *
 * The methods are virtual, so tests can override them with a fake
 * and the test machine is not rebooted.
 */
class SystemActions
{
    public:
        virtual ~SystemActions() = default;

        /// Reboots with "systemctl reboot", so all services (also the daemon and the player) stop cleanly.
        /// Only if that fails: writes all buffers to disk and reboots at once.
        virtual void reboot();

        /// Mounts device read-only to mount_point (nosuid, nodev, noexec). Returns false on error.
        virtual bool mount(const std::string &device, const std::string &mount_point, const std::string &fs_type);

        /// Unmounts mount_point, also when the device is already removed (lazy unmount).
        /// A directory that is no mount point is no error.
        virtual void unmount(const std::string &mount_point);
};

#endif // SYSTEM_ACTIONS_HPP
