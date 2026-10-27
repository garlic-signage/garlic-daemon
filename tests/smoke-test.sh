#!/bin/sh
# Smoke test of the whole daemon: starts it with a player, sends a message over the
# socket and ends it with SIGTERM. The daemon must stop the player and remove the socket.
# Usage: smoke-test.sh <garlic-daemon binary>
set -eu

daemon="$1"
dir=$(mktemp -d /tmp/garlic-smoke-XXXXXX)
daemon_pid=""
trap 'if [ -n "$daemon_pid" ]; then kill "$daemon_pid" 2>/dev/null; fi; rm -rf "$dir"' EXIT

fail()
{
    echo "FAIL: $1"
    echo "--- log of the daemon:"
    cat "$dir/log"
    exit 1
}

# Repeats the command until it succeeds, at most 5 seconds
wait_for()
{
    for _ in $(seq 50); do
        if "$@"; then
            return 0
        fi
        sleep 0.1
    done
    return 1
}

# The player writes its pid and runs until it is stopped
cat > "$dir/garlic-daemon.conf" <<EOF
socket    = "$dir/garlic.sock";
state_dir = "$dir";
Usb       = { mount_dir = "$dir/usb"; };
Player    = {
    user    = "$(id -un)";
    command = "echo \$\$ > $dir/player.pid; exec sleep 30";
};
EOF

"$daemon" --config "$dir/garlic-daemon.conf" 2> "$dir/log" &
daemon_pid=$!

wait_for test -S "$dir/garlic.sock" || fail "socket was not created"
wait_for test -s "$dir/player.pid" || fail "player was not started"
player_pid=$(cat "$dir/player.pid")

# Like the player: one message per line
python3 -c 'import socket, sys
client = socket.socket(socket.AF_UNIX)
client.connect(sys.argv[1])
client.sendall(b"smoke-test\n")' "$dir/garlic.sock"
wait_for grep -q "Received message: smoke-test" "$dir/log" || fail "message was not received"

kill -TERM "$daemon_pid"
status=0
wait "$daemon_pid" || status=$?
daemon_pid=""
[ "$status" -eq 0 ] || fail "daemon ended with exit code $status instead of 0"

! kill -0 "$player_pid" 2>/dev/null || fail "player is still running"
[ ! -e "$dir/garlic.sock" ] || fail "socket file was not removed"
echo "OK"
