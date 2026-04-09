#!/usr/bin/env python3
"""Game test client - sends commands to game via pipe."""

import os
import sys
import time
from pathlib import Path

PROJECT_ROOT = Path("/Users/rexarrior/programming/my/netshtorm_like_game")
GAME_EXE = PROJECT_ROOT / "build" / "netstorm_like"
SCREENSHOTS_DIR = PROJECT_ROOT / "test_screenshots"
CONTROL_PIPE = PROJECT_ROOT / ".game_control_pipe"

SCREENSHOTS_DIR.mkdir(exist_ok=True)


class PipeClient:
    """Persistent pipe connection to the game."""

    def __init__(self, pipe_path):
        self.pipe_path = pipe_path
        self.f = None

    def connect(self):
        self.f = open(self.pipe_path, "w")

    def send(self, cmd):
        if self.f:
            self.f.write(cmd + "\n")
            self.f.flush()

    def close(self):
        if self.f:
            self.f.close()


def send_command(cmd: str) -> None:
    """Send command to game via control pipe."""
    if not CONTROL_PIPE.exists():
        print("Error: Game pipe not found. Is the game running in test mode?")
        return
    with open(CONTROL_PIPE, "w") as f:
        f.write(cmd + "\n")
        f.flush()


def run_sequence():
    """Run a test sequence using the game's built-in screenshot."""
    print("=== Game Test Sequence ===")
    print()

    if not CONTROL_PIPE.exists():
        print("Error: Game pipe not found. Start game with GAME_TEST_MODE=1")
        return

    # Keep pipe open for all commands
    client = PipeClient(CONTROL_PIPE)
    client.connect()

    # Sequence
    print("[1] Taking menu screenshot...")
    client.send("screenshot menu")
    time.sleep(2)  # Wait for screenshot to be processed
    print("    Done")

    print("[2] Starting game...")
    client.send("start_game")
    time.sleep(3)  # Wait for game to initialize

    print("[3] Taking gameplay screenshot...")
    client.send("screenshot gameplay")
    time.sleep(2)  # Wait for screenshot to be processed
    print("    Done")

    print("[4] Placing a bridge...")
    client.send("place_bridge 4 4")
    time.sleep(2)

    print("[5] Taking screenshot after bridge...")
    client.send("screenshot after_bridge")
    time.sleep(2)  # Wait for screenshot to be processed
    print("    Done")

    client.close()

    print()
    print("=== Sequence complete ===")
    print(f"Screenshots saved to: {SCREENSHOTS_DIR}")

    for f in sorted(SCREENSHOTS_DIR.glob("*.png")):
        size_kb = f.stat().st_size // 1024
        print(f"  {f.name} ({size_kb} KB)")


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "run":
        run_sequence()
    else:
        print("Usage: python3 test_client.py run")
