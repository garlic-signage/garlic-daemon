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

#ifndef PLAYER_PROTOCOL_HPP
#define PLAYER_PROTOCOL_HPP

#include <map>
#include <mutex>
#include <string>
#include <nlohmann/json.hpp>
#include "local_socket_server.hpp"
#include "message_dispatcher.hpp"
#include "system_actions.hpp"
#include "watchdog.hpp"

/**
 * Protocol 1 between player and daemon (docs/player-api.md): JSON-RPC 2.0, one message per line.
 * A line that starts with '{' is JSON-RPC, any other the text protocol of before (MessageDispatcher),
 * until no player with it is left.
 *
 * Per client: hello first (protocol, client "player", "admin" or "debug"), then requests and
 * notifications. Only the client "player" sends heartbeat and closing and gets requests of the daemon;
 * a newer player connection replaces an older one.
 *
 * The daemon offers only what it can do: reboot (as root). handle() is called by the listener thread
 * of the Daemon; requestPlayer() may be called from any thread.
 */
class PlayerProtocol
{
    public:
        static constexpr int PROTOCOL_VERSION = 1;

        PlayerProtocol(LocalSocketServer &socket_server, MessageDispatcher &legacy, SystemActions &system_actions,
                       Watchdog &watchdog, int heartbeat_timeout, std::string version);

        /// A line or a closed connection from the socket server.
        void handle(const LocalSocketServer::Message &message);

        /// Sends a request to the player, e.g. setIndex for a USB stick. false: no player or it does not offer it.
        bool requestPlayer(const std::string &method, const nlohmann::json &params);

        /// heartbeat_interval for hello: a third of the timeout, at least 1 s; 0 without hang detection.
        static int heartbeatInterval(int heartbeat_timeout);

    private:
        // codes of JSON-RPC 2.0 and of the range for implementations
        static constexpr int PARSE_ERROR      = -32700;
        static constexpr int INVALID_REQUEST  = -32600;
        static constexpr int METHOD_NOT_FOUND = -32601;
        static constexpr int INVALID_PARAMS   = -32602;
        static constexpr int FAILED           = -32000;
        static constexpr int NOT_READY        = -32001;
        static constexpr int NOT_SUPPORTED    = -32002;

        struct Client
        {
                std::string kind;                  // "player", "admin", "debug"
                std::vector<std::string> offers;  // capabilities of the player
        };

        LocalSocketServer &socket_server_;
        MessageDispatcher &legacy_;
        SystemActions &system_actions_;
        Watchdog &watchdog_;
        int heartbeat_timeout_;
        std::string version_;

        std::mutex mtx_; // requestPlayer() may come from another thread
        std::map<int, Client> clients_; // after hello
        int player_ = 0;                // client id of the player, 0: none
        long long next_id_ = 1;

        void handleJson(int client, const std::string &line);
        void handleRequest(int client, const nlohmann::json &message);
        void handleNotification(int client, const nlohmann::json &message);
        void hello(int client, const nlohmann::json &id, const nlohmann::json &params);
        void reboot(int client, const nlohmann::json &id, const nlohmann::json &params);

        void answer(int client, const nlohmann::json &id, const nlohmann::json &result = nlohmann::json::object());
        void fail(int client, const nlohmann::json &id, int code, const std::string &message,
                  const std::string &reason = "");
};

#endif // PLAYER_PROTOCOL_HPP
