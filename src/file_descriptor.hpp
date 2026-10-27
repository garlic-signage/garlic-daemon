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

#ifndef FILE_DESCRIPTOR_HPP
#define FILE_DESCRIPTOR_HPP

#include <utility>
#include <unistd.h>

/**
 * Owns a file descriptor, e.g. of a socket, and closes it in the destructor.
 * It can be moved but not copied, so only one object closes the descriptor.
 * A negative value means no descriptor, e.g. after socket() failed.
 */
class FileDescriptor
{
    public:
        explicit FileDescriptor(int fd = -1) : fd_(fd) {}
        ~FileDescriptor() { reset(); }

        FileDescriptor(FileDescriptor &&other) noexcept : fd_(std::exchange(other.fd_, -1)) {}
        FileDescriptor &operator=(FileDescriptor &&other) noexcept
        {
            if (this != &other)
            {
                reset();
                fd_ = std::exchange(other.fd_, -1);
            }
            return *this;
        }

        int get() const { return fd_; }
        bool isValid() const { return fd_ >= 0; }

    private:
        int fd_;

        void reset()
        {
            if (fd_ >= 0)
                close(fd_);
            fd_ = -1;
        }
};

#endif // FILE_DESCRIPTOR_HPP
