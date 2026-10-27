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

#include "usb_monitor.hpp"

#include <filesystem>
#include <memory>
#include <poll.h>
#include <libudev.h>
#include "log.hpp"

namespace
{
/// Deleter for std::unique_ptr, releases a udev object with its unref function
template <auto unref> struct UdevUnref
{
        void operator()(auto *object) const { unref(object); }
};

// udev objects that release themselves when they go out of scope
using UdevContext   = std::unique_ptr<udev, UdevUnref<udev_unref>>;
using UdevMonitor   = std::unique_ptr<udev_monitor, UdevUnref<udev_monitor_unref>>;
using UdevEnumerate = std::unique_ptr<udev_enumerate, UdevUnref<udev_enumerate_unref>>;
using UdevDevice    = std::unique_ptr<udev_device, UdevUnref<udev_device_unref>>;

/// Returns a udev property or "" if it does not exist
std::string property(udev_device *device, const char *name)
{
    const char *value = udev_device_get_property_value(device, name);
    return value != nullptr ? value : "";
}
} // namespace

UsbMonitor::UsbMonitor(const Configuration &configuration, SystemActions &system_actions,
                       UsbContentHandler &content_handler)
    : mount_dir_(configuration.getUsbMountDir()), system_actions_(system_actions), content_handler_(content_handler)
{
}

UsbMonitor::~UsbMonitor()
{
    unmountAll();
}

void UsbMonitor::run(std::stop_token stop_token)
{
    UdevContext udev_context(udev_new());
    if (!udev_context)
    {
        logError("Cannot create udev context, USB sticks will not be mounted.");
        return;
    }

    UdevMonitor monitor(udev_monitor_new_from_netlink(udev_context.get(), "udev"));
    if (!monitor)
    {
        logError("Cannot create udev monitor, USB sticks will not be mounted.");
        return;
    }
    udev_monitor_filter_add_match_subsystem_devtype(monitor.get(), "block", nullptr);
    udev_monitor_enable_receiving(monitor.get());

    removeOldMounts();

    // After the monitor is enabled, so no stick plugged in meanwhile is missed.
    // A stick reported twice is mounted only once, see deviceAdded().
    addPluggedDevices(udev_context.get());

    pollfd monitor_fd = {udev_monitor_get_fd(monitor.get()), POLLIN, 0};
    while (!stop_token.stop_requested())
    {
        if (poll(&monitor_fd, 1, POLL_TIMEOUT_MS) <= 0)
            continue; // timeout, check stop_token again

        UdevDevice device(udev_monitor_receive_device(monitor.get()));
        if (device)
            handleUdevDevice(device.get());
    }
}

void UsbMonitor::deviceAdded(const std::string &device_node, const std::string &fs_type)
{
    if (mounted_.contains(device_node))
        return;

    if (!SUPPORTED_FILESYSTEMS.contains(fs_type))
    {
        logWarning("USB device {} has unsupported filesystem '{}', not mounted.", device_node, fs_type);
        return;
    }

    std::string mount_point = mount_dir_ + "/" + std::filesystem::path(device_node).filename().string();
    std::error_code error;
    std::filesystem::create_directories(mount_point, error);
    if (error)
    {
        logError("Cannot create {}: {}", mount_point, error.message());
        return;
    }

    if (!system_actions_.mount(device_node, mount_point, fs_type))
    {
        std::filesystem::remove(mount_point, error);
        return;
    }

    mounted_[device_node] = mount_point;
    logInfo("Mounted USB device {} to {}", device_node, mount_point);
    content_handler_.handle(mount_point);
}

void UsbMonitor::deviceRemoved(const std::string &device_node)
{
    auto entry = mounted_.find(device_node);
    if (entry == mounted_.end())
        return;

    system_actions_.unmount(entry->second);
    std::error_code error;
    std::filesystem::remove(entry->second, error); // only removes an empty directory
    logInfo("Unmounted USB device {} from {}", device_node, entry->second);
    mounted_.erase(entry);
}

void UsbMonitor::removeOldMounts()
{
    std::error_code error;
    for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(mount_dir_, error))
    {
        if (!entry.is_directory(error) || entry.is_symlink(error))
            continue;

        system_actions_.unmount(entry.path().string());   // no error if it is no mount point
        if (std::filesystem::remove(entry.path(), error)) // only removes an empty directory
            logInfo("Removed old mount point {}", entry.path().string());
    }
}

/**
 * Handles the USB sticks that are already plugged in.
 */
void UsbMonitor::addPluggedDevices(udev *udev_context)
{
    UdevEnumerate enumerate(udev_enumerate_new(udev_context));
    if (!enumerate)
        return;
    udev_enumerate_add_match_subsystem(enumerate.get(), "block");
    udev_enumerate_scan_devices(enumerate.get());

    udev_list_entry *entry = nullptr;
    udev_list_entry_foreach(entry, udev_enumerate_get_list_entry(enumerate.get()))
    {
        UdevDevice device(udev_device_new_from_syspath(udev_context, udev_list_entry_get_name(entry)));
        if (device)
            handleUdevDevice(device.get());
    }
}

/**
 * A device from an event or from the list of plugged devices, read from udev.
 */
void UsbMonitor::handleUdevDevice(udev_device *device)
{
    const char *node   = udev_device_get_devnode(device);
    const char *action = udev_device_get_action(device);
    handleDevice({.action   = action != nullptr ? action : "",
                  .node     = node != nullptr ? node : "",
                  .bus      = property(device, "ID_BUS"),
                  .fs_usage = property(device, "ID_FS_USAGE"),
                  .fs_type  = property(device, "ID_FS_TYPE")});
}

/**
 * Only partitions or whole disks on USB with a filesystem are mounted.
 */
void UsbMonitor::handleDevice(const Device &device)
{
    if (device.node.empty())
        return;

    if (device.action == "remove")
    {
        deviceRemoved(device.node);
        return;
    }

    // Devices from addPluggedDevices() have no action, they count as added
    if (device.action != "add" && !device.action.empty())
        return;

    if (device.bus != "usb" || device.fs_usage != "filesystem")
        return;

    deviceAdded(device.node, device.fs_type);
}

void UsbMonitor::unmountAll()
{
    while (!mounted_.empty())
        deviceRemoved(mounted_.begin()->first);
}
