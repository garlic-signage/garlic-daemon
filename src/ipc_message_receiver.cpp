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

#include "ipc_message_receiver.hpp"

#include <cstring>
#include <ctime>

/**
 * @brief IPCMessageReceiver::IPCMessageReceiver
 *
 * Opens a message queue with given name and throws an exception
 * when it is not possible
 *
 * @param name
 */
IPCMessageReceiver::IPCMessageReceiver(const std::string &name)
{
    message_queue_name = name;
    mq                 = mq_open(message_queue_name.c_str(), O_RDWR | O_CREAT, 0644, nullptr);
    if (mq == (mqd_t)-1)
    {
        throw std::runtime_error("Unable to open message queue");
    }
}

/**
 * @brief IPCMessageReceiver::~IPCMessageReceiver
 *
 * Stops and joins the receiver thread first, because it still uses the queue.
 * Then the message queue is deleted, otherwise the last message could be left.
 */
IPCMessageReceiver::~IPCMessageReceiver()
{
    receiver_thread.request_stop();
    if (receiver_thread.joinable())
        receiver_thread.join();

    mq_close(mq);
    mq_unlink(message_queue_name.c_str());
}

/**
 * @brief IPCMessageReceiver::startIPCReceiver
 *
 * Starts the loopIPCReceiver thread.
 */
void IPCMessageReceiver::startIPCReceiver()
{
    receiver_thread = std::jthread([this](std::stop_token stop_token) { loopIPCReceiver(stop_token); });
}

/**
 * @brief IPCMessageReceiver::stopIPCReceiver
 *
 * Requests the receiver thread to stop and wakes up waitForMessage().
 * Calling it more than once is harmless.
 */
void IPCMessageReceiver::stopIPCReceiver()
{
    receiver_thread.request_stop();
}

/**
 * @brief IPCMessageReceiver::pollingQueue
 *
 * Returns immediately after checking if there is a message in queue.
 *
 * @param message
 * @return bool
 */
bool IPCMessageReceiver::pollingQueue(std::string &message)
{
    std::unique_lock<std::mutex> lock(mtx);
    if (queue.empty())
        return false;

    message = queue.front();
    queue.pop();
    return true;
}

/**
 * @brief IPCMessageReceiver::waitForMessage
 *
 * Waits until a message enters the queue or the receiver is stopped.
 *
 * @param message
 * @return true when a message was received, false when the receiver was stopped.
 *         After a stop it always returns false.
 */
bool IPCMessageReceiver::waitForMessage(std::string &message)
{
    std::unique_lock<std::mutex> lock(mtx);
    std::stop_token stop_token = receiver_thread.get_stop_token();
    cv.wait(lock, stop_token, [this] { return !queue.empty(); });

    // After a stop no more messages are handed out, even if some are still queued
    if (stop_token.stop_requested())
        return false;

    message = queue.front();
    queue.pop();
    return true;
}

/**
 * @brief IPCMessageReceiver::loopIPCReceiver
 *
 * Main loop of the receiver thread. Received messages will be pushed into
 * a std::queue<std::string> queue. mq_timedreceive() returns at least every
 * RECEIVE_TIMEOUT_MS, so the loop can check whether it should stop.
 */
void IPCMessageReceiver::loopIPCReceiver(std::stop_token stop_token)
{
    char buffer[8192];

    while (!stop_token.stop_requested())
    {
        // mq_timedreceive() needs an absolute time
        timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_nsec += RECEIVE_TIMEOUT_MS * 1000000L;
        if (timeout.tv_nsec >= 1000000000L)
        {
            timeout.tv_sec += 1;
            timeout.tv_nsec -= 1000000000L;
        }

        ssize_t bytes_read = mq_timedreceive(mq, buffer, sizeof(buffer), nullptr, &timeout);
        if (bytes_read >= 0)
            push(std::string(buffer, bytes_read));
        else if (errno != ETIMEDOUT && errno != EINTR)
            std::cerr << "Failed to receive message: " << strerror(errno) << std::endl;
    }
    std::cout << "Finish message receiver thread." << std::endl;
}

/**
 * @brief IPCMessageReceiver::push
 *
 * A threadsafe push into std::queue<std::string> queue;
 *
 * @param message
 */
void IPCMessageReceiver::push(const std::string &message)
{
    std::unique_lock<std::mutex> lock(mtx);
    queue.push(message);
    cv.notify_one();
}
