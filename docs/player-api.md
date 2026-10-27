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
| Several connections | allowed, e.g. the player and `socat` for debugging. Messages from the daemon go to all of them |

The daemon checks the user of a connection with `SO_PEERCRED`, the kernel reports it.
The file permissions of the socket (`0666`) therefore do not matter.

**Daemon not running** (e.g. during its start or restart): connecting fails or the connection is closed.
The player should try again every few seconds. Messages sent while there is no connection are lost.

## Message format

- One message per line, ended by `\n`. `\r\n` is accepted too.
- A message is a command, optionally followed by a space and an argument: `play_smil /path/index.smil`.
  So far only messages from the daemon have arguments. The daemon compares the whole line,
  so e.g. `reboot now` is an unknown message.
- Commands are case-sensitive and have no leading or trailing spaces.
- UTF-8. A line may have at most 8192 bytes, a longer line closes the connection.
- Empty lines are ignored.
- There are **no answers**. The player does not get a confirmation that a command was executed.

## Messages from the player to the daemon

| Message | Action |
|---|---|
| `heartbeat` | the player is alive, resets the hang detection (see below) |
| `reboot` | the daemon syncs the disks and reboots the system |
| `install_app` | not implemented yet |
| `remove_app` | not implemented yet |

Unknown messages are logged and otherwise ignored.

## Messages from the daemon to the player

Planned, not implemented yet. The USB actions are still stubs (`src/usb_actions.cpp`):

| Message | Meaning |
|---|---|
| `play_smil <path>` | play the SMIL index at path, e.g. `/media/garlic/sda1/SMIL/index.smil` |
| `config <path>` | apply the player part (player name, content url ...) of the config.xml at path |

The player should ignore messages it does not know, so new messages do not break older players.

## Heartbeat

The daemon restarts the player when it sent no `heartbeat` for `Player.heartbeat_timeout` seconds.
The hang detection is off (`0`) by default, until the player sends heartbeats.

- Send `heartbeat` from a **timer in the GUI thread**, e.g. a `QTimer` every 10 seconds.
  Not from an extra thread: that thread would continue to send while the user interface hangs.
- Then set `heartbeat_timeout` to 3 to 6 times the interval, e.g. `60`.
- After its start the player has `heartbeat_timeout` seconds until the first heartbeat is expected.

## What the player gets from the daemon

The daemon normally runs as root (systemd unit). Then:

- **User**: the player runs as `Player.user` with its groups.
- **Environment**: `HOME`, `USER`, `LOGNAME`, `PATH=/usr/local/bin:/usr/bin:/bin`
  and the entries of `Player.environment`, e.g. `DISPLAY=:0`.
- **Working directory**: the home directory of `Player.user`.

When the daemon runs as normal user (e.g. during development), the player runs as the same user,
gets the environment of the daemon plus `Player.environment`, and the working directory `/`.

In both cases:

- **Process group**: the player gets its own. Child processes of the player are stopped together with it.
- **Stop**: `SIGTERM` to the process group, after 5 seconds `SIGKILL`.
  The player should handle `SIGTERM` and end within 5 seconds.
- **Exit**: when the player ends, the daemon starts it again, with a delay after repeated crashes
  (see README, sections Backoff and Reboot). An exit is always treated as crash,
  there is no exit code for "do not restart".

## USB sticks

- Sticks are mounted read-only to `/media/garlic/<device>`, e.g. `/media/garlic/sda1` (config value `Usb.mount_dir`).
- The files are readable for all users, so the player can read them.
- The mount point disappears when the stick is removed. The player has to expect that a path
  it got becomes invalid at any time.

## Example: Qt client

```cpp
#include <QLocalSocket>
#include <QTimer>

class DaemonConnection : public QObject
{
    public:
        DaemonConnection()
        {
            connect(&socket, &QLocalSocket::readyRead, this, &DaemonConnection::readLines);
            connect(&socket, &QLocalSocket::disconnected, this, [this] { reconnect_timer.start(); });
            connect(&socket, &QLocalSocket::errorOccurred, this, [this] { reconnect_timer.start(); });

            // Reconnect every 5 seconds while there is no connection
            reconnect_timer.setInterval(5000);
            reconnect_timer.setSingleShot(true);
            connect(&reconnect_timer, &QTimer::timeout, this, &DaemonConnection::connectToDaemon);

            // Heartbeat from the GUI thread
            heartbeat_timer.setInterval(10000);
            connect(&heartbeat_timer, &QTimer::timeout, this, [this] { send("heartbeat"); });
            heartbeat_timer.start();

            connectToDaemon();
        }

        void send(const QByteArray &message)
        {
            if (socket.state() == QLocalSocket::ConnectedState)
                socket.write(message + "\n");
        }

    private:
        QLocalSocket socket;
        QTimer reconnect_timer;
        QTimer heartbeat_timer;

        void connectToDaemon() { socket.connectToServer("/run/garlic-daemon/garlic-daemon.sock"); }

        void readLines()
        {
            while (socket.canReadLine())
            {
                QByteArray line = socket.readLine().trimmed();
                if (line.startsWith("play_smil "))
                    ; // play line.mid(10)
                // ignore unknown messages
            }
        }
};
```

## Testing by hand

```sh
# as root or as Player.user
echo heartbeat | socat - UNIX-CONNECT:/run/garlic-daemon/garlic-daemon.sock

# interactive, shows messages from the daemon too
socat - UNIX-CONNECT:/run/garlic-daemon/garlic-daemon.sock
```

The daemon logs connections and messages (except `heartbeat`): `journalctl -u garlic-daemon -f`
