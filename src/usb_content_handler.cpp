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

#include "usb_content_handler.hpp"

#include "log.hpp"

UsbContentHandler::UsbContentHandler(const Configuration &configuration, UsbActions &actions)
    : firmware_file(configuration.getUsbFirmwareFile()), actions(actions)
{
}

namespace
{
/// true if root/relative is a regular file and no part of the path is a symlink.
/// A symlink on the stick could point to a file of the system, e.g. /etc/shadow.
bool isFileOnStick(const std::filesystem::path &root, const std::filesystem::path &relative)
{
    std::filesystem::path path = root;
    for (const std::filesystem::path &part : relative)
    {
        path /= part;
        if (std::filesystem::is_symlink(std::filesystem::symlink_status(path)))
            return false;
    }
    return std::filesystem::is_regular_file(std::filesystem::symlink_status(path));
}
} // namespace

void UsbContentHandler::handle(const std::filesystem::path &root)
{
    LOG_INFO("Look for files on USB stick ", root.string());

    if (isFileOnStick(root, CONFIG_XML))
        actions.applyConfig((root / CONFIG_XML).string());

    if (isFileOnStick(root, firmware_file))
        actions.updatePlayer((root / firmware_file).string());

    if (isFileOnStick(root, SMIL_INDEX))
        actions.playSmil((root / SMIL_INDEX).string());
}
