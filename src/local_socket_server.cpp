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

#include "local_socket_server.hpp"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <system_error>
#include <poll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include "log.hpp"

LocalSocketServer::LocalSocketServer(const std::string &socket_path, uid_t allowed_uid)
    : socket_path_(socket_path), allowed_uid_(allowed_uid)
{
    sockaddr_un address{};
    address.sun_family = AF_UNIX;
    if (socket_path.size() >= sizeof(address.sun_path))
        throw std::runtime_error("Socket path too long: " + socket_path);
    std::strcpy(address.sun_path, socket_path.c_str());

    listen_fd_ = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (listen_fd_ < 0)
        throw std::system_error(errno, std::system_category(), "Cannot create socket");

    unlink(socket_path.c_str()); // old socket file, e.g. after a crash
    if (bind(listen_fd_, reinterpret_cast<sockaddr *>(&address), sizeof(address)) != 0 || listen(listen_fd_, 5) != 0)
    {
        int error = errno; // close() could change errno
        close(listen_fd_);
        throw std::system_error(error, std::system_category(), "Cannot create socket " + socket_path);
    }

    // Everybody may connect, isAllowed() decides who may stay
    chmod(socket_path.c_str(), 0666);
}

LocalSocketServer::~LocalSocketServer()
{
    server_thread_.request_stop();
    if (server_thread_.joinable())
        server_thread_.join();

    for (const Client &client : clients_)
        close(client.fd);
    close(listen_fd_);
    unlink(socket_path_.c_str());
}

void LocalSocketServer::start()
{
    server_thread_ = std::jthread([this](std::stop_token stop_token) { loop(stop_token); });
}

void LocalSocketServer::stop()
{
    server_thread_.request_stop();
}

bool LocalSocketServer::pollingQueue(std::string &message)
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (queue_.empty())
        return false;

    message = queue_.front();
    queue_.pop();
    return true;
}

bool LocalSocketServer::waitForMessage(std::string &message)
{
    std::unique_lock<std::mutex> lock(mtx_);
    std::stop_token stop_token = server_thread_.get_stop_token();
    cv_.wait(lock, stop_token, [this] { return !queue_.empty(); });

    // After a stop no more messages are handed out, even if some are still queued
    if (stop_token.stop_requested())
        return false;

    message = queue_.front();
    queue_.pop();
    return true;
}

void LocalSocketServer::send(const std::string &message)
{
    std::string line = message + "\n";
    std::lock_guard<std::mutex> lock(clients_mtx_);
    for (const Client &client : clients_)
    {
        // MSG_NOSIGNAL: no SIGPIPE when the client is gone, the server thread removes it then
        if (::send(client.fd, line.data(), line.size(), MSG_NOSIGNAL | MSG_DONTWAIT) < 0)
            logWarning("Cannot send to client: {}", std::system_category().message(errno));
    }
}

/**
 * Waits for new connections and data. poll() returns at least every POLL_TIMEOUT_MS,
 * so the loop can check whether it should stop.
 */
void LocalSocketServer::loop(std::stop_token stop_token)
{
    while (!stop_token.stop_requested())
    {
        std::vector<pollfd> fds = {{listen_fd_, POLLIN, 0}};
        {
            std::lock_guard<std::mutex> lock(clients_mtx_);
            for (const Client &client : clients_)
                fds.push_back({client.fd, POLLIN, 0});
        }

        if (poll(fds.data(), fds.size(), POLL_TIMEOUT_MS) <= 0)
            continue;

        if (fds[0].revents & POLLIN)
            acceptClient();

        std::lock_guard<std::mutex> lock(clients_mtx_);
        for (size_t i = 1; i < fds.size(); i++)
        {
            if (fds[i].revents == 0)
                continue;

            auto client = std::ranges::find(clients_, fds[i].fd, &Client::fd);
            if (client != clients_.end() && !readClient(*client))
            {
                close(client->fd);
                clients_.erase(client);
                logInfo("Client disconnected.");
            }
        }
    }
    logInfo("Finish socket server thread.");
}

void LocalSocketServer::acceptClient()
{
    int fd = accept4(listen_fd_, nullptr, nullptr, SOCK_CLOEXEC);
    if (fd < 0)
        return;

    if (!isAllowed(fd))
    {
        close(fd);
        return;
    }

    std::lock_guard<std::mutex> lock(clients_mtx_);
    clients_.push_back({fd, ""});
    logInfo("Client connected.");
}

/**
 * Reads the available data and pushes every complete line into the queue.
 * Returns false when the connection is closed or a line is too long.
 */
bool LocalSocketServer::readClient(Client &client)
{
    char buffer[4096];
    ssize_t bytes_read = recv(client.fd, buffer, sizeof(buffer), 0);
    if (bytes_read <= 0)
        return false;

    client.buffer.append(buffer, bytes_read);

    size_t end;
    while ((end = client.buffer.find('\n')) != std::string::npos)
    {
        std::string line = client.buffer.substr(0, end);
        client.buffer.erase(0, end + 1);

        if (line.ends_with('\r'))
            line.pop_back();
        if (!line.empty())
            push(line);
    }

    if (client.buffer.size() > MAX_LINE_LENGTH)
    {
        logWarning("Line longer than {} bytes, close connection.", MAX_LINE_LENGTH);
        return false;
    }
    return true;
}

/**
 * The kernel tells the user of the connected process.
 */
bool LocalSocketServer::isAllowed(int fd) const
{
    ucred credentials{};
    socklen_t length = sizeof(credentials);
    if (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &credentials, &length) != 0)
        return false;

    if (credentials.uid == 0 || credentials.uid == allowed_uid_)
        return true;

    logWarning("Rejected connection from uid {} (pid {})", credentials.uid, credentials.pid);
    return false;
}

void LocalSocketServer::push(const std::string &message)
{
    std::lock_guard<std::mutex> lock(mtx_);
    queue_.push(message);
    cv_.notify_one();
}
