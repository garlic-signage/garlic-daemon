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

#include "watchdog.hpp"

#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <fstream>
#include <thread>
#include <grp.h>
#include <pwd.h>
#include <sys/wait.h>
#include <unistd.h>
#include "log.hpp"

extern char **environ;

Watchdog::Watchdog(const Configuration &configuration, SystemActions &system_actions)
    : player_name(configuration.getPlayerName()), player_command(configuration.getPlayerCommand()),
      player_user(configuration.getPlayerUser()), player_environment(configuration.getPlayerEnvironment()),
      heartbeat_timeout(configuration.getHeartbeatTimeout()),
      reboot_after_crashes(configuration.getRebootAfterCrashes()),
      reboot_stamp_file(configuration.getStateDir() + "/last_reboot"), system_actions(system_actions)
{
}

Watchdog::~Watchdog()
{
    stopPlayer();
}

void Watchdog::check()
{
    std::lock_guard<std::mutex> lock(mtx);
    if (!is_active)
        return;

    if (isRunning() && isHanging())
    {
        LOG_WARNING(player_name, " sent no heartbeat for ", heartbeat_timeout.count(), " seconds, restart it.");
        stop();
        playerEnded(); // a hang counts like a crash
    }

    if (isRunning())
        return;

    if (std::chrono::steady_clock::now() < next_start) // backoff after crashes
        return;

    start();
}

void Watchdog::heartbeat()
{
    std::lock_guard<std::mutex> lock(mtx);
    last_heartbeat = std::chrono::steady_clock::now();
}

void Watchdog::setActivity(bool is_active)
{
    std::lock_guard<std::mutex> lock(mtx);
    this->is_active = is_active;
}

void Watchdog::stopPlayer()
{
    std::lock_guard<std::mutex> lock(mtx);
    stop();
    crash_count = 0;
    next_start  = {};
}

bool Watchdog::isPlayerRunning()
{
    std::lock_guard<std::mutex> lock(mtx);
    return isRunning();
}

pid_t Watchdog::getPlayerPid()
{
    std::lock_guard<std::mutex> lock(mtx);
    return isRunning() ? player_pid : -1;
}

std::chrono::seconds Watchdog::restartDelay(int crash_count)
{
    if (crash_count <= 0)
        return std::chrono::seconds(0);

    // 5, 10, 20, 40 ... seconds; the shift is limited, so it cannot overflow
    auto delay = BACKOFF_START * (1 << std::min(crash_count - 1, 10));
    return std::min<std::chrono::seconds>(delay, BACKOFF_MAX);
}

/**
 * Starts the player with "/bin/sh -c <command>".
 *
 * The daemon has several threads, so after fork() the child may only call
 * async-signal-safe functions. Therefore everything that allocates memory
 * (environment, user lookup) is prepared before fork().
 */
void Watchdog::start()
{
    // When the daemon runs as root, the player runs as player_user
    bool switch_user = (getuid() == 0);
    uid_t uid        = 0;
    gid_t gid        = 0;
    std::vector<gid_t> groups;
    std::string home = "/";
    if (switch_user)
    {
        passwd *pw = getpwnam(player_user.c_str());
        if (pw == nullptr)
        {
            LOG_ERROR("User ", player_user, " does not exist, cannot start ", player_name);
            return;
        }
        uid  = pw->pw_uid;
        gid  = pw->pw_gid;
        home = pw->pw_dir;

        int count = 0;
        getgrouplist(player_user.c_str(), gid, nullptr, &count); // returns the number of groups
        groups.resize(count);
        getgrouplist(player_user.c_str(), gid, groups.data(), &count);
    }

    // Environment: as root a minimal one for the player user, otherwise the one of the daemon.
    // Entries from the config replace existing ones with the same name.
    std::vector<std::string> environment;
    if (switch_user)
        environment = {"HOME=" + home, "USER=" + player_user, "LOGNAME=" + player_user,
                       "PATH=/usr/local/bin:/usr/bin:/bin"};
    else
        for (char **entry = environ; *entry != nullptr; entry++)
            environment.emplace_back(*entry);

    for (const std::string &entry : player_environment)
    {
        std::string name = entry.substr(0, entry.find('=') + 1); // "KEY="
        std::erase_if(environment, [&name](const std::string &e) { return e.starts_with(name); });
        environment.push_back(entry);
    }

    std::vector<char *> envp;
    for (std::string &entry : environment)
        envp.push_back(entry.data());
    envp.push_back(nullptr);

    const char *argv[] = {"/bin/sh", "-c", player_command.c_str(), nullptr};

    sigset_t no_signals;
    sigemptyset(&no_signals);

    pid_t pid = fork();
    if (pid < 0)
    {
        LOG_ERROR("Cannot start ", player_name, ": ", strerror(errno));
        return;
    }

    if (pid == 0) // child
    {
        setpgid(0, 0);                                  // own process group, see stop()
        sigprocmask(SIG_SETMASK, &no_signals, nullptr); // the daemon has blocked some signals
        if (switch_user)
        {
            if (setgroups(groups.size(), groups.data()) != 0 || setgid(gid) != 0 || setuid(uid) != 0)
                _exit(126);
        }
        if (chdir(home.c_str()) != 0)
            _exit(126);

        execve(argv[0], const_cast<char *const *>(argv), envp.data());
        _exit(127); // only reached when execve() failed
    }

    // Also set the process group here, otherwise a stop() right after the start could
    // happen before the child has called setpgid() and would not reach the player
    setpgid(pid, pid);

    player_pid     = pid;
    started_at     = std::chrono::steady_clock::now();
    last_heartbeat = started_at; // the startup time counts like a heartbeat
    LOG_INFO("Started ", player_name, " with pid ", pid, ": ", player_command);
}

/**
 * Sends SIGTERM to the process group of the player, waits up to STOP_TIMEOUT
 * and sends SIGKILL if it is still running.
 */
void Watchdog::stop()
{
    if (player_pid < 0)
        return;

    kill(-player_pid, SIGTERM);

    auto deadline = std::chrono::steady_clock::now() + STOP_TIMEOUT;
    while (waitpid(player_pid, nullptr, WNOHANG) == 0)
    {
        if (std::chrono::steady_clock::now() >= deadline)
        {
            LOG_WARNING(player_name, " does not stop, send SIGKILL.");
            kill(-player_pid, SIGKILL);
            waitpid(player_pid, nullptr, 0);
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    kill(-player_pid, SIGKILL); // processes of the group that ignored SIGTERM
    LOG_INFO("Stopped ", player_name, " with pid ", player_pid);
    player_pid = -1;
}

/**
 * Checks without blocking whether the player has ended. An ended player
 * is reaped, so no zombie process is left.
 */
bool Watchdog::isRunning()
{
    if (player_pid < 0)
        return false;

    int status   = 0;
    pid_t result = waitpid(player_pid, &status, WNOHANG);
    if (result == 0)
        return true;

    if (result == player_pid)
    {
        if (WIFEXITED(status))
            LOG_WARNING(player_name, " exited with code ", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            LOG_WARNING(player_name, " was killed by signal ", WTERMSIG(status));
    }
    kill(-player_pid, SIGKILL); // processes of a start script that are still left
    player_pid = -1;
    playerEnded();
    return false;
}

bool Watchdog::isHanging() const
{
    if (heartbeat_timeout.count() <= 0) // hang detection disabled
        return false;

    return std::chrono::steady_clock::now() - last_heartbeat > heartbeat_timeout;
}

/**
 * Called when the player has crashed or hung. Calculates when it may be started again.
 */
void Watchdog::playerEnded()
{
    auto now = std::chrono::steady_clock::now();
    if (now - started_at < STABLE_RUNTIME)
        crash_count++;
    else
        crash_count = 0; // it ran long enough, so this is no crash loop

    next_start = now + restartDelay(crash_count);
    if (crash_count > 0)
        LOG_WARNING(player_name, " crashed ", crash_count, " times in a row, next start in ",
                    restartDelay(crash_count).count(), " seconds.");

    if (reboot_after_crashes > 0 && crash_count >= reboot_after_crashes)
        rebootIfAllowed();
}

/**
 * Reboots, if the last reboot by the watchdog is at least REBOOT_INTERVAL ago.
 * The time is stored as seconds since 1970 (system clock), because the
 * steady clock starts again with every boot.
 */
void Watchdog::rebootIfAllowed()
{
    auto now = std::chrono::system_clock::now();

    long long last_reboot = 0;
    bool has_last_reboot  = static_cast<bool>(std::ifstream(reboot_stamp_file) >> last_reboot);
    if (has_last_reboot)
    {
        auto since_last_reboot = now - std::chrono::system_clock::time_point(std::chrono::seconds(last_reboot));
        // Also true for a negative value, i.e. the clock went back (e.g. no RTC). Then rather do not reboot.
        if (since_last_reboot < REBOOT_INTERVAL)
        {
            LOG_WARNING("No reboot, the watchdog already rebooted within the last ",
                        std::chrono::duration_cast<std::chrono::hours>(REBOOT_INTERVAL).count(), " hours.");
            return;
        }
    }

    // Without the stamp the next boot could not know about this reboot, so a broken
    // player could cause a reboot loop. Therefore no stamp means no reboot.
    std::ofstream stamp(reboot_stamp_file);
    stamp << std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() << std::endl;
    if (!stamp)
    {
        LOG_ERROR("No reboot, cannot write ", reboot_stamp_file);
        return;
    }
    stamp.close();

    LOG_ERROR(player_name, " crashed ", crash_count, " times in a row, reboot the system.");
    system_actions.reboot();
}
