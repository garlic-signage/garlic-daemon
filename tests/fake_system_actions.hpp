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

#ifndef FAKE_SYSTEM_ACTIONS_HPP
#define FAKE_SYSTEM_ACTIONS_HPP

#include <atomic>
#include "system_actions.hpp"

// Replaces the real system calls, so the test machine is not rebooted.
// It only counts how often each action was called.
// The counter is atomic, because the Daemon calls it from its listener thread.
class FakeSystemActions : public SystemActions
{
    public:
        std::atomic<int> reboot_calls = 0;

        void reboot() override { reboot_calls++; }
};

#endif // FAKE_SYSTEM_ACTIONS_HPP
