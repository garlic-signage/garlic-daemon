/*************************************************************************************
    garlic-daemon: Linux daemon for garlic-player
    Copyright (C) 2026 Nikolaos Sagiadinos <ns@smil-control.com>
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

#ifndef LOCAL_SOCKET_SERVER_HPP
#define LOCAL_SOCKET_SERVER_HPP

#include <condition_variable>
#include <mutex>
#include <queue>
#include <stop_token>
#include <string>
#include <thread>
#include <vector>
#include <sys/types.h>
#include "file_descriptor.hpp"

/**
 * Unix domain socket for the communication between player and daemon.
 *
 * Protocol: one message per line, terminated by '\n' ("\r\n" is accepted too).
 * Empty lines are ignored, a line longer than MAX_LINE_LENGTH closes the connection.
 *
 * Only clients running as allowed_uid or as root are accepted. The kernel tells
 * the user of a client (SO_PEERCRED), so the file permissions of the socket do
 * not matter: everybody may connect, but other users are disconnected at once.
 *
 * Received lines can be fetched in two ways:
 *  1. pollingQueue(): returns at once, false when there is no message.
 *  2. waitForMessage(): blocks until a message arrives, false when the server was stopped.
 *     Call it only after start().
 *
 * send() writes a line to all connected clients, e.g. to tell the player something.
 * All methods are thread safe. The destructor stops the thread and removes the socket file,
 * the file descriptors close themselves (FileDescriptor).
 */
class LocalSocketServer
{
    public:
        /// Creates the socket, throws std::runtime_error on error.
        /// An old socket file, e.g. after a crash, is replaced.
        LocalSocketServer(const std::string &socket_path, uid_t allowed_uid);
        ~LocalSocketServer();

        void start();

        /// Requests the stop and wakes up waitForMessage(). Calling it more than once is harmless.
        void stop();

        bool pollingQueue(std::string &message);
        bool waitForMessage(std::string &message);

        /// Sends message + '\n' to all connected clients.
        void send(const std::string &message);

    private:
        static constexpr int POLL_TIMEOUT_MS    = 200; // how often the thread checks whether it should stop
        static constexpr size_t MAX_LINE_LENGTH = 8192;

        struct Client
        {
                FileDescriptor fd;
                std::string buffer; // received data without a complete line yet
        };

        std::string socket_path_;
        uid_t allowed_uid_;
        FileDescriptor listen_fd_;

        std::mutex clients_mtx_; // the server thread changes the clients, send() writes to them
        std::vector<Client> clients_;

        std::mutex mtx_;
        std::queue<std::string> queue_;
        std::condition_variable_any cv_;
        std::jthread server_thread_; // last member, so it is destroyed first

        void loop(std::stop_token stop_token);
        void acceptClient();
        bool readClient(Client &client);
        bool isAllowed(int fd) const;
        void push(const std::string &message);
};

#endif // LOCAL_SOCKET_SERVER_HPP
