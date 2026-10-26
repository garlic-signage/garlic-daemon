# garlic-daemon

Linux daemon for the [garlic-player](https://github.com/sagiadinos/garlic-player) digital signage player.

The player runs as a normal user. The daemon runs as root and does the things
the player is not allowed to do:

- **Watchdog**: starts the player and restarts it when it crashes or hangs
- **System commands** from the player, e.g. reboot

Planned: player updates, mounting USB sticks.

## Watchdog

The daemon starts the player itself with `/bin/sh -c <Player.command>` as `Player.user`.
Do not start the player additionally via a desktop autostart.

- The watchdog checks the player every 5 seconds and starts it again when it has ended.
- Hang detection: the player sends the message `heartbeat` regularly. If no heartbeat arrives
  for `Player.heartbeat_timeout` seconds, the player is stopped and started again.
  `0` disables it (default, until the player sends heartbeats).
- Stopping: SIGTERM to all processes of the player, after 5 seconds SIGKILL.
- When the daemon stops, the player is stopped too.

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

Without the option the prefix is `/usr/local`, then the config goes to `/usr/local/etc/garlic-daemon/`.

## Configuration

See `resource/config/garlic-daemon.conf` (libconfig syntax). Every value is optional.

`systemctl reload garlic-daemon` sends SIGHUP to the daemon, `systemctl stop` sends SIGTERM.

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
