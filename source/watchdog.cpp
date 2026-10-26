/*************************************************************************************
    garlic-daemon: Linux daemon for garlic-player
    Copyright (C) 2023 Nikolaos Saghiadinos <ns@smil-control.com>
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

#include "watchdog.hpp"

Watchdog::Watchdog(const string &proc_path) : proc_path(proc_path) {}

void Watchdog::setActivity(bool is_active)
{
    this->is_active = is_active;
}

void Watchdog::check()
{
    if (!isPlayerRunning())
    {
        std::cerr << "garlic-player needs to be started" << std::endl;
    }
}

void Watchdog::startPlayer()
{
    std::system(path_to_player_executable.c_str());
}

bool Watchdog::isPlayerRunning()
{
    if (!is_active) // if watchdog is not active return always true
        return true;

    return (getPlayerProcId() > -1);
}

int Watchdog::getPlayerProcId()
{
    int pid = -1;

    DIR *dp = opendir(proc_path.c_str());
    if (dp == NULL)
        return pid;

    struct dirent *dirp;
    while (pid < 0 && (dirp = readdir(dp)))
    {
        int id = atoi(dirp->d_name); // Skip non-numeric entries
        if (id > 0 && isInCmdLine(dirp->d_name))
        {
            pid = id;
        }
    }
    closedir(dp);

    // ToDo: Check if it is neccessary to reboot if the player is an zombie and cannot restarted

    return pid;
}

bool Watchdog::isInCmdLine(string dir_name)
{
    // Read contents of virtual /proc/{pid}/cmdline file
    string cmdPath = proc_path + "/" + dir_name + "/cmdline";
    ifstream cmdFile(cmdPath.c_str());
    string cmdLine;

    getline(cmdFile, cmdLine);
    if (!cmdLine.empty())
    {
        // Keep first cmdline item which contains the program path
        size_t pos = cmdLine.find('\0');
        if (pos != string::npos)
            cmdLine = cmdLine.substr(0, pos);

        // Keep program name only, removing the path
        pos = cmdLine.rfind('/');
        if (pos != string::npos)
            cmdLine = cmdLine.substr(pos + 1);

        return (player_name == cmdLine);
    }

    return false;
}

bool Watchdog::isZombie(int pid)
{
    bool is_zombie = false;

    string stat_path = proc_path + "/" + to_string(pid) + "/stat";

    FILE *fpstat = fopen(stat_path.c_str(), "r");
    if (!fpstat)
    {
        perror(stat_path.c_str());
        exit(EXIT_FAILURE);
    };
    {
        int rpid = 0;
        char rcmd[32];
        char rstatc = 0;
        fscanf(fpstat, "%d %30s %c", &rpid, rcmd, &rstatc);
        is_zombie = rstatc == 'Z';
    }

    fclose(fpstat);

    return is_zombie;
}
