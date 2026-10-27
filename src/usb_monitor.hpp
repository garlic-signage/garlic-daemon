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

#ifndef USB_MONITOR_HPP
#define USB_MONITOR_HPP

#include <map>
#include <set>
#include <stop_token>
#include <string>
#include "configuration.hpp"
#include "system_actions.hpp"
#include "usb_content_handler.hpp"

struct udev;
struct udev_device;

/**
 * Mounts USB sticks when they are plugged in and unmounts them when they are removed.
 *
 * run() listens to udev events of block devices. A USB partition with a known
 * filesystem is mounted read-only to <Usb.mount_dir>/<device name>, e.g.
 * /media/garlic/sda1, and then passed to the UsbContentHandler.
 * Sticks that are already plugged in when run() starts are handled too.
 *
 * deviceAdded() and deviceRemoved() contain the logic without udev, so tests can call them.
 * The class is not thread safe, all methods are called from the USB thread of the Daemon.
 */
class UsbMonitor
{
    public:
        UsbMonitor(const Configuration &configuration, SystemActions &system_actions,
                   UsbContentHandler &content_handler);

        /// Unmounts all sticks that are still mounted.
        ~UsbMonitor();

        /// Waits for udev events until a stop is requested.
        void run(std::stop_token stop_token);

        /// Mounts the device and passes it to the content handler.
        void deviceAdded(const std::string &device_node, const std::string &fs_type);

        /// Unmounts the device if it was mounted by this class.
        void deviceRemoved(const std::string &device_node);

    private:
        static constexpr int POLL_TIMEOUT_MS = 200; // how often run() checks whether it should stop

        /// Filesystems of USB sticks, others (e.g. swap, encrypted) are not mounted
        const std::set<std::string> SUPPORTED_FILESYSTEMS = {"vfat", "exfat", "ntfs3", "ext2", "ext3", "ext4"};

        std::string mount_dir_;
        SystemActions &system_actions_;
        UsbContentHandler &content_handler_;
        std::map<std::string, std::string> mounted_; // device node -> mount point

        void addPluggedDevices(udev *udev_context);
        void handleDevice(udev_device *device);
        void unmountAll();
};

#endif // USB_MONITOR_HPP
