# garlic-daemon

Linux daemon for the [garlic-player](https://github.com/sagiadinos/garlic-player) digital signage player.

The player runs as a normal user. The daemon runs as root and does the things
the player is not allowed to do:

- **Watchdog**: checks periodically whether the player process is running
- **System commands** from the player, e.g. reboot

Planned: restart of a crashed or hanging player, player updates, mounting USB sticks.

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
