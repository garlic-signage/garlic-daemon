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

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "usb_monitor.hpp"
#include "fake_system_actions.hpp"
#include "fake_usb_actions.hpp"

namespace fs = std::filesystem;

// Tests handleDevice(), deviceAdded() and deviceRemoved() without udev and without real mounts.
// The mount directory is a temporary directory.
class UsbMonitorTest : public ::testing::Test
{
    protected:
        fs::path mount_dir;
        Configuration configuration;
        FakeSystemActions system_actions;
        FakeUsbActions usb_actions;

        void SetUp() override
        {
            char dir_template[] = "/tmp/garlic-test-mount-XXXXXX";
            mount_dir           = mkdtemp(dir_template);

            libconfig::Config config;
            config.readString("Usb = { mount_dir = \"" + mount_dir.string() + "\"; };");
            configuration.readLibConfigValues(&config);
        }

        void TearDown() override { fs::remove_all(mount_dir); }
};

TEST_F(UsbMonitorTest, AddedDeviceIsMountedAndHandled)
{
    // The fake does not mount, so the "content of the stick" is created in the mount point before
    fs::create_directories(mount_dir / "sdb1");
    std::ofstream(mount_dir / "sdb1" / "config.xml") << "content";

    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);
    monitor.deviceAdded("/dev/sdb1", "vfat");

    ASSERT_EQ(system_actions.mount_calls.size(), 1);
    EXPECT_EQ(system_actions.mount_calls[0].device, "/dev/sdb1");
    EXPECT_EQ(system_actions.mount_calls[0].mount_point, (mount_dir / "sdb1").string());
    EXPECT_EQ(system_actions.mount_calls[0].fs_type, "vfat");
    EXPECT_EQ(usb_actions.calls, std::vector<std::string>{"applyConfig " + (mount_dir / "sdb1/config.xml").string()});
}

TEST_F(UsbMonitorTest, MountPointIsCreated)
{
    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);

    monitor.deviceAdded("/dev/sdb1", "exfat");

    EXPECT_TRUE(fs::is_directory(mount_dir / "sdb1"));
}

TEST_F(UsbMonitorTest, SameDeviceIsMountedOnlyOnce)
{
    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);

    monitor.deviceAdded("/dev/sdb1", "vfat");
    monitor.deviceAdded("/dev/sdb1", "vfat"); // e.g. reported by event and by the list of plugged devices

    EXPECT_EQ(system_actions.mount_calls.size(), 1);
}

TEST_F(UsbMonitorTest, UnsupportedFilesystemIsNotMounted)
{
    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);

    monitor.deviceAdded("/dev/sdb1", "crypto_LUKS");
    monitor.deviceAdded("/dev/sdb2", "swap");
    monitor.deviceAdded("/dev/sdb3", "");

    EXPECT_TRUE(system_actions.mount_calls.empty());
}

TEST_F(UsbMonitorTest, FailedMountIsNotHandled)
{
    system_actions.mount_result = false;
    fs::create_directories(mount_dir / "sdb1");
    std::ofstream(mount_dir / "sdb1" / "config.xml") << "content";

    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);
    monitor.deviceAdded("/dev/sdb1", "vfat");

    EXPECT_TRUE(usb_actions.calls.empty());
}

TEST_F(UsbMonitorTest, FailedMountRemovesMountPoint)
{
    system_actions.mount_result = false;
    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);

    monitor.deviceAdded("/dev/sdb1", "vfat");

    EXPECT_FALSE(fs::exists(mount_dir / "sdb1"));
}

TEST_F(UsbMonitorTest, RemovedDeviceIsUnmounted)
{
    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);
    monitor.deviceAdded("/dev/sdb1", "vfat");

    monitor.deviceRemoved("/dev/sdb1");

    EXPECT_EQ(system_actions.unmount_calls, std::vector<std::string>{(mount_dir / "sdb1").string()});
    EXPECT_FALSE(fs::exists(mount_dir / "sdb1"));
}

TEST_F(UsbMonitorTest, UnknownDeviceIsNotUnmounted)
{
    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);

    monitor.deviceRemoved("/dev/sda1"); // e.g. the system disk, not mounted by the monitor

    EXPECT_TRUE(system_actions.unmount_calls.empty());
}

TEST_F(UsbMonitorTest, DestructorUnmountsAll)
{
    UsbContentHandler handler(configuration, usb_actions);
    {
        UsbMonitor monitor(configuration, system_actions, handler);
        monitor.deviceAdded("/dev/sdb1", "vfat");
        monitor.deviceAdded("/dev/sdc1", "ext4");
    }

    EXPECT_EQ(system_actions.unmount_calls.size(), 2);
}

// handleDevice() gets the values udev reports, this is a USB stick with one FAT partition
const UsbMonitor::Device USB_STICK = {
    .action = "add", .node = "/dev/sdb1", .bus = "usb", .fs_usage = "filesystem", .fs_type = "vfat"};

TEST_F(UsbMonitorTest, AddedUsbPartitionIsMounted)
{
    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);

    monitor.handleDevice(USB_STICK);

    ASSERT_EQ(system_actions.mount_calls.size(), 1);
    EXPECT_EQ(system_actions.mount_calls[0].device, "/dev/sdb1");
    EXPECT_EQ(system_actions.mount_calls[0].fs_type, "vfat");
}

TEST_F(UsbMonitorTest, AlreadyPluggedInDeviceWithoutActionIsMounted)
{
    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);
    UsbMonitor::Device device = USB_STICK;
    device.action             = ""; // devices from addPluggedDevices()

    monitor.handleDevice(device);

    EXPECT_EQ(system_actions.mount_calls.size(), 1);
}

TEST_F(UsbMonitorTest, DeviceNotOnUsbIsNotMounted)
{
    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);
    UsbMonitor::Device device = USB_STICK;
    device.bus                = "ata"; // e.g. the system disk

    monitor.handleDevice(device);

    EXPECT_TRUE(system_actions.mount_calls.empty());
}

TEST_F(UsbMonitorTest, DeviceWithoutFilesystemIsNotMounted)
{
    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);

    // The whole disk with a partition table, an encrypted partition, a device without anything on it
    for (const char *fs_usage : {"", "crypto", "other"})
    {
        UsbMonitor::Device device = USB_STICK;
        device.fs_usage           = fs_usage;
        monitor.handleDevice(device);
    }

    EXPECT_TRUE(system_actions.mount_calls.empty());
}

TEST_F(UsbMonitorTest, DeviceWithoutNodeIsIgnored)
{
    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);
    UsbMonitor::Device device = USB_STICK;
    device.node               = "";

    monitor.handleDevice(device);

    EXPECT_TRUE(system_actions.mount_calls.empty());
}

TEST_F(UsbMonitorTest, OtherActionsAreIgnored)
{
    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);

    for (const char *action : {"change", "move", "bind"})
    {
        UsbMonitor::Device device = USB_STICK;
        device.action             = action;
        monitor.handleDevice(device);
    }

    EXPECT_TRUE(system_actions.mount_calls.empty());
    EXPECT_TRUE(system_actions.unmount_calls.empty());
}

TEST_F(UsbMonitorTest, RemoveActionUnmounts)
{
    UsbContentHandler handler(configuration, usb_actions);
    UsbMonitor monitor(configuration, system_actions, handler);
    monitor.handleDevice(USB_STICK);

    // A removed device may no longer report its properties, only action and node count
    monitor.handleDevice({.action = "remove", .node = "/dev/sdb1", .bus = "", .fs_usage = "", .fs_type = ""});

    EXPECT_EQ(system_actions.unmount_calls, std::vector<std::string>{(mount_dir / "sdb1").string()});
}
