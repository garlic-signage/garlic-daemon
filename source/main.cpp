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

#include <filesystem>
#include "cmd_parser.hpp"
#include "daemon.hpp"
#include "log.hpp"
#include "watchdog.hpp"
#include "configuration.hpp"
#include "message_dispatcher.hpp"

int main(int argc, char **argv)
{
    CmdParser MyCmdParser(argc, argv);
    Configuration MyConfiguration;
    if (MyCmdParser.checkForConfigOption())
    {
        libconfig::Config config;
        config.readFile(MyCmdParser.getConfigFilePath().c_str());
        MyConfiguration.readLibConfigValues(&config);
    }
    std::string lockfile_name = "/tmp/" + MyConfiguration.getAppName() + ".lock";

#ifdef NDEBUG
    if (std::filesystem::exists(lockfile_name))
    {
    std:
        cerr << ("Application is already running!") << std::endl;
        return 1;
    }
    else
    {
        ofstream file;
        file.open(lockfile_name);
    }
#endif
    try
    {

        Watchdog MyWatchdog;
        IPCMessageReceiver MyIPCMessageReceiver("/garlic-daemon");
        MyIPCMessageReceiver.startIPCReceiver();
        SystemActions MySystemActions;
        MessageDispatcher MyMessageDispatcher(MySystemActions);

        Daemon &MyDaemon = Daemon::instance(); // The Daemon class is a singleton to avoid be instantiate more than once
        MyDaemon.setIPC(&MyIPCMessageReceiver);
        MyDaemon.setMessageDispatcher(&MyMessageDispatcher);
        std::thread WatchdogThread(&Daemon::startWatchdog, &Daemon::instance(), &MyWatchdog);
        std::thread MessageQueueThread(&Daemon::startMessageListener, &Daemon::instance());
        WatchdogThread.join();
        MessageQueueThread.join();
        MyIPCMessageReceiver.stopIPCReceiver();

        LOG_INFO("The daemon process ended gracefully.");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        std::remove(lockfile_name.c_str());
        return 1;
    }

    std::remove(lockfile_name.c_str()); // important to do as last entry
    return 0;
}
