#!/usr/bin/env python3
"""Game test controller using internal FIFO pipe.

Uses the game's built-in test control pipe (.game_control_pipe) for commands.
This is the preferred approach - no OS-level input simulation needed.
"""

from __future__ import annotations

import os
import sys
import time
import subprocess
from pathlib import Path
from typing import Optional
import argparse

PROJECT_ROOT = Path("/Users/rexarrior/programming/my/netshtorm_like_game")
GAME_EXE = PROJECT_ROOT / "build" / "netstorm_like"
SCREENSHOTS_DIR = PROJECT_ROOT / "test_screenshots"
PIPE_PATH = PROJECT_ROOT / ".game_control_pipe"

# Ensure screenshots directory exists
SCREENSHOTS_DIR.mkdir(exist_ok=True)


class GameController:
    """Controls the game via internal FIFO pipe commands."""

    def __init__(self, game_exe: Path, screenshots_dir: Path):
        self.game_exe = game_exe
        self.screenshots_dir = screenshots_dir
        self.process: Optional[subprocess.Popen] = None
        self.pipe = None

    def start(self) -> None:
        """Start the game in test mode."""
        env = os.environ.copy()
        env["GAME_TEST_MODE"] = "1"
        self.process = subprocess.Popen(
            [str(self.game_exe)],
            cwd=str(self.game_exe.parent.parent),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            env=env,
        )
        # Wait for pipe to be created
        time.sleep(2)
        # Open pipe for writing
        if PIPE_PATH.exists():
            self.pipe = open(PIPE_PATH, "w")
            print(f"Connected to game pipe: {PIPE_PATH}")
        else:
            print(f"Warning: Pipe not created: {PIPE_PATH}")

    def send_command(self, cmd: str) -> None:
        """Send a command to the game via FIFO pipe."""
        if self.pipe:
            self.pipe.write(cmd + "\n")
            self.pipe.flush()
            print(f"Sent: {cmd}")

    def wait(self, seconds: float) -> None:
        """Wait for specified seconds."""
        time.sleep(seconds)

    def screenshot(self, name: str) -> None:
        """Take a screenshot via game command."""
        self.send_command(f"screenshot {name}")
        self.wait(0.5)

    def start_game(self) -> None:
        """Start game from menu."""
        self.send_command("start_game")
        self.wait(1)

    def pan(self, dx: int, dy: int) -> None:
        """Pan camera by (dx, dy) pixels."""
        self.send_command(f"pan {dx} {dy}")
        self.wait(0.3)

    def zoom(self, factor: float) -> None:
        """Zoom camera by factor (e.g., 1.2 = zoom in 20%)."""
        self.send_command(f"zoom {factor}")
        self.wait(0.3)

    def click(self, x: int, y: int) -> None:
        """Click at screen position."""
        # For now, just log - actual click simulation would need game integration
        print(f"Click at ({x}, {y})")

    def quit(self) -> None:
        """Quit the game."""
        self.send_command("quit")
        self.wait(0.5)

    def stop(self) -> None:
        """Stop the game."""
        if self.pipe:
            self.pipe.close()
            self.pipe = None
        if self.process:
            self.process.terminate()
            try:
                self.process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                self.process.kill()
            self.process = None

    def __enter__(self):
        self.start()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.stop()


def run_test_sequence():
    """Run a basic test sequence."""
    print("Starting game test...")
    print(f"Game: {GAME_EXE}")
    print(f"Screenshots: {SCREENSHOTS_DIR}")
    print()

    with GameController(GAME_EXE, SCREENSHOTS_DIR) as controller:
        controller.start_game()
        controller.screenshot("test_basic")
        print("Test complete!")


def run_camera_test():
    """Test camera controls: pan and zoom via internal pipe."""
    print("Starting camera control test...")
    print(f"Game: {GAME_EXE}")
    print(f"Screenshots: {SCREENSHOTS_DIR}")
    print()

    with GameController(GAME_EXE, SCREENSHOTS_DIR) as controller:
        print("[1/6] Starting game...")
        controller.start_game()
        controller.screenshot("cam_01_home")

        print("[2/6] Panning camera right by 100 pixels...")
        controller.pan(100, 0)
        controller.screenshot("cam_02_pan_right")

        print("[3/6] Panning down by 50 pixels...")
        controller.pan(0, 50)
        controller.screenshot("cam_03_pan_down")

        print("[4/6] Zooming in by 20%...")
        controller.zoom(1.2)
        controller.screenshot("cam_04_zoom_in")

        print("[5/6] Zooming out by 30%...")
        controller.zoom(0.7)
        controller.screenshot("cam_05_zoom_out")

        print("[6/6] Panning back left and up...")
        controller.pan(-100, -50)
        controller.screenshot("cam_06_back")

        print()
        print("Test complete!")
        print(f"Screenshots saved to: {SCREENSHOTS_DIR}")

        print()
        print("Generated screenshots:")
        for f in sorted(SCREENSHOTS_DIR.glob("cam_*.png")):
            print(f"  - {f.name}")


def analyze_screenshots():
    """Analyze generated screenshots."""
    print("\nAnalyzing screenshots...")

    screenshots = sorted(SCREENSHOTS_DIR.glob("*.png"))
    if not screenshots:
        print("No screenshots found!")
        return

    for i, screenshot in enumerate(screenshots[:5], 1):
        print(f"\n[{i}] {screenshot.name}:")
        print(f"    Size: {screenshot.stat().st_size} bytes")

        try:
            from PIL import Image

            img = Image.open(screenshot)
            print(f"    Dimensions: {img.size}")
            print(f"    Mode: {img.mode}")
        except Exception as e:
            print(f"    Could not analyze: {e}")

    print("\nTo analyze with MiniMax, use the understand_image MCP tool.")


def main():
    parser = argparse.ArgumentParser(
        description="Game test controller via internal pipe"
    )
    parser.add_argument(
        "action",
        choices=["test", "camera", "analyze", "clean"],
        help="Action: test (basic), camera (camera controls), analyze (screenshots), clean (delete)",
    )

    args = parser.parse_args()

    if args.action == "test":
        run_test_sequence()
    elif args.action == "camera":
        run_camera_test()
    elif args.action == "analyze":
        analyze_screenshots()
    elif args.action == "clean":
        print(f"Cleaning screenshots in {SCREENSHOTS_DIR}...")
        for f in SCREENSHOTS_DIR.glob("*.png"):
            f.unlink()
            print(f"  Deleted: {f.name}")
        print("Done")
    elif args.action == "clean":
        print(f"Cleaning screenshots in {SCREENSHOTS_DIR}...")
        for f in SCREENSHOTS_DIR.glob("*.png"):
            f.unlink()
            print(f"  Deleted: {f.name}")
        print("Done")


if __name__ == "__main__":
    main()
