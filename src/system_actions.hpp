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

/**
 * Bundles the calls that change the operating system (e.g. reboot).
 *
 * The methods are virtual, so tests can override them with a fake
 * and the test machine is not rebooted.
 */
class SystemActions
{
    public:
        virtual ~SystemActions() = default;

        /// Writes all buffers to disk and reboots the machine.
        virtual void reboot();
};

#endif // SYSTEM_ACTIONS_HPP
