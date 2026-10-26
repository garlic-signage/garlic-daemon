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
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include "ipc_message_receiver.hpp"

// Integration tests with a real POSIX message queue.
// Every test uses its own queue name, so tests do not see each other's messages.
// The receiver thread is stopped and joined by the destructor at the end of each test.
class IPCMessageReceiverTest : public ::testing::Test
{
    protected:
        std::string queue_name;

        void SetUp() override
        {
            queue_name = "/garlic-test-" + std::to_string(getpid()) + "-" +
                         ::testing::UnitTest::GetInstance()->current_test_info()->name();
        }

        // Sends a message like the player would do from another process.
        void send(const std::string &message)
        {
            mqd_t sender = mq_open(queue_name.c_str(), O_WRONLY);
            ASSERT_NE(sender, (mqd_t)-1) << strerror(errno);
            ASSERT_EQ(mq_send(sender, message.c_str(), message.size(), 0), 0) << strerror(errno);
            mq_close(sender);
        }

        // Polls until a message arrives, but at most 2 seconds, so a broken test fails instead of hanging.
        static bool pollWithTimeout(IPCMessageReceiver &receiver, std::string &message)
        {
            auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
            while (std::chrono::steady_clock::now() < deadline)
            {
                if (receiver.pollingQueue(message))
                    return true;
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
            return false;
        }
};

TEST_F(IPCMessageReceiverTest, ThrowsOnInvalidQueueName)
{
    // A slash inside the name is not allowed for POSIX message queues
    EXPECT_THROW(IPCMessageReceiver receiver("/invalid/name"), std::runtime_error);
}

TEST_F(IPCMessageReceiverTest, PollingEmptyQueueReturnsFalse)
{
    IPCMessageReceiver receiver(queue_name);
    std::string message;

    EXPECT_FALSE(receiver.pollingQueue(message));
}

TEST_F(IPCMessageReceiverTest, PollingReceivesMessage)
{
    IPCMessageReceiver receiver(queue_name);
    receiver.startIPCReceiver();

    send("reboot");

    std::string message;
    EXPECT_TRUE(pollWithTimeout(receiver, message));
    EXPECT_EQ(message, "reboot");
}

TEST_F(IPCMessageReceiverTest, WaitForMessageReceivesMessage)
{
    IPCMessageReceiver receiver(queue_name);
    receiver.startIPCReceiver();

    send("reboot");

    std::string message;
    EXPECT_TRUE(receiver.waitForMessage(message));
    EXPECT_EQ(message, "reboot");
}

TEST_F(IPCMessageReceiverTest, KeepsMessageOrder)
{
    IPCMessageReceiver receiver(queue_name);
    receiver.startIPCReceiver();

    send("first");
    send("second");
    send("third");

    std::string message;
    EXPECT_TRUE(receiver.waitForMessage(message));
    EXPECT_EQ(message, "first");
    EXPECT_TRUE(receiver.waitForMessage(message));
    EXPECT_EQ(message, "second");
    EXPECT_TRUE(receiver.waitForMessage(message));
    EXPECT_EQ(message, "third");
}

TEST_F(IPCMessageReceiverTest, MessageSentBeforeStartIsReceived)
{
    IPCMessageReceiver receiver(queue_name);
    send("install_app");

    receiver.startIPCReceiver();

    std::string message;
    EXPECT_TRUE(pollWithTimeout(receiver, message));
    EXPECT_EQ(message, "install_app");
}

TEST_F(IPCMessageReceiverTest, TerminateIsANormalMessage)
{
    IPCMessageReceiver receiver(queue_name);
    receiver.startIPCReceiver();

    // No message has a special meaning, so no other process can stop the receiver
    send("TERMINATE");
    send("reboot");

    std::string message;
    EXPECT_TRUE(receiver.waitForMessage(message));
    EXPECT_EQ(message, "TERMINATE");
    EXPECT_TRUE(receiver.waitForMessage(message));
    EXPECT_EQ(message, "reboot");
}

TEST_F(IPCMessageReceiverTest, StopWakesUpWaitForMessage)
{
    IPCMessageReceiver receiver(queue_name);
    receiver.startIPCReceiver();

    // Stops the receiver from another thread while waitForMessage() is blocking
    std::jthread stopper(
        [&receiver]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            receiver.stopIPCReceiver();
        });

    std::string message;
    EXPECT_FALSE(receiver.waitForMessage(message));
}

TEST_F(IPCMessageReceiverTest, WaitForMessageAfterStopReturnsFalse)
{
    IPCMessageReceiver receiver(queue_name);
    receiver.startIPCReceiver();
    receiver.stopIPCReceiver();

    std::string message;
    EXPECT_FALSE(receiver.waitForMessage(message));
}

TEST_F(IPCMessageReceiverTest, WaitForMessageAfterStopIgnoresQueuedMessages)
{
    IPCMessageReceiver receiver(queue_name);
    receiver.startIPCReceiver();

    send("reboot");
    std::this_thread::sleep_for(std::chrono::milliseconds(300)); // message is now queued
    receiver.stopIPCReceiver();

    std::string message;
    EXPECT_FALSE(receiver.waitForMessage(message));
}

TEST_F(IPCMessageReceiverTest, StopRightAfterStartIsSafe)
{
    auto begin = std::chrono::steady_clock::now();
    {
        IPCMessageReceiver receiver(queue_name);
        receiver.startIPCReceiver();
        receiver.stopIPCReceiver();
        receiver.stopIPCReceiver(); // calling stop twice is harmless
    }

    // The destructor joins the thread, which needs at most one receive timeout
    EXPECT_LT(std::chrono::steady_clock::now() - begin, std::chrono::seconds(1));
}

TEST_F(IPCMessageReceiverTest, QueueIsRemovedOnDestruction)
{
    {
        IPCMessageReceiver receiver(queue_name);
    }

    // The queue no longer exists, so opening it without O_CREAT fails
    mqd_t mq = mq_open(queue_name.c_str(), O_RDONLY);
    EXPECT_EQ(mq, (mqd_t)-1);
    EXPECT_EQ(errno, ENOENT);
}
