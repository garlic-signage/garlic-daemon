# Protocol 1: the supervisor side

garlic-daemon speaks protocol 1 (`src/player_protocol.cpp`, described in [player-api.md](player-api.md)): hello,
heartbeat, closing, reboot, requests to the player; the text protocol of before still works. Not yet: quit before
stop, strict mode (lock), admin interface, back channel, garlic-service. The protocol itself (JSON-RPC 2.0, `hello`, `heartbeat`, `closing`, the methods in both
directions, errors) is described from the side of the player in the garlic-player repository:
`src/core/docs/supervisor_protocol.md`. This file holds what concerns the supervisor: garlic-daemon, the coming
garlic-service and the new Android launcher.

## Who implements it

| Supervisor     | Platform | Implementation                                                                  |
|----------------|----------|---------------------------------------------------------------------------------|
| garlic-daemon  | Linux    | C++                                                                             |
| garlic-service | Windows  | one C++ code base with garlic-daemon: protocol, watchdog, settings, back channel shared; the system actions per platform |
| launcher 2     | Android  | Java, without NDK: only one runtime on the device                               |

Launcher 1.x stays frozen (broadcasts, ContentProvider); the new player reaches it without this protocol.

The protocol exists twice, in C++ and in Java. Test cases (requests and expected answers as JSON lines) are read by the
tests of both, so they cannot drift apart. A test client that plays them over the socket checks every real supervisor.

The shared code base contradicts "Linux only" of [CONVENTIONS.md](../CONVENTIONS.md); that changes when garlic-service
comes.

## Endpoint and access

| Supervisor     | Endpoint                                               | Who may connect                                         |
|----------------|--------------------------------------------------------|---------------------------------------------------------|
| garlic-daemon  | `/run/garlic-daemon/garlic-daemon.sock` (config value `socket`) | `Player.user` and root, checked with `SO_PEERCRED`; the file mode does not matter |
| garlic-service | named pipe `\\.\pipe\garlic-service`                   | the user of the player and the service, by ACL          |
| launcher 2     | `LocalSocket`, abstract namespace `garlic-launcher`    | the uid of the player package (`getPeerCredentials()`)  |

- The supervisor starts the player with `--supervisor <endpoint>` (Android: intent extra `supervisor`), e.g.
  appended to `Player.command`.
- **Several clients**, as today: the player, the admin interface, `socat` for debugging. Each says in `hello` what it
  is (`client`: `player`, `admin`, `debug`). Requests for the player go only to the client `player`; only it sends
  `heartbeat`.

## From the line protocol of today

| garlic-daemon today | Protocol 1                       |
|---------------------|----------------------------------|
| `heartbeat`         | `heartbeat` (notification)       |
| `reboot`            | `reboot` with `task_id`          |
| `install_app`       | `installSoftware` with `path`    |
| `remove_app`        | dropped                          |
| `play_smil <path>`  | `setIndex` with `uri`            |
| `config <path>`     | `applyConfiguration` with `path` |

Migration: a line that starts with `{` is JSON-RPC, any other the old text, until no player with the text protocol is
left. The new player speaks only protocol 1. Socket, `SO_PEERCRED`, heartbeat timeout, backoff and exit code 127 stay.

## Watchdog

- `heartbeat_interval` in the answer to `hello`: `Player.heartbeat_timeout` / 3 to / 6, `0` when the hang detection
  is off. After the start the player has `heartbeat_timeout` seconds until its first heartbeat.
- **`closing`** replaces the missing exit code for "do not restart": after `reason: user` the daemon does not start the
  player again, after `restart` it does. An end without `closing` stays a crash (backoff).
- **Stop**: `quit` if the player offers it, wait for `closing` at most 10 s; then `SIGTERM` to the process group, after
  5 s `SIGKILL`. garlic-service: `quit`, then `TerminateProcess`.
- Before `reboot` and `installSoftware` the same stop, so the player does not die in the middle of a write.

## Settings

The player keeps its own settings (uuid, name, content URL, HTTP server, reboot times) also with a supervisor. The
supervisor owns:

| Setting                                        | How it gets them                                                 |
|------------------------------------------------|------------------------------------------------------------------|
| master volume, brightness, rotation, time zone, NTP, standby | from the player (`setVolume`, `setBrightness`, …), which reads every `configuration.xml`; the supervisor offers only what it can apply as capability |
| start delay of the player (launcher: 15 s), lock, service password | its own settings, from its own interface and later from the CMS |

- `device` in `hello` (`uuid`, `content_url`, `player_name`) is only a start value for a new player; the player stays
  the one truth of its identity.
- Capability `configuration`: the supervisor is the way name and content URL come in (launcher with its own dialog,
  `applyConfiguration`); the player then shows them in its dialog but does not change them.

## Lock

The strict mode of the launcher (kiosk, service password): nobody changes a public screen with a keyboard. The
supervisor has the state and tells the player: `locked` in `hello`, `setLocked` when the service mode changes. The
player then opens nothing a keyboard or a touch could reach.

**The player stays locked, it has no password of its own.** Only the supervisor unlocks: a technician enters its
service mode with the service password (launcher, admin interface), the supervisor sends `setLocked` with `false`, and
menu and dialogs of the player open again. So the password is checked in one place only, and the player offers nothing
to try passwords on. While locked there are no debug infos on site, only through the CMS, the REST API or `debug.log`.

garlic-daemon and garlic-service have no strict mode yet; without `locked` a desktop device on a public screen stays
open to the keyboard. A locked desktop device can be unlocked on site only when the admin interface exists, until
then only through the configuration of the daemon.

## Admin interface

garlic-daemon and garlic-service run without a user interface. For their own settings (start delay, lock, service
password; later network, screen, start and stop of the player) a small program of its own, Qt Widgets, a client
`admin` of the same protocol with methods of its own (e.g. `getSettings`, `changeSettings`, `setLocked`). The
supervisor checks the service password itself, the player never sees it. The supervisor shows it instead of the player when no player is installed or someone enters the
service mode with the password, like the launcher. Later everything is configured from the CMS as well.

## Back channel to the CMS

The supervisor, not the player, holds the connection to the CMS: it runs also when the player crashed, hangs or is
being updated, and it has the rights for restart, installation and network.

- **Connection**: WebSocket over TLS (`wss`), opened by the supervisor: through NAT and firewalls, no open port on the
  device.
- **Format**: JSON-RPC 2.0, one message per frame, the rules of protocol 1 (`hello`, capabilities, `id` and answer,
  unknown is ignored); one library for both.
- **CMS → device**: commands of the supervisor (`reboot`, `setLocked`, `changeSettings`, `installSoftware`); commands
  for the player (screenshot, load the index again, clear the cache) the supervisor passes on with protocol 1.
- **Device → CMS**: state on change (player runs or hangs, locked, version); the connection itself shows "online".
- **Pull stays**: index, tasks and reports go on through the player. Without back channel only the immediacy is
  missing.
- Implemented twice as well: in the C++ code base of daemon and service, and as a small Java library without Android
  dependencies in its core, for the launcher and other devices, with test cases of its own.

### Pairing

A device token in a `configuration.xml` would lie in files (download, USB stick, file system) and could be read on
the way. Instead:

1. At the first start the supervisor creates a key pair; the private key never leaves the device.
2. It connects to a pairing endpoint of the CMS without login and sends its public key, the uuid and a random code
   (at least 8 characters). The CMS keeps it as open request, about 10 minutes.
3. The supervisor shows the code on the screen (launcher, admin interface; never the player).
4. The installer enters the code in their account of the CMS. The CMS binds the request to the account and confirms it
   over the open connection.
5. From then on the device proves itself with its key (client certificate or signed challenge). The CMS can revoke it.

The pairing endpoint takes requests from anyone: rate limit and short life of open requests, so nobody floods it or
tries codes. Devices without back channel run as before.

### Other devices

A small Java app with the back channel library and an adapter to the local REST API of the device can give devices with
a foreign player a back channel too, e.g. IAdea (foreign APKs are allowed, a REST API exists, IAdea's own back channel
goes only to its own paid CMS). Its adapter checks at the start what the firmware offers; showing the pairing code
needs an overlay over the foreign player. Not planned yet.

## Open

- **IAdea**: how the app logs in to the REST API of the device; if it needs credentials, they have to come with the
  pairing.
