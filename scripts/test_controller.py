#!/usr/bin/env python3
"""Game test controller with screenshot capture.

Allows automated testing of the game by:
1. Running the game in test mode
2. Sending commands via a control pipe
3. Capturing screenshots after each action
4. Analyzing results
"""

from __future__ import annotations

import os
import sys
import time
import subprocess
import struct
from pathlib import Path
from typing import Optional, List, Tuple
from PIL import Image
import argparse

PROJECT_ROOT = Path("/Users/rexarrior/programming/my/netshtorm_like_game")
GAME_EXE = PROJECT_ROOT / "build" / "netstorm_like"
SCREENSHOTS_DIR = PROJECT_ROOT / "test_screenshots"
CONTROL_PIPE = PROJECT_ROOT / ".game_control_pipe"

# Ensure screenshots directory exists
SCREENSHOTS_DIR.mkdir(exist_ok=True)


class ScreenshotCapture:
    """Captures screenshots using macOS screencapture utility."""

    @staticmethod
    def capture(filename: str, display: int = 0) -> Path:
        """Capture screen and save to file.

        Args:
            filename: Name of the screenshot file (without path)
            display: Display number (0 = main display)

        Returns:
            Path to the captured screenshot
        """
        filepath = SCREENSHOTS_DIR / filename

        # Use macOS screencapture utility
        cmd = [
            "screencapture",
            "-x",  # no sound
            "-m",  # main display only
            str(filepath),
        ]

        subprocess.run(cmd, check=True, capture_output=True)
        return filepath


class GameController:
    """Controls the game via a control pipe for automated testing."""

    def __init__(self, game_exe: Path, control_pipe: Path):
        self.game_exe = game_exe
        self.control_pipe = control_pipe
        self.process: Optional[subprocess.Popen] = None
        self.screenshot_counter = 0

    def start(self) -> None:
        """Start the game in test mode."""
        # Remove existing pipe
        if self.control_pipe.exists():
            self.control_pipe.unlink()

        # Create FIFO pipe
        os.mkfifo(self.control_pipe)

        # Start game with test mode flag
        env = os.environ.copy()
        env["GAME_TEST_MODE"] = "1"

        self.process = subprocess.Popen(
            [str(self.game_exe)],
            env=env,
            cwd=str(self.game_exe.parent.parent),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        # Give game time to start
        time.sleep(1)

    def send_command(self, command: str) -> None:
        """Send a command to the game via control pipe.

        Commands:
            click x y - Click at screen coordinates
            key k - Press key (w/a/s/d for movement, etc.)
            wait n - Wait n seconds
            screenshot name - Take screenshot with given name
        """
        if not self.control_pipe.exists():
            raise RuntimeError("Control pipe not created")

        with open(self.control_pipe, "w") as f:
            f.write(command + "\n")
            f.flush()

    def click(self, x: int, y: int) -> Path:
        """Click at coordinates and take screenshot."""
        self.send_command(f"click {x} {y}")
        time.sleep(0.1)  # Small delay for game to process
        return self.screenshot()

    def key(self, key: str) -> Path:
        """Press key and take screenshot."""
        self.send_command(f"key {key}")
        time.sleep(0.1)
        return self.screenshot()

    def wait(self, seconds: float) -> None:
        """Wait for specified seconds."""
        time.sleep(seconds)

    def screenshot(self, name: Optional[str] = None) -> Path:
        """Take a screenshot."""
        if name is None:
            self.screenshot_counter += 1
            name = f"screenshot_{self.screenshot_counter:03d}"

        filepath = ScreenshotCapture.capture(name)
        return filepath

    def stop(self) -> None:
        """Stop the game."""
        if self.process:
            self.process.terminate()
            try:
                self.process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                self.process.kill()
            self.process = None

        # Clean up pipe
        if self.control_pipe.exists():
            self.control_pipe.unlink()

    def __enter__(self):
        self.start()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.stop()


def run_manual_test_sequence():
    """Run a manual test sequence for demonstration."""
    print("Starting game test controller...")
    print(f"Game: {GAME_EXE}")
    print(f"Screenshots: {SCREENSHOTS_DIR}")
    print()

    with GameController(GAME_EXE, CONTROL_PIPE) as controller:
        # Test sequence
        print("Taking initial screenshot...")
        controller.screenshot("01_initial")

        print("Clicking center of screen...")
        controller.click(640, 360)
        controller.screenshot("02_after_click")

        print("Waiting 2 seconds...")
        controller.wait(2)
        controller.screenshot("03_after_wait")

        print()
        print("Test sequence complete!")
        print(f"Screenshots saved to: {SCREENSHOTS_DIR}")

        # List screenshots
        for f in sorted(SCREENSHOTS_DIR.glob("*.png")):
            print(f"  - {f.name}")


def run_automated_test():
    """Run automated test analyzing game state."""
    print("Running automated game test...")

    with GameController(GAME_EXE, CONTROL_PIPE) as controller:
        # Initial state
        controller.screenshot("game_initial")

        # Try some interactions
        for i in range(5):
            controller.click(400 + i * 50, 300)
            controller.screenshot(f"game_step_{i + 1}")
            controller.wait(0.5)

        print("Automated test complete")


def main():
    parser = argparse.ArgumentParser(description="Game test controller")
    parser.add_argument(
        "mode",
        choices=["manual", "auto"],
        help="Test mode: manual (interactive) or auto (sequence)",
    )

    args = parser.parse_args()

    if args.mode == "manual":
        run_manual_test_sequence()
    else:
        run_automated_test()


if __name__ == "__main__":
    main()
