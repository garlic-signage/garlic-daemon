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

#include "player_protocol.hpp"

#include <algorithm>
#include <utility>
#include "log.hpp"

using nlohmann::json;

PlayerProtocol::PlayerProtocol(LocalSocketServer &socket_server, MessageDispatcher &legacy,
                               SystemActions &system_actions, Watchdog &watchdog, int heartbeat_timeout,
                               std::string version)
    : socket_server_(socket_server), legacy_(legacy), system_actions_(system_actions), watchdog_(watchdog),
      heartbeat_timeout_(heartbeat_timeout), version_(std::move(version))
{
}

int PlayerProtocol::heartbeatInterval(int heartbeat_timeout)
{
    if (heartbeat_timeout <= 0)
        return 0;
    return std::max(1, heartbeat_timeout / 3);
}

void PlayerProtocol::handle(const LocalSocketServer::Message &message)
{
    if (message.closed)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        clients_.erase(message.client);
        if (player_ == message.client)
            player_ = 0;
        return;
    }

    if (message.text.starts_with('{'))
    {
        handleJson(message.client, message.text);
        return;
    }

    if (message.text != "heartbeat") // comes every few seconds and would flood the log
        logInfo("Received message: {}", message.text);
    legacy_.dispatch(message.text);
}

void PlayerProtocol::handleJson(int client, const std::string &line)
{
    json message = json::parse(line, nullptr, false); // no exceptions: discarded on error
    if (message.is_discarded() || !message.is_object())
    {
        fail(client, nullptr, PARSE_ERROR, "parse error");
        return;
    }

    json id = message.value("id", json());
    if (message.value("jsonrpc", "") != "2.0")
    {
        fail(client, id, INVALID_REQUEST, "invalid request");
        return;
    }

    if (message.contains("method"))
    {
        if (!message["method"].is_string())
        {
            fail(client, id, INVALID_REQUEST, "invalid request");
            return;
        }
        // a field of the wrong type (e.g. "task_id": 7) throws in json::value(): invalid params, the daemon goes on
        try
        {
            if (message.contains("id"))
                handleRequest(client, message);
            else
                handleNotification(client, message);
        }
        catch (const json::exception &e)
        {
            if (message.contains("id"))
                fail(client, id, INVALID_PARAMS, "invalid params", e.what());
        }
        return;
    }

    if (message.contains("id") && (message.contains("result") || message.contains("error")))
    {
        if (message.contains("error")) // answers of the player to requests of the daemon
            logWarning("The player answered with an error: {}", message["error"].dump());
        return;
    }
    fail(client, id, INVALID_REQUEST, "invalid request");
}

void PlayerProtocol::handleRequest(int client, const json &message)
{
    json id               = message["id"];
    std::string method    = message["method"];
    json params           = message.value("params", json::object());

    if (method == "hello")
    {
        hello(client, id, params);
        return;
    }

    bool is_player = false;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!clients_.contains(client))
        {
            fail(client, id, NOT_READY, "not ready");
            return;
        }
        is_player = (client == player_);
    }

    if (method == "reboot" && is_player)
        reboot(client, id, params);
    else
        fail(client, id, METHOD_NOT_FOUND, "method not found"); // also what the daemon does not offer
}

void PlayerProtocol::handleNotification(int client, const json &message)
{
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (client != player_)
            return; // heartbeat and closing only from the player, unknown is ignored
    }

    std::string method = message["method"];
    if (method == "heartbeat")
    {
        watchdog_.heartbeat();
        return;
    }
    if (method == "closing")
    {
        json params        = message.value("params", json::object());
        std::string reason = params.is_object() ? params.value("reason", "") : "";
        logInfo("The player is closing: {}", reason.empty() ? "no reason" : reason);
        watchdog_.playerClosing(reason != "user"); // "user": exit in the menu, do not start it again
    }
}

void PlayerProtocol::hello(int client, const json &id, const json &params)
{
    int protocol = params.is_object() ? params.value("protocol", 0) : 0;
    if (protocol < PROTOCOL_VERSION)
    {
        fail(client, id, NOT_SUPPORTED, "protocol not supported");
        socket_server_.disconnect(client);
        return;
    }

    Client state;
    state.kind = params.value("client", "player");
    if (params.contains("capabilities") && params["capabilities"].is_array())
        for (const json &capability : params["capabilities"])
            if (capability.is_string())
                state.offers.push_back(capability);

    json capabilities = json::array();
    if (system_actions_.canReboot())
        capabilities.push_back("reboot");

    json result = {
        {"protocol", PROTOCOL_VERSION},
        {"supervisor", {{"name", "garlic-daemon"}, {"version", version_}}},
        {"capabilities", capabilities},
        {"heartbeat_interval", heartbeatInterval(heartbeat_timeout_)},
        {"locked", false}, // no strict mode yet
    };
    {
        std::lock_guard<std::mutex> lock(mtx_);
        bool is_player   = (state.kind == "player");
        clients_[client] = std::move(state);
        if (is_player)
            player_ = client; // a newer connection of the player replaces the old one
    }
    if (params.contains("player") && params["player"].is_object())
        logInfo("Hello from {} {} ({})", params["player"].value("name", "a player"),
                params["player"].value("version", "?"), params.value("client", "player"));
    answer(client, id, result);
}

void PlayerProtocol::reboot(int client, const json &id, const json &params)
{
    std::string task_id = params.is_object() ? params.value("task_id", "") : "";
    if (!system_actions_.canReboot())
    {
        fail(client, id, FAILED, "failed", "no permission");
        return;
    }
    logInfo("Reboot requested by the player, task {}", task_id.empty() ? "-" : task_id);
    answer(client, id); // before the reboot: the player learns that it was taken over
    system_actions_.reboot();
}

bool PlayerProtocol::requestPlayer(const std::string &method, const json &params)
{
    int player = 0;
    long long id = 0;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        auto found = clients_.find(player_);
        if (found == clients_.end() || std::ranges::find(found->second.offers, method) == found->second.offers.end())
            return false;
        player = player_;
        id     = next_id_++;
    }
    json request = {{"jsonrpc", "2.0"}, {"id", id}, {"method", method}, {"params", params}};
    socket_server_.sendTo(player, request.dump());
    return true;
}

void PlayerProtocol::answer(int client, const json &id, const json &result)
{
    json message = {{"jsonrpc", "2.0"}, {"id", id}, {"result", result}};
    socket_server_.sendTo(client, message.dump());
}

void PlayerProtocol::fail(int client, const json &id, int code, const std::string &message, const std::string &reason)
{
    json error = {{"code", code}, {"message", message}};
    if (!reason.empty())
        error["data"] = {{"reason", reason}};
    json answer = {{"jsonrpc", "2.0"}, {"id", id}, {"error", error}};
    socket_server_.sendTo(client, answer.dump());
}
