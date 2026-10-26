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

#include "cmd_parser.hpp"
#include "configuration.hpp"
#include "daemon.hpp"
#include "log.hpp"
#include "message_dispatcher.hpp"
#include "watchdog.hpp"

int main(int argc, char **argv)
{
    CmdParser cmd_parser(argc, argv);
    try
    {
        Configuration configuration;
        if (cmd_parser.checkForConfigOption())
        {
            libconfig::Config config;
            config.readFile(cmd_parser.getConfigFilePath().c_str());
            configuration.readLibConfigValues(&config);
        }

        // Must happen before any thread is started, so all threads inherit the blocked signals
        Daemon::blockSignals();

        SystemActions system_actions;
        Watchdog watchdog(configuration, system_actions);
        IPCMessageReceiver receiver(configuration.getMessageQueue());
        MessageDispatcher dispatcher(system_actions, watchdog);

        Daemon daemon(watchdog, receiver, dispatcher);
        daemon.start();
        daemon.waitForTerminationSignal(); // blocks until SIGINT or SIGTERM

        LOG_INFO("The daemon process ended gracefully.");
    }
    catch (const libconfig::FileIOException &)
    {
        std::cerr << "Error: cannot read config file " << cmd_parser.getConfigFilePath() << std::endl;
        return 1;
    }
    catch (const libconfig::ParseException &e)
    {
        std::cerr << "Error: config file " << e.getFile() << ":" << e.getLine() << " - " << e.getError() << std::endl;
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
