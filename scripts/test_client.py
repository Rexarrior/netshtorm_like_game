#!/usr/bin/env python3
"""Simple game test client - sends commands to game via pipe and captures screenshots."""

import os
import sys
import time
import subprocess
from pathlib import Path
from PIL import Image

PROJECT_ROOT = Path("/Users/rexarrior/programming/my/netshtorm_like_game")
GAME_EXE = PROJECT_ROOT / "build" / "netstorm_like"
SCREENSHOTS_DIR = PROJECT_ROOT / "test_screenshots"
CONTROL_PIPE = PROJECT_ROOT / ".game_control_pipe"

SCREENSHOTS_DIR.mkdir(exist_ok=True)

# Game window dimensions (known from config)
GAME_WIDTH = 1280
GAME_HEIGHT = 720


def capture_game_window() -> Image.Image:
    """Capture the game window using CGWindowListCreateImage."""
    try:
        import Quartz as quartz

        # Get list of windows
        window_list = quartz.CGWindowListCopyWindowInfo(
            quartz.kCGWindowListOptionOnScreenOnly
            | quartz.kCGWindowListExcludeDesktopElements,
            quartz.kCGNullWindowID,
        )

        # Find the netstorm_like window
        for window in window_list:
            owner_name = window.get("kCGWindowOwnerName", "")
            window_name = window.get("kCGWindowName", "")
            window_id = window.get("kCGWindowID", 0)

            if "netstorm" in (owner_name + window_name).lower():
                bounds = window.get("kCGWindowBounds", {})
                x = int(bounds.get("X", 0))
                y = int(bounds.get("Y", 0))
                width = int(bounds.get("Width", GAME_WIDTH))
                height = int(bounds.get("Height", GAME_HEIGHT))

                print(
                    f"    Found window: id={window_id}, bounds=({x},{y},{width},{height})",
                    file=sys.stderr,
                )

                # Capture just this window
                img = quartz.CGWindowListCreateImage(
                    ((x, y), (width, height)),
                    quartz.kCGWindowListOptionIncludingWindow,
                    window_id,
                    quartz.kCGWindowImageBoundsIgnoreFraming,
                )

                # Convert CGImage to PIL Image
                return Image.frombuffer(
                    "RGBA",
                    (width, height),
                    img.data_provider.as_data_bytes(),
                    "raw",
                    "RGBA",
                    0,
                    1,
                )
    except Exception as e:
        print(f"    CGWindow capture failed: {e}", file=sys.stderr)

    return None


def take_screenshot(name: str) -> Path:
    """Take screenshot of game window only."""
    filepath = SCREENSHOTS_DIR / f"{name}.png"

    # Try CGWindow capture first
    img = capture_game_window()
    if img:
        img.save(filepath)
        return filepath

    # Fallback: try screencapture with window ID from screencapture -il
    try:
        # List windows
        result = subprocess.run(
            ["screencapture", "-il"], capture_output=True, text=True
        )

        # Parse output looking for netstorm window
        # Format: "id: window_name (x,y) wxh"
        for line in result.stdout.split("\n"):
            if "netstorm" in line.lower():
                # Extract window ID (first number before colon)
                wid = line.split(":")[0].strip()
                if wid.isdigit():
                    print(f"    Using window ID: {wid}", file=sys.stderr)
                    subprocess.run(
                        ["screencapture", "-x", "-l", wid, str(filepath)], check=True
                    )
                    return filepath
    except Exception as e:
        print(f"    screencapture -il failed: {e}", file=sys.stderr)

    # Last resort: full screen, will be big but at least works
    print(f"    Warning: Using full screen capture", file=sys.stderr)
    subprocess.run(["screencapture", "-x", str(filepath)], check=True)
    return filepath


def send_command(cmd: str) -> None:
    """Send command to game via control pipe."""
    if not CONTROL_PIPE.exists():
        print("Error: Game pipe not found. Is the game running in test mode?")
        return
    with open(CONTROL_PIPE, "w") as f:
        f.write(cmd + "\n")
        f.flush()


def run_sequence():
    """Run a test sequence."""
    print("=== Game Test Sequence ===")
    print()

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

    for f in sorted(SCREENSHOTS_DIR.glob("*.png")):
        size_kb = f.stat().st_size // 1024
        print(f"  {f.name} ({size_kb} KB)")


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "run":
        run_sequence()
    else:
        print("Usage: python3 test_client.py run")
