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

#include <gtest/gtest.h>
#include <chrono>
#include <csignal>
#include <cstring>
#include <filesystem>
#include <thread>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "player_protocol.hpp"
#include "current_user.hpp"
#include "fake_system_actions.hpp"

namespace fs = std::filesystem;
using namespace std::chrono_literals;
using nlohmann::json;

// Protocol 1 over a real Unix socket. The test plays the listener thread of the Daemon: after every line of a client
// it hands the next message of the server to the protocol (pump()).
class PlayerProtocolTest : public ::testing::Test
{
    protected:
        fs::path tmp_dir;
        std::string socket_path;
        std::vector<int> client_fds;
        FakeSystemActions system_actions;
        Configuration configuration;
        std::unique_ptr<Watchdog> watchdog;
        std::unique_ptr<MessageDispatcher> dispatcher;
        std::unique_ptr<LocalSocketServer> server;
        std::unique_ptr<PlayerProtocol> protocol;

        void SetUp() override
        {
            char dir_template[] = "/tmp/garlic-test-protocol-XXXXXX";
            tmp_dir             = mkdtemp(dir_template);
            socket_path         = (tmp_dir / "test.sock").string();
            create(30);
        }

        void TearDown() override
        {
            for (int fd : client_fds)
                close(fd);
            protocol.reset();
            server.reset();
            dispatcher.reset();
            watchdog.reset();
            fs::remove_all(tmp_dir);
        }

        void create(int heartbeat_timeout, const std::string &player_command = "sleep 30")
        {
            libconfig::Config config;
            config.readString("state_dir = \"" + tmp_dir.string() + "\"; Player = { user = \"" + currentUser() +
                              "\"; command = \"" + player_command + "\"; heartbeat_timeout = " +
                              std::to_string(heartbeat_timeout) + "; };");
            configuration.readLibConfigValues(&config);
            watchdog   = std::make_unique<Watchdog>(configuration, system_actions);
            dispatcher = std::make_unique<MessageDispatcher>(system_actions, *watchdog);
            server     = std::make_unique<LocalSocketServer>(socket_path, getuid());
            protocol   = std::make_unique<PlayerProtocol>(*server, *dispatcher, system_actions, *watchdog,
                                                        heartbeat_timeout, "9.9");
            server->start();
        }

        int connectClient()
        {
            int fd = socket(AF_UNIX, SOCK_STREAM, 0);
            sockaddr_un address{};
            address.sun_family = AF_UNIX;
            std::strcpy(address.sun_path, socket_path.c_str());
            EXPECT_EQ(connect(fd, reinterpret_cast<sockaddr *>(&address), sizeof(address)), 0) << strerror(errno);
            client_fds.push_back(fd);
            return fd;
        }

        /// Hands the next message of the server (a line or a closed connection) to the protocol.
        void pump()
        {
            LocalSocketServer::Message message;
            ASSERT_TRUE(server->waitForMessage(message));
            protocol->handle(message);
        }

        void send(int fd, const std::string &line)
        {
            std::string data = line + "\n";
            ASSERT_EQ(::write(fd, data.data(), data.size()), static_cast<ssize_t>(data.size()));
            pump();
        }

        /// One line from the daemon, empty after timeout or at the end of the connection.
        static std::string readLine(int fd, std::chrono::milliseconds timeout = 2s)
        {
            timeval tv{static_cast<time_t>(timeout.count() / 1000), static_cast<suseconds_t>((timeout.count() % 1000) * 1000)};
            setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
            std::string line;
            char c = 0;
            while (recv(fd, &c, 1, 0) == 1)
            {
                if (c == '\n')
                    return line;
                line += c;
            }
            return "";
        }

        json request(int fd, const json &message)
        {
            send(fd, message.dump());
            std::string line = readLine(fd);
            return line.empty() ? json() : json::parse(line);
        }

        json hello(int fd, const std::string &client = "player", const json &capabilities = json::array())
        {
            return request(fd, {{"jsonrpc", "2.0"},
                                {"id", 1},
                                {"method", "hello"},
                                {"params", {{"protocol", 1}, {"client", client}, {"capabilities", capabilities},
                                            {"player", {{"name", "garlic-player"}, {"version", "2.0.0"}}}}}});
        }

        static int errorCode(const json &answer)
        {
            return answer.contains("error") ? answer["error"].value("code", 0) : 0;
        }
};

TEST_F(PlayerProtocolTest, HelloAnswersWhatTheDaemonOffers)
{
    int fd      = connectClient();
    json answer = hello(fd);

    ASSERT_TRUE(answer.contains("result")) << answer.dump();
    json result = answer["result"];
    EXPECT_EQ(answer["id"], 1);
    EXPECT_EQ(result["protocol"], 1);
    EXPECT_EQ(result["supervisor"]["name"], "garlic-daemon");
    EXPECT_EQ(result["supervisor"]["version"], "9.9");
    EXPECT_EQ(result["capabilities"], json::array({"reboot"}));
    EXPECT_EQ(result["heartbeat_interval"], 10); // a third of heartbeat_timeout 30
    EXPECT_EQ(result["locked"], false);
}

TEST_F(PlayerProtocolTest, WithoutRootNoRebootIsOffered)
{
    system_actions.can_reboot = false;
    int fd                    = connectClient();
    EXPECT_EQ(hello(fd)["result"]["capabilities"], json::array());

    json answer = request(fd, {{"jsonrpc", "2.0"}, {"id", 2}, {"method", "reboot"}, {"params", {{"task_id", "t"}}}});
    EXPECT_EQ(errorCode(answer), -32000);
    EXPECT_EQ(answer["error"]["data"]["reason"], "no permission");
    EXPECT_EQ(system_actions.reboot_calls, 0);
}

TEST(PlayerProtocol, HeartbeatIntervalIsAThirdOfTheTimeout)
{
    EXPECT_EQ(PlayerProtocol::heartbeatInterval(0), 0); // no hang detection
    EXPECT_EQ(PlayerProtocol::heartbeatInterval(60), 20);
    EXPECT_EQ(PlayerProtocol::heartbeatInterval(2), 1); // at least 1 s
}

TEST_F(PlayerProtocolTest, AnOlderProtocolIsRefusedAndTheConnectionClosed)
{
    int fd      = connectClient();
    json answer = request(fd, {{"jsonrpc", "2.0"}, {"id", 1}, {"method", "hello"}, {"params", {{"protocol", 0}}}});
    EXPECT_EQ(errorCode(answer), -32002);
    pump(); // the closed connection
    EXPECT_EQ(readLine(fd), "");
}

TEST_F(PlayerProtocolTest, RequestsBeforeHelloAreNotReady)
{
    int fd      = connectClient();
    json answer = request(fd, {{"jsonrpc", "2.0"}, {"id", 5}, {"method", "reboot"}});
    EXPECT_EQ(errorCode(answer), -32001);
    EXPECT_EQ(answer["id"], 5);
    EXPECT_EQ(system_actions.reboot_calls, 0);
}

TEST_F(PlayerProtocolTest, RebootIsAnsweredAndDone)
{
    int fd = connectClient();
    hello(fd);
    json answer = request(fd, {{"jsonrpc", "2.0"}, {"id", "r"}, {"method", "reboot"}, {"params", {{"task_id", "t-1"}}}});
    EXPECT_EQ(answer["id"], "r");
    EXPECT_EQ(answer["result"], json::object());
    EXPECT_EQ(system_actions.reboot_calls, 1);
}

TEST_F(PlayerProtocolTest, WhatTheDaemonDoesNotOfferIsMethodNotFound)
{
    int fd = connectClient();
    hello(fd);
    for (const char *method : {"installSoftware", "screenOff", "deepStandby", "setVolume", "dance"})
    {
        json answer = request(fd, {{"jsonrpc", "2.0"}, {"id", 3}, {"method", method}});
        EXPECT_EQ(errorCode(answer), -32601) << method;
    }
}

TEST_F(PlayerProtocolTest, BrokenMessagesGetTheirErrorAndTheDaemonGoesOn)
{
    int fd = connectClient();
    hello(fd);

    send(fd, "{this is no json");
    json answer = json::parse(readLine(fd));
    EXPECT_EQ(errorCode(answer), -32700);
    EXPECT_TRUE(answer["id"].is_null());

    EXPECT_EQ(errorCode(request(fd, {{"id", 4}, {"method", "reboot"}})), -32600); // no jsonrpc 2.0
    EXPECT_EQ(errorCode(request(fd, {{"jsonrpc", "2.0"}, {"id", 4}, {"method", 7}})), -32600);
    EXPECT_EQ(errorCode(request(fd, {{"jsonrpc", "2.0"}, {"id", 4}, {"method", "reboot"}, {"params", {{"task_id", 7}}}})),
              -32602); // wrong type
    EXPECT_EQ(system_actions.reboot_calls, 0);

    EXPECT_TRUE(request(fd, {{"jsonrpc", "2.0"}, {"id", 5}, {"method", "reboot"}}).contains("result"));
}

TEST_F(PlayerProtocolTest, NotificationsGetNoAnswer)
{
    int fd = connectClient();
    hello(fd);
    send(fd, json({{"jsonrpc", "2.0"}, {"method", "heartbeat"}, {"params", {{"uptime", 3}}}}).dump());
    send(fd, json({{"jsonrpc", "2.0"}, {"method", "dance"}}).dump());
    send(fd, json({{"jsonrpc", "2.0"}, {"id", 77}, {"result", {}}}).dump()); // an answer the daemon did not ask for
    EXPECT_EQ(readLine(fd, 200ms), "");
}

TEST_F(PlayerProtocolTest, TheTextProtocolOfBeforeStillWorks)
{
    int fd = connectClient();
    send(fd, "reboot");
    EXPECT_EQ(system_actions.reboot_calls, 1);
}

TEST_F(PlayerProtocolTest, ClosingByTheUserKeepsThePlayerClosed)
{
    int fd = connectClient();
    hello(fd);
    watchdog->check();
    pid_t pid = watchdog->getPlayerPid();
    ASSERT_GT(pid, 0);

    send(fd, json({{"jsonrpc", "2.0"}, {"method", "closing"}, {"params", {{"reason", "user"}}}}).dump());
    kill(pid, SIGTERM);
    std::this_thread::sleep_for(100ms);
    watchdog->check();

    EXPECT_TRUE(watchdog->isClosedByUser());
    EXPECT_EQ(watchdog->getPlayerPid(), -1);
}

TEST_F(PlayerProtocolTest, OnlyThePlayerMayCloseAndReboot)
{
    int fd = connectClient();
    hello(fd, "admin");
    watchdog->check();
    pid_t pid = watchdog->getPlayerPid();
    ASSERT_GT(pid, 0);

    EXPECT_EQ(errorCode(request(fd, {{"jsonrpc", "2.0"}, {"id", 2}, {"method", "reboot"}})), -32601);
    send(fd, json({{"jsonrpc", "2.0"}, {"method", "closing"}, {"params", {{"reason", "user"}}}}).dump());
    kill(pid, SIGTERM);
    std::this_thread::sleep_for(100ms);
    watchdog->check();

    EXPECT_FALSE(watchdog->isClosedByUser()); // the closing of an admin client does not count
    EXPECT_EQ(system_actions.reboot_calls, 0);
}

TEST_F(PlayerProtocolTest, RequestsToThePlayerOnlyWithItsCapability)
{
    int fd = connectClient();
    hello(fd, "player", {"setIndex", "quit"});

    EXPECT_TRUE(protocol->requestPlayer("setIndex", {{"uri", "/media/garlic/sda1/SMIL/index.smil"}}));
    json request = json::parse(readLine(fd));
    EXPECT_EQ(request["method"], "setIndex");
    EXPECT_EQ(request["params"]["uri"], "/media/garlic/sda1/SMIL/index.smil");
    EXPECT_TRUE(request.contains("id"));

    EXPECT_FALSE(protocol->requestPlayer("applyConfiguration", {{"path", "/x"}})); // not offered
}

TEST_F(PlayerProtocolTest, AGonePlayerGetsNoRequests)
{
    int fd = connectClient();
    hello(fd, "player", {"setIndex"});
    close(fd);
    std::erase(client_fds, fd);
    pump(); // the closed connection

    EXPECT_FALSE(protocol->requestPlayer("setIndex", {{"uri", "/x"}}));
}

TEST_F(PlayerProtocolTest, ANewerPlayerConnectionReplacesTheOlder)
{
    int old_fd = connectClient();
    hello(old_fd, "player", {"setIndex"});
    int new_fd = connectClient();
    hello(new_fd, "player", {"setIndex"});

    EXPECT_TRUE(protocol->requestPlayer("setIndex", {{"uri", "/x"}}));
    EXPECT_FALSE(readLine(new_fd).empty());
    EXPECT_EQ(readLine(old_fd, 200ms), "");
}
