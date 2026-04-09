#!/usr/bin/env python3
"""Simple game test client - sends commands to game via pipe and captures screenshots."""

import os
import sys
import time
import subprocess
from pathlib import Path

PROJECT_ROOT = Path("/Users/rexarrior/programming/my/netshtorm_like_game")
GAME_EXE = PROJECT_ROOT / "build" / "netstorm_like"
SCREENSHOTS_DIR = PROJECT_ROOT / "test_screenshots"
CONTROL_PIPE = PROJECT_ROOT / ".game_control_pipe"

SCREENSHOTS_DIR.mkdir(exist_ok=True)


def send_command(cmd: str) -> None:
    """Send command to game via control pipe."""
    if not CONTROL_PIPE.exists():
        print("Error: Game pipe not found. Is the game running in test mode?")
        return
    with open(CONTROL_PIPE, "w") as f:
        f.write(cmd + "\n")
        f.flush()


def take_screenshot(name: str) -> Path:
    """Take screenshot using macOS screencapture."""
    filepath = SCREENSHOTS_DIR / f"{name}.png"
    subprocess.run(["screencapture", "-x", str(filepath)], check=True)
    return filepath


def run_sequence():
    """Run a test sequence."""
    print("=== Game Test Sequence ===")
    print()

    # Check if game is running
    if not CONTROL_PIPE.exists():
        print("Error: Game pipe not found. Start game with GAME_TEST_MODE=1")
        return

    # Sequence
    print("[1] Starting game...")
    send_command("start_game")
    time.sleep(1)
    take_screenshot("01_started")
    print("    Screenshot: 01_started.png")

    print("[2] Waiting for map to render...")
    time.sleep(2)
    take_screenshot("02_map_visible")
    print("    Screenshot: 02_map_visible.png")

    print("[3] Placing a bridge...")
    # Place at grid position (4, 4) - near home island
    send_command("place_bridge 4 4")
    time.sleep(0.5)
    take_screenshot("03_bridge_placed")
    print("    Screenshot: 03_bridge_placed.png")

    print("[4] Waiting and checking energy system...")
    time.sleep(2)
    take_screenshot("04_energy_check")
    print("    Screenshot: 04_energy_check.png")

    print()
    print("=== Sequence complete ===")
    print(f"Screenshots saved to: {SCREENSHOTS_DIR}")

    # List screenshots
    for f in sorted(SCREENSHOTS_DIR.glob("*.png")):
        size_kb = f.stat().st_size // 1024
        print(f"  {f.name} ({size_kb} KB)")


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "run":
        run_sequence()
    else:
        print("Usage:")
        print("  python3 test_client.py run  - Run test sequence")
        print("  python3 test_client.py cmd '<command>' - Send single command")
        print()
        print("Commands: start_game, place_bridge GX GY, screenshot NAME, key KEY")
