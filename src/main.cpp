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

#include <print>
#include <pwd.h>
#include <unistd.h>
#include "cmd_parser.hpp"
#include "configuration.hpp"
#include "daemon.hpp"
#include "local_socket_server.hpp"
#include "log.hpp"
#include "message_dispatcher.hpp"
#include "usb_actions.hpp"
#include "usb_content_handler.hpp"
#include "usb_monitor.hpp"
#include "watchdog.hpp"

/**
 * The user that may connect to the socket besides root: Player.user when the
 * daemon runs as root, otherwise the user of the daemon (e.g. during development).
 */
static uid_t playerUid(const Configuration &configuration)
{
    if (getuid() != 0)
        return getuid();

    passwd *pw = getpwnam(configuration.getPlayerUser().c_str());
    if (pw == nullptr)
    {
        logWarning("User {} does not exist, only root may connect to the socket.", configuration.getPlayerUser());
        return 0;
    }
    return pw->pw_uid;
}

int main(int argc, char **argv)
{
    CmdParser cmd_parser(argc, argv);
    if (!cmd_parser.getError().empty())
    {
        std::print(stderr, "Error: {}\n\n{}", cmd_parser.getError(), CmdParser::helpText());
        return 2; // usual exit code for a wrong command line
    }

    if (cmd_parser.has("--help"))
    {
        std::print("{}", CmdParser::helpText());
        return 0;
    }

    if (cmd_parser.has("--version"))
    {
        std::println("garlic-daemon {}", GARLIC_DAEMON_VERSION);
        return 0;
    }

    try
    {
        logInfo("garlic-daemon {} starts.", GARLIC_DAEMON_VERSION);

        // Order: defaults, then config file, then command line options
        Configuration configuration;
        if (cmd_parser.has("--config"))
        {
            logInfo("Config file: {}", cmd_parser.value("--config"));
            libconfig::Config config;
            config.readFile(cmd_parser.value("--config").c_str());
            configuration.readLibConfigValues(&config);
        }
        configuration.applyCommandLine(cmd_parser);

        // Must happen before any thread is started, so all threads inherit the blocked signals
        Daemon::blockSignals();

        SystemActions system_actions;
        Watchdog watchdog(configuration, system_actions);
        LocalSocketServer socket_server(configuration.getSocketPath(), playerUid(configuration));
        MessageDispatcher dispatcher(system_actions, watchdog);

        UsbActions usb_actions; // stub, see usb_actions.hpp
        UsbContentHandler usb_content_handler(configuration, usb_actions);
        UsbMonitor usb_monitor(configuration, system_actions, usb_content_handler);

        Daemon daemon(watchdog, socket_server, dispatcher, usb_monitor);
        daemon.start();
        daemon.waitForTerminationSignal(); // blocks until SIGINT or SIGTERM

        logInfo("The daemon process ended gracefully.");
    }
    catch (const libconfig::FileIOException &)
    {
        logError("Cannot read config file {}", cmd_parser.value("--config"));
        return 1;
    }
    catch (const libconfig::ParseException &e)
    {
        logError("Config file {}:{} - {}", e.getFile(), e.getLine(), e.getError());
        return 1;
    }
    catch (const std::exception &e)
    {
        logError("{}", e.what());
        return 1;
    }

    return 0;
}
