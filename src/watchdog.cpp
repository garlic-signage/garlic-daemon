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
#include <fstream>
#include <system_error>
#include <thread>
#include <grp.h>
#include <pwd.h>
#include <sys/wait.h>
#include <unistd.h>
#include "log.hpp"

extern char **environ;

Watchdog::Watchdog(const Configuration &configuration, SystemActions &system_actions,
                   std::chrono::milliseconds stable_runtime)
    : player_name_(configuration.getPlayerName()), player_command_(configuration.getPlayerCommand()),
      player_user_(configuration.getPlayerUser()), player_environment_(configuration.getPlayerEnvironment()),
      heartbeat_timeout_(configuration.getHeartbeatTimeout()),
      reboot_after_crashes_(configuration.getRebootAfterCrashes()),
      reboot_stamp_file_(configuration.getStateDir() + "/last_reboot"), stable_runtime_(stable_runtime),
      system_actions_(system_actions)
{
}

Watchdog::~Watchdog()
{
    stopPlayer();
}

void Watchdog::check()
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (!is_active_)
        return;

    if (isRunning() && isHanging())
    {
        logWarning("{} sent no heartbeat for {} seconds, restart it.", player_name_, heartbeat_timeout_.count());
        stop();
        playerEnded(); // a hang counts like a crash
    }

    if (isRunning())
        return;

    if (std::chrono::steady_clock::now() < next_start_) // backoff after crashes
        return;

    start();
}

void Watchdog::heartbeat()
{
    std::lock_guard<std::mutex> lock(mtx_);
    last_heartbeat_ = std::chrono::steady_clock::now();
}

void Watchdog::setActivity(bool is_active)
{
    std::lock_guard<std::mutex> lock(mtx_);
    is_active_ = is_active;
}

void Watchdog::stopPlayer()
{
    std::lock_guard<std::mutex> lock(mtx_);
    stop();
    crash_count_ = 0;
    next_start_  = {};
}

bool Watchdog::isPlayerRunning()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return isRunning();
}

pid_t Watchdog::getPlayerPid()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return isRunning() ? player_pid_ : -1;
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
        passwd *pw = getpwnam(player_user_.c_str());
        if (pw == nullptr)
        {
            logError("User {} does not exist, cannot start {}", player_user_, player_name_);
            return;
        }
        uid  = pw->pw_uid;
        gid  = pw->pw_gid;
        home = pw->pw_dir;

        int count = 0;
        getgrouplist(player_user_.c_str(), gid, nullptr, &count); // returns the number of groups
        groups.resize(count);
        getgrouplist(player_user_.c_str(), gid, groups.data(), &count);
    }

    // Environment: as root a minimal one for the player user, otherwise the one of the daemon.
    // Entries from the config replace existing ones with the same name.
    std::vector<std::string> environment;
    if (switch_user)
        environment = {"HOME=" + home, "USER=" + player_user_, "LOGNAME=" + player_user_,
                       "PATH=/usr/local/bin:/usr/bin:/bin"};
    else
        for (char **entry = environ; *entry != nullptr; entry++)
            environment.emplace_back(*entry);

    for (const std::string &entry : player_environment_)
    {
        std::string name = entry.substr(0, entry.find('=') + 1); // "KEY="
        std::erase_if(environment, [&name](const std::string &e) { return e.starts_with(name); });
        environment.push_back(entry);
    }

    std::vector<char *> envp;
    for (std::string &entry : environment)
        envp.push_back(entry.data());
    envp.push_back(nullptr);

    const char *argv[] = {"/bin/sh", "-c", player_command_.c_str(), nullptr};

    sigset_t no_signals;
    sigemptyset(&no_signals);

    pid_t pid = fork();
    if (pid < 0)
    {
        logError("Cannot start {}: {}", player_name_, std::system_category().message(errno));
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
        _exit(EXIT_COMMAND_NOT_FOUND); // only reached when execve() failed
    }

    // Also set the process group here, otherwise a stop() right after the start could
    // happen before the child has called setpgid() and would not reach the player
    setpgid(pid, pid);

    player_pid_     = pid;
    started_at_     = std::chrono::steady_clock::now();
    last_heartbeat_ = started_at_; // the startup time counts like a heartbeat
    if (!player_missing_)          // otherwise logged in isRunning(), when the player is found
        logInfo("Started {} with pid {}: {}", player_name_, pid, player_command_);
}

/**
 * Sends SIGTERM to the process group of the player, waits up to STOP_TIMEOUT
 * and sends SIGKILL if it is still running.
 */
void Watchdog::stop()
{
    if (player_pid_ < 0)
        return;

    kill(-player_pid_, SIGTERM);

    auto deadline = std::chrono::steady_clock::now() + STOP_TIMEOUT;
    while (waitpid(player_pid_, nullptr, WNOHANG) == 0)
    {
        if (std::chrono::steady_clock::now() >= deadline)
        {
            logWarning("{} does not stop, send SIGKILL.", player_name_);
            kill(-player_pid_, SIGKILL);
            waitpid(player_pid_, nullptr, 0);
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    kill(-player_pid_, SIGKILL); // processes of the group that ignored SIGTERM
    logInfo("Stopped {} with pid {}", player_name_, player_pid_);
    player_pid_ = -1;
}

/**
 * Checks without blocking whether the player has ended. An ended player
 * is reaped, so no zombie process is left.
 */
bool Watchdog::isRunning()
{
    if (player_pid_ < 0)
        return false;

    int status   = 0;
    pid_t result = waitpid(player_pid_, &status, WNOHANG);
    if (result == 0)
    {
        if (player_missing_)
        {
            player_missing_ = false;
            logInfo("Found {}, started with pid {}: {}", player_name_, player_pid_, player_command_);
        }
        return true;
    }

    // 127: the shell did not find the command, see "Missing player" in watchdog.hpp
    if (result == player_pid_ && WIFEXITED(status) && WEXITSTATUS(status) == EXIT_COMMAND_NOT_FOUND)
    {
        if (!player_missing_)
            logError("{} not found: {}, waiting for it.", player_name_, player_command_);
        player_missing_ = true;
        kill(-player_pid_, SIGKILL);
        player_pid_ = -1;
        return false;
    }

    player_missing_ = false;
    if (result == player_pid_)
    {
        if (WIFEXITED(status))
            logWarning("{} exited with code {}", player_name_, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            logWarning("{} was killed by signal {}", player_name_, WTERMSIG(status));
    }
    kill(-player_pid_, SIGKILL); // processes of a start script that are still left
    player_pid_ = -1;
    playerEnded();
    return false;
}

bool Watchdog::isHanging() const
{
    if (heartbeat_timeout_.count() <= 0) // hang detection disabled
        return false;

    return std::chrono::steady_clock::now() - last_heartbeat_ > heartbeat_timeout_;
}

/**
 * Called when the player has crashed or hung. Calculates when it may be started again.
 */
void Watchdog::playerEnded()
{
    auto now = std::chrono::steady_clock::now();
    if (now - started_at_ < stable_runtime_)
        crash_count_++;
    else
        crash_count_ = 0; // it ran long enough, so this is no crash loop

    next_start_ = now + restartDelay(crash_count_);
    if (crash_count_ > 0)
        logWarning("{} crashed {} times in a row, next start in {} seconds.", player_name_, crash_count_,
                   restartDelay(crash_count_).count());

    if (reboot_after_crashes_ > 0 && crash_count_ >= reboot_after_crashes_)
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
    bool has_last_reboot  = static_cast<bool>(std::ifstream(reboot_stamp_file_) >> last_reboot);
    if (has_last_reboot)
    {
        auto since_last_reboot = now - std::chrono::system_clock::time_point(std::chrono::seconds(last_reboot));
        // Also true for a negative value, i.e. the clock went back (e.g. no RTC). Then rather do not reboot.
        if (since_last_reboot < REBOOT_INTERVAL)
        {
            logWarning("No reboot, the watchdog already rebooted within the last {} hours.",
                       std::chrono::duration_cast<std::chrono::hours>(REBOOT_INTERVAL).count());
            return;
        }
    }

    // Without the stamp the next boot could not know about this reboot, so a broken
    // player could cause a reboot loop. Therefore no stamp means no reboot.
    std::ofstream stamp(reboot_stamp_file_);
    stamp << std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() << std::endl;
    if (!stamp)
    {
        logError("No reboot, cannot write {}", reboot_stamp_file_);
        return;
    }
    stamp.close();

    logError("{} crashed {} times in a row, reboot the system.", player_name_, crash_count_);
    system_actions_.reboot();
}
