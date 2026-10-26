# garlic-daemon

Linux daemon for the [garlic-player](https://github.com/sagiadinos/garlic-player) digital signage player.

The player runs as a normal user. The daemon runs as root and does the things
the player is not allowed to do:

- **Watchdog**: starts the player, restarts it when it crashes or hangs,
  and reboots the system if the player crashes again and again
- **System commands** from the player, e.g. reboot

Planned: player updates, mounting USB sticks.

## Watchdog

The daemon starts the player itself and keeps it running.
Do not start the player additionally via a desktop autostart, otherwise two players run.

### Start

- Command: `/bin/sh -c <Player.command>`, so a start script with arguments is possible.
- User: when the daemon runs as root, the player runs as `Player.user` with its groups,
  `HOME`, `USER`, `LOGNAME` and `PATH`. Otherwise it runs as the user of the daemon.
- Environment: the entries of `Player.environment` are added, e.g. `DISPLAY=:0`.
- The player gets its own process group. Stopping it stops all processes of the group,
  so also the processes a start script has started.

### Checks

The watchdog checks the player every 5 seconds:

| Situation | Detected by | Reaction |
|---|---|---|
| Player is not running (first start) | | start |
| Player has ended (crash, `kill`, exit) | `waitpid()`, the player is a child process | restart, see Backoff |
| Player hangs | no `heartbeat` message for `Player.heartbeat_timeout` seconds | stop, restart, see Backoff |

**Hang detection** needs the player: it has to send the message `heartbeat` regularly.
Send it from a timer in the GUI thread (e.g. a `QTimer` every 10 seconds), not from an extra thread,
otherwise heartbeats continue while the user interface hangs.
Set `heartbeat_timeout` to 3 to 6 times the interval, e.g. 60.
`0` (default) disables the hang detection, as long as the player does not send heartbeats.

**Stopping** the player: SIGTERM to the process group, after 5 seconds SIGKILL.
When the daemon stops, it stops the player too.

### Backoff

A player that crashes right after its start would otherwise be restarted every 5 seconds.
Therefore the watchdog counts the **crashes in a row**: a crash is an end (or a hang)
within 60 seconds after the start. The next start is delayed:

| Crashes in a row | 1 | 2 | 3 | 4 | 5 | 6 | 7 and more |
|---|---|---|---|---|---|---|---|
| Delay before next start | 5 s | 10 s | 20 s | 40 s | 80 s | 160 s | 300 s |

- A player that ran longer than 60 seconds resets the counter and is restarted without delay.
- Because the watchdog checks every 5 seconds, a crash is noticed up to 5 seconds later.
- A stop on purpose (e.g. for an update) is no crash, the next start comes without delay.

### Reboot

The cause of a crash loop can be the system and not the player, e.g. a hanging graphics driver
or a full memory. Therefore the system is rebooted after `Player.reboot_after_crashes` crashes
in a row (default 7: the crash at which the delay has reached 5 minutes, about 6 minutes after the first crash
when the player crashes right after each start).

A broken player (e.g. after a faulty update) would crash again after the reboot and cause
a reboot loop. Therefore:

- The watchdog reboots **at most once every 6 hours**. The time of its last reboot is stored
  in `<state_dir>/last_reboot` (default `/var/lib/garlic-daemon/last_reboot`), so it survives the reboot.
- Within the 6 hours it only restarts the player with the backoff and logs a warning.
- If the file cannot be written, there is no reboot.
- If the clock went back (e.g. no RTC and no network time yet), the last reboot seems to be
  in the future. Then there is no reboot either.
- `reboot_after_crashes = 0` disables the reboot.

The systemd unit creates `/var/lib/garlic-daemon` with `StateDirectory=`.

### Log messages

All messages go to the journal: `journalctl -u garlic-daemon`

| Message | Meaning |
|---|---|
| `Started garlic-player with pid …` | player started |
| `garlic-player exited with code …` / `was killed by signal …` | player has ended |
| `garlic-player sent no heartbeat for … seconds, restart it.` | hang detected |
| `garlic-player crashed … times in a row, next start in … seconds.` | backoff |
| `garlic-player crashed … times in a row, reboot the system.` | reboot |
| `No reboot, the watchdog already rebooted within the last 6 hours.` | reboot skipped |
| `No reboot, cannot write …` | reboot skipped, check `state_dir` |
| `garlic-player does not stop, send SIGKILL.` | player ignored SIGTERM |

## Requirements

- GCC 14 or newer (C++23), CMake 3.20 or newer
- libconfig++ (Debian: `libconfig++-dev`)
- GoogleTest for the tests (Debian: `libgtest-dev`)

## Build and test

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Without tests: `cmake -S . -B build -DBUILD_TESTS=OFF`

## Install

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
sudo cmake --install build
sudo systemctl enable --now garlic-daemon
```

With `-DCMAKE_INSTALL_PREFIX=/usr` this installs:

| File                                       | Path                                      |
|--------------------------------------------|-------------------------------------------|
| Binary                                     | `/usr/bin/garlic-daemon`                  |
| Config                                     | `/etc/garlic-daemon/garlic-daemon.conf`   |
| systemd unit                               | `/usr/lib/systemd/system/garlic-daemon.service` |
| State (created by systemd)                 | `/var/lib/garlic-daemon/`                 |

Without the option the prefix is `/usr/local`, then the config goes to `/usr/local/etc/garlic-daemon/`.

## Configuration

File: `/etc/garlic-daemon/garlic-daemon.conf`, libconfig syntax, see `resource/config/garlic-daemon.conf`.
Every value is optional. The daemon reads it at start: `garlic-daemon --config <file>`.
An invalid file ends the daemon with an error message.

| Value | Default | Meaning |
|---|---|---|
| `app_name` | `"garlic-daemon"` | name of the application |
| `message_queue` | `"/garlic-daemon"` | POSIX message queue for the messages of the player |
| `state_dir` | `"/var/lib/garlic-daemon"` | directory for files that must survive a reboot |
| `Player.name` | `"garlic-player"` | name in log messages |
| `Player.command` | `"/opt/garlic-player/garlic.sh"` | start command, run with `/bin/sh -c` |
| `Player.user` | `"digitalsignage"` | user the player runs as |
| `Player.environment` | `[]` | extra environment variables, e.g. `["DISPLAY=:0"]` |
| `Player.heartbeat_timeout` | `0` | seconds without heartbeat until restart, `0` = off |
| `Player.reboot_after_crashes` | `7` | crashes in a row until reboot, `0` = never |

Signals: `systemctl stop garlic-daemon` sends SIGTERM, the daemon stops the player and ends.
`systemctl reload garlic-daemon` sends SIGHUP, which is only logged so far.
To apply a changed config, restart the daemon: `systemctl restart garlic-daemon`.

## Messages

The player sends commands as plain text to the POSIX message queue `/garlic-daemon`
(configurable with `message_queue`):

| Message       | Action                     |
|---------------|----------------------------|
| `heartbeat`   | player is alive, see Watchdog |
| `reboot`      | syncs the disks and reboots |
| `install_app` | not implemented yet        |
| `remove_app`  | not implemented yet        |

Example: `mq_open("/garlic-daemon", O_WRONLY)` and `mq_send(mq, "reboot", 6, 0)`.

## Source code

- `src/`: all classes, `main.cpp` only wires them together
- `tests/`: one GoogleTest file per class
- `resource/`: config file and systemd unit template
- Rules for the code: see `CONVENTIONS.md`

## License

GNU Affero General Public License v3, see `LICENSE`.
