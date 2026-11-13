# Player API

Everything the garlic-player needs to work together with the garlic-daemon.

## Overview

```
garlic-daemon (root)                         garlic-player (Player.user)
 ├─ starts and watches the player  ───────►  child process of the daemon
 └─ Unix socket  ◄──────── one line per message ────────►  QLocalSocket
```

- The daemon **starts the player** itself (see README, section Watchdog).
  Do not start it additionally via a desktop autostart.
- Daemon and player talk over a **Unix domain socket**, in both directions.

## Connection

| | |
|---|---|
| Socket | `/run/garlic-daemon/garlic-daemon.sock` (config value `socket`) |
| Type | stream socket (`SOCK_STREAM`), Qt: `QLocalSocket` |
| Allowed users | `Player.user` and root, all other connections are closed at once |
| Several connections | allowed, e.g. the player and `socat` for debugging. Answers of protocol 1 go to the client that asked, requests of the daemon only to the player |

The daemon checks the user of a connection with `SO_PEERCRED`, the kernel reports it.
The file permissions of the socket (`0666`) therefore do not matter.

**Daemon not running** (e.g. during its start or restart): connecting fails or the connection is closed.
The player should try again every few seconds. Messages sent while there is no connection are lost.

## Protocol 1

[JSON-RPC 2.0](https://www.jsonrpc.org/specification), one message per line, as described from the side of the player
in the garlic-player repository (`src/core/docs/supervisor_protocol.md`); the plan of the supervisor side is
[protocol-1.md](protocol-1.md). Implemented in `PlayerProtocol` (`src/player_protocol.cpp`).

- One JSON object per line, UTF-8, ended by `\n` (`\r\n` is accepted). At most 64 KiB per line, a longer line
  closes the connection.
- A line that starts with `{` is JSON-RPC, any other the [text protocol](#text-protocol-of-before) of before.
- The player starts with `hello`; before it every request gets `-32001 not ready`.
- Start the player with `--supervisor <socket>`, e.g. `Player.command = "garlic-player --supervisor
  /run/garlic-daemon/garlic-daemon.sock"`; the daemon does not add it, an older player would not know the option.

### hello

```json
{"jsonrpc":"2.0","id":1,"method":"hello","params":{"protocol":1,"client":"player","capabilities":["applyConfiguration","setIndex","quit","setLocked"]}}
{"jsonrpc":"2.0","id":1,"result":{"protocol":1,"supervisor":{"name":"garlic-daemon","version":"0.1.0"},"capabilities":["reboot"],"heartbeat_interval":20,"locked":false}}
```

| Field                | The daemon                                                                      |
|----------------------|---------------------------------------------------------------------------------|
| `protocol`           | 1; a client with a lower one gets `-32002 protocol not supported` and is disconnected |
| `client`             | `player` (default), `admin`, `debug`: only the player sends `heartbeat` and `closing`, may reboot and gets requests of the daemon; a newer player connection replaces the older |
| `capabilities`       | `reboot`, only when the daemon runs as root                                     |
| `heartbeat_interval` | `Player.heartbeat_timeout` / 3, at least 1; `0` when the hang detection is off  |
| `locked`             | always `false`, the daemon has no strict mode yet                               |
| `device`             | not sent: the daemon has no uuid or content URL of its own                     |

### Player → daemon

| Message     | Kind         | The daemon                                                                    |
|-------------|--------------|-------------------------------------------------------------------------------|
| `reboot`    | request      | answers `{}`, then `systemctl --no-block reboot`; not as root: `-32000` with `no permission`. `task_id` is logged |
| `heartbeat` | notification | resets the hang detection                                                     |
| `closing`   | notification | the next end is no crash: `reason` `user` (exit in the menu): the player is not started again until the daemon restarts; any other: started again at once, without backoff |
| any other request | request | `-32601 method not found` (e.g. `installSoftware`, `screenOff`: not offered) |
| any other notification | notification | ignored                                               |

### Daemon → player

`PlayerProtocol::requestPlayer()` sends requests the player offers in its `hello`, e.g. `setIndex` with `uri` or
`applyConfiguration` with `path`; nothing uses it yet (the USB actions are stubs). Answers of the player are not
waited for, errors are logged.

### Errors

`-32700` not JSON (`id` null), `-32600` no `jsonrpc` 2.0 or `method` no string, `-32601` method not found,
`-32602` a field of the wrong type (e.g. `task_id` a number), `-32000` failed (`data.reason`), `-32001` before
`hello`, `-32002` protocol not supported. A broken message never stops the daemon.

## Text protocol of before

For players that do not speak protocol 1 yet, until none is left.

- A message is a command, optionally followed by a space and an argument: `play_smil /path/index.smil`.
  The daemon compares the whole line, so e.g. `reboot now` is an unknown message.
- Commands are case-sensitive and have no leading or trailing spaces. Empty lines are ignored.
- There are **no answers**. The player does not get a confirmation that a command was executed.

| Message from the player | Action |
|---|---|
| `heartbeat` | the player is alive, resets the hang detection (see below) |
| `reboot` | the daemon reboots the system with `systemctl --no-block reboot`, so the player gets `SIGTERM` first. Only as root |
| `install_app` | not implemented yet |
| `remove_app` | not implemented yet |

Unknown messages are logged and otherwise ignored. Messages from the daemon to the player (`play_smil <path>`,
`config <path>`) were planned for the text protocol; they come as `setIndex` and `applyConfiguration` of protocol 1.

## Heartbeat

The daemon restarts the player when it sent no `heartbeat` for `Player.heartbeat_timeout` seconds.
The hang detection is off (`0`) by default, until the player sends heartbeats.

- Send `heartbeat` from a **timer in the GUI thread**, every `heartbeat_interval` of `hello`.
  Not from an extra thread: that thread would continue to send while the user interface hangs.
- Set `heartbeat_timeout` to 3 to 6 times the interval, e.g. `60`; protocol 1 tells the player a third of it.
- After its start the player has `heartbeat_timeout` seconds until the first heartbeat is expected.

## What the player gets from the daemon

The daemon normally runs as root (systemd unit). Then:

- **User**: the player runs as `Player.user` with its groups.
- **Environment**: `HOME`, `USER`, `LOGNAME`, `PATH=/usr/local/bin:/usr/bin:/bin`
  and the entries of `Player.environment`, e.g. `DISPLAY=:0`.
- **Working directory**: the home directory of `Player.user`, `/` if it does not exist.

When the daemon runs as normal user (e.g. during development), the player runs as the same user,
gets the environment of the daemon plus `Player.environment`, and the working directory `/`.
The message `reboot` and the watchdog then do not reboot, the daemon only logs a warning.

In both cases:

- **Process group**: the player gets its own. Child processes of the player are stopped together with it.
- **Stop**: `SIGTERM` to the process group, after 5 seconds `SIGKILL`.
  The player should handle `SIGTERM` and end within 5 seconds.
- **Exit**: when the player ends, the daemon starts it again, with a delay after repeated crashes
  (see README, sections Backoff and Reboot). An end without `closing` (protocol 1) is a crash;
  after `closing` with `user` the player stays closed, see [Protocol 1](#protocol-1).
  Exit code 127 means "player not found" and is no crash, so the player must not use it.

## USB sticks

- Sticks are mounted read-only to `/media/garlic/<device>`, e.g. `/media/garlic/sda1` (config value `Usb.mount_dir`).
- The files are readable for all users, so the player can read them.
- The mount point disappears when the stick is removed. The player has to expect that a path
  it got becomes invalid at any time.

## Example: client

The garlic-player has its client in `src/player-ui/supervisor` (`SupervisorClient`, Qt `QLocalSocket`). By hand:

```sh
socat - UNIX-CONNECT:/run/garlic-daemon/garlic-daemon.sock
{"jsonrpc":"2.0","id":1,"method":"hello","params":{"protocol":1,"client":"debug"}}
```

## Testing by hand

```sh
# as root or as Player.user; the text protocol of before
echo heartbeat | socat - UNIX-CONNECT:/run/garlic-daemon/garlic-daemon.sock

# interactive, shows messages from the daemon too
socat - UNIX-CONNECT:/run/garlic-daemon/garlic-daemon.sock
```

The daemon logs connections and messages (except `heartbeat`): `journalctl -u garlic-daemon -f`
