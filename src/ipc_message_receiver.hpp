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

#ifndef IPCMESSAGERECEIVER_HPP
#define IPCMESSAGERECEIVER_HPP

#include <mqueue.h>
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stop_token>
#include <thread>

/**
 * @brief The IPCMessageReceiver class
 *
 * This threadsafe class receives messages from a POSIX message queue
 * in a thread started with startIPCReceiver().
 * A mutex is used for thread safety.
 *
 * When a message is received it will be stored in std::queue<std::string> queue;
 * There are two ways to get these messages:
 *  1. Polling
 *      Calling non-blocking pollingQueue(std::string& message) will fill the parameter
 *      with the oldest message from queue and return true. It will return
 *      false when queue is empty.
 *
 *  2. Waiting for a message
 *      Calling waitForMessage(std::string& message) blocks until a message
 *      is received and returns true. It returns false when the receiver was stopped,
 *      also when messages are still queued.
 *      This is the more resource friendly and preferred way.
 *      Call it only after startIPCReceiver().
 *
 * stopIPCReceiver() only requests the stop. The destructor waits until
 * the thread has finished, so the object can be destroyed safely.
 */
class IPCMessageReceiver
{

    public:
        explicit IPCMessageReceiver(const std::string &name);
        ~IPCMessageReceiver();
        void startIPCReceiver();
        void stopIPCReceiver();
        bool pollingQueue(std::string &message);
        bool waitForMessage(std::string &message);

    private:
        /// How often the receiver thread checks whether it should stop
        static constexpr int RECEIVE_TIMEOUT_MS = 200;

        std::string message_queue_name;
        mqd_t mq;
        std::queue<std::string> queue;
        std::mutex mtx;
        std::condition_variable_any cv;
        std::jthread receiver_thread; // last member, so it is destroyed first

        void loopIPCReceiver(std::stop_token stop_token);
        void push(const std::string &message);
};

#endif // IPCMESSAGERECEIVER_HPP
