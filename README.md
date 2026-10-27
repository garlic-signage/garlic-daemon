# garlic-daemon

[![CI](https://github.com/sagiadinos/garlic-daemon/actions/workflows/ci.yml/badge.svg)](https://github.com/sagiadinos/garlic-daemon/actions/workflows/ci.yml)
![Coverage](https://raw.githubusercontent.com/sagiadinos/garlic-daemon/badges/coverage.svg)

Linux daemon for the [garlic-player](https://github.com/sagiadinos/garlic-player) digital signage player.

The player runs as a normal user. The daemon runs as root and does the things
the player is not allowed to do:

- **Watchdog**: starts the player, restarts it when it crashes or hangs,
  and reboots the system if the player crashes again and again
- **System commands** from the player, e.g. reboot
- **USB sticks**: mounts them and reacts to a configuration, a player update or SMIL content on them

Planned: the real actions for the USB stick content (so far stubs), player updates.

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
| Player is missing | exit code 127, the shell did not find `Player.command` | log once, try again every 5 seconds, no backoff, no reboot |

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
- A missing player (exit code 127) is no crash. A player installed later starts within 5 seconds.
  A start script should start the player with `exec`, so a missing player binary also ends with 127.

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

## USB sticks

The daemon mounts USB sticks itself, so the player can read them.

- It listens to udev events. A partition (or a whole stick without partitions) on USB with a
  supported filesystem (`vfat`, `exfat`, `ntfs3`, `ext2`, `ext3`, `ext4`) is mounted to
  `<Usb.mount_dir>/<device>`, e.g. `/media/garlic/sda1`.
- The mount is **read-only** with `nosuid`, `nodev`, `noexec`, so pulling the stick without
  unmounting cannot damage it and nothing on it can be executed.
- Sticks that are already plugged in when the daemon starts are mounted too.
- When the stick is removed, it is unmounted and the mount point is deleted.
  When the daemon stops, all sticks are unmounted.

### Content of the stick

After mounting, the daemon looks into the root directory of the stick. It handles **all** files it finds,
in this order, so one stick can set up a device completely:

| Order | File | Action |
|---|---|---|
| 1 | `config.xml` | the daemon applies the WiFi settings, the rest (player name, content url ...) goes to the player |
| 2 | `garlic-player.AppImage` (name from `Usb.firmware_file`) | updates the player |
| 3 | `SMIL/index.smil` | passes the SMIL index to the player (same layout as for the Android garlic-launcher) |

The actions are **stubs** so far (`src/usb_actions.cpp`), they only write a log message.

Only regular files count. Directories with these names and symlinks are ignored,
because a symlink on a stick could point to a file of the system.

## Log messages

All messages go to the journal: `journalctl -u garlic-daemon`

Each message has its priority (error, warning, info, debug) in the journal,
e.g. only warnings and errors: `journalctl -u garlic-daemon -p warning`

| Message | Meaning |
|---|---|
| `Started garlic-player with pid …` | player started |
| `User … does not exist, cannot start garlic-player` | `Player.user` is missing on the system, the player is not started |
| `garlic-player not found: …, waiting for it.` | `Player.command` does not exist (exit code 127), logged once |
| `Found garlic-player, started with pid …` | the missing player exists now and runs |
| `garlic-player exited with code …` / `was killed by signal …` | player has ended |
| `garlic-player sent no heartbeat for … seconds, restart it.` | hang detected |
| `garlic-player crashed … times in a row, next start in … seconds.` | backoff |
| `garlic-player crashed … times in a row, reboot the system.` | reboot |
| `No reboot, the watchdog already rebooted within the last 6 hours.` | reboot skipped |
| `No reboot, cannot write …` | reboot skipped, check `state_dir` |
| `garlic-player does not stop, send SIGKILL.` | player ignored SIGTERM |
| `Mounted USB device … to …` / `Unmounted USB device …` | USB stick plugged in / removed |
| `USB device … has unsupported filesystem …` | stick not mounted, e.g. encrypted or swap |
| `Cannot mount … to …` | mount failed, e.g. damaged filesystem |
| `Not implemented yet: …` | stub of a USB action, see above |
| `Client connected.` / `Client disconnected.` | player (or another client) connected to the socket |
| `Rejected connection from uid …` | a user other than `Player.user` or root tried to connect |
| `Unknown message …, ignored.` | a client sent a message the daemon does not know, see [docs/player-api.md](docs/player-api.md) |
| `Cannot read config file …` | the config file does not exist or is not readable, the daemon ends |
| `Config file …:… - …` | syntax error in the config file (file, line, error), the daemon ends |
| `Config value … has the wrong type` | e.g. `heartbeat_timeout = "60"` instead of `60`, the daemon ends |

## Requirements

- GCC 14 or newer (C++23), CMake 3.20 or newer
- libconfig++ (Debian: `libconfig++-dev`)
- libudev (Debian: `libudev-dev`)
- GoogleTest for the tests (Debian: `libgtest-dev`), python3 for the smoke test

## Build and test

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Without tests: `cmake -S . -B build -DBUILD_TESTS=OFF`

Warnings as errors: `cmake -S . -B build -DWARNINGS_AS_ERRORS=ON`. The code must build without warnings, see `CONVENTIONS.md`.

CI: GitHub Actions builds and tests every push on Debian Trixie (GCC 14) with warnings as errors,
see `.github/workflows/ci.yml`. The tests run with coverage, the badge above shows the line coverage of `src/`.

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
| Socket (created at runtime)                | `/run/garlic-daemon/garlic-daemon.sock`   |

Without the option the prefix is `/usr/local`, then the config goes to `/usr/local/etc/garlic-daemon/`.

## Configuration

File: `/etc/garlic-daemon/garlic-daemon.conf`, libconfig syntax, see `resource/config/garlic-daemon.conf`.
Every value is optional. The daemon reads it at start: `garlic-daemon --config <file>`.
An invalid file ends the daemon with an error message.

| Value | Default | Meaning |
|---|---|---|
| `socket` | `"/run/garlic-daemon/garlic-daemon.sock"` | Unix socket for the player, see [docs/player-api.md](docs/player-api.md) |
| `state_dir` | `"/var/lib/garlic-daemon"` | directory for files that must survive a reboot |
| `Usb.mount_dir` | `"/media/garlic"` | USB sticks are mounted to `<mount_dir>/<device>` |
| `Usb.firmware_file` | `"garlic-player.AppImage"` | file name of a player update on the stick |
| `Player.name` | `"garlic-player"` | name in log messages |
| `Player.command` | `"/opt/garlic-player/garlic.sh"` | start command, run with `/bin/sh -c` |
| `Player.user` | `"digitalsignage"` | user the player runs as |
| `Player.environment` | `[]` | extra environment variables as `"KEY=VALUE"`, e.g. `["DISPLAY=:0"]`, they replace variables with the same name. An entry without `=` ends the daemon with an error |
| `Player.heartbeat_timeout` | `0` | seconds without heartbeat until restart, `0` = off |
| `Player.reboot_after_crashes` | `7` | crashes in a row until reboot, `0` = never |

Signals: `systemctl stop garlic-daemon` sends SIGTERM, the daemon stops the player and ends.
`systemctl reload garlic-daemon` sends SIGHUP, which is only logged so far.
To apply a changed config, restart the daemon: `systemctl restart garlic-daemon`.

## Command line options

```
garlic-daemon [options]
```

| Option | Meaning |
|---|---|
| `--config <file>` | read the configuration from file |
| `--socket <path>` | Unix socket for the player (config: `socket`) |
| `--player-command <command>` | command that starts the player (config: `Player.command`) |
| `--heartbeat-timeout <seconds>` | restart the player without heartbeat, `0` = off (config: `Player.heartbeat_timeout`) |
| `--state-dir <dir>` | directory for files that survive a reboot (config: `state_dir`) |
| `--usb-mount-dir <dir>` | USB sticks are mounted below this directory (config: `Usb.mount_dir`) |
| `--help` | show the options and exit |
| `--version` | show the version and exit |

- Order of the values: default, then config file, then command line option.
- An unknown option, an argument without option or a missing value ends the daemon with exit code 2,
  so a typo is not silently ignored.

**Test run as normal user**, without config file and without root:

```sh
garlic-daemon --socket /tmp/garlic.sock --player-command "sleep 1000" \
              --state-dir /tmp --usb-mount-dir /tmp/usb
echo heartbeat | socat - UNIX-CONNECT:/tmp/garlic.sock
```

As normal user the daemon cannot mount USB sticks or reboot, it logs an error instead.

## Player API

The player talks to the daemon over the Unix socket `/run/garlic-daemon/garlic-daemon.sock`,
one message per line, e.g. `heartbeat` or `reboot`.
Everything the player needs (connection, messages, heartbeat, signals, USB paths, Qt example)
is described in [docs/player-api.md](docs/player-api.md).

## Source code

- `src/`: all classes, `main.cpp` only wires them together
- `tests/`: one GoogleTest file per class
- `resource/`: config file and systemd unit template
- `docs/`: further documentation, e.g. the player API
- Rules for the code: see `CONVENTIONS.md`

## License

GNU Affero General Public License v3, see `LICENSE`.
