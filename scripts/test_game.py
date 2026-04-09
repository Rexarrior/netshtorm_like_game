#!/usr/bin/env python3
"""Game test controller using OS-level input simulation.

Uses AppleScript for mouse/keyboard simulation and screencapture for screenshots.
This approach doesn't require game modifications - it simulates real user input.
"""

from __future__ import annotations

import os
import sys
import time
import subprocess
from pathlib import Path
from typing import Optional, List, Tuple
import argparse

PROJECT_ROOT = Path("/Users/rexarrior/programming/my/netshtorm_like_game")
GAME_EXE = PROJECT_ROOT / "build" / "netstorm_like"
SCREENSHOTS_DIR = PROJECT_ROOT / "test_screenshots"

# Ensure screenshots directory exists
SCREENSHOTS_DIR.mkdir(exist_ok=True)


def run_osascript(script: str) -> str:
    """Run AppleScript and return output."""
    result = subprocess.run(
        ["osascript", "-e", script], capture_output=True, text=True, check=True
    )
    return result.stdout.strip()


def get_window_bounds(
    window_name: str = "NetStorm-Like",
) -> Optional[Tuple[int, int, int, int]]:
    """Get window bounds (x, y, width, height) by window name."""
    script = f"""
tell application "System Events"
    set win to first window of (first process whose name contains "netstorm_like")
    if exists win then
        get position of win
    else
        return ""
    end if
end tell
"""
    # This is a simplified version - in reality we'd need more robust window detection
    return None


def get_screen_size() -> Tuple[int, int]:
    """Get main screen size."""
    script = "get screen size of (info for (path to front most application))"
    # Alternative using system_profiler
    result = subprocess.run(
        ["system_profiler", "SPDisplaysDataType", "-json"],
        capture_output=True,
        text=True,
    )
    import json

    data = json.loads(result.stdout)
    # Simplified - just return the main display size
    return 1280, 720  # Game's screen size


class ScreenshotCapture:
    """Captures screenshots using macOS screencapture utility."""

    @staticmethod
    def capture(filename: str, window_only: bool = True) -> Path:
        """Capture screen and save to file.

        Args:
            filename: Name of the screenshot file (without path)
            window_only: If True, capture window only

        Returns:
            Path to the captured screenshot
        """
        filepath = SCREENSHOTS_DIR / filename

        if window_only:
            # Capture with window selection
            # -W: capture window
            # -s: allow selecting window
            # For automated mode, we'll use -x (no interaction) and -m (main display)
            cmd = ["screencapture", "-x", "-m", str(filepath)]
        else:
            cmd = ["screencapture", "-x", str(filepath)]

        subprocess.run(cmd, check=True, capture_output=True)
        return filepath


class AppleScriptInput:
    """Simulates mouse and keyboard input using AppleScript."""

    @staticmethod
    def click(x: int, y: int) -> None:
        """Click at screen coordinates."""
        script = f"""
tell application "System Events"
    click at {{ {x}, {y} }}
end tell
"""
        run_osascript(script)

    @staticmethod
    def double_click(x: int, y: int) -> None:
        """Double click at screen coordinates."""
        script = f"""
tell application "System Events"
    double click at {{ {x}, {y} }}
end tell
"""
        run_osascript(script)

    @staticmethod
    def type_text(text: str) -> None:
        """Type text."""
        script = f'''
tell application "System Events"
    keystroke "{text}"
end tell
'''
        run_osascript(script)

    @staticmethod
    def press_key(key: str) -> None:
        """Press a key (e.g., 'return', 'escape', 'a', etc.)."""
        script = f'''
tell application "System Events"
    keystroke "{key}"
end tell
'''
        run_osascript(script)


class GameController:
    """Controls the game via OS-level input simulation."""

    def __init__(self, game_exe: Path, screenshots_dir: Path):
        self.game_exe = game_exe
        self.screenshots_dir = screenshots_dir
        self.process: Optional[subprocess.Popen] = None
        self.screenshot_counter = 0
        self.input = AppleScriptInput()

    def start(self) -> None:
        """Start the game."""
        self.process = subprocess.Popen(
            [str(self.game_exe)],
            cwd=str(self.game_exe.parent.parent),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        # Give game time to start and become frontmost
        time.sleep(2)

    def click(self, x: int, y: int) -> Path:
        """Click at coordinates and take screenshot."""
        self.input.click(x, y)
        time.sleep(0.2)  # Small delay for game to process
        return self.screenshot()

    def double_click(self, x: int, y: int) -> Path:
        """Double click at coordinates."""
        self.input.double_click(x, y)
        time.sleep(0.2)
        return self.screenshot()

    def type_text(self, text: str) -> None:
        """Type text."""
        self.input.type_text(text)

    def press_key(self, key: str) -> Path:
        """Press key and take screenshot."""
        self.input.press_key(key)
        time.sleep(0.2)
        return self.screenshot()

    def wait(self, seconds: float) -> None:
        """Wait for specified seconds."""
        time.sleep(seconds)

    def screenshot(self, name: Optional[str] = None) -> Path:
        """Take a screenshot."""
        if name is None:
            self.screenshot_counter += 1
            name = f"screenshot_{self.screenshot_counter:03d}"
        else:
            name = f"{name}"

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

    def __enter__(self):
        self.start()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.stop()


def run_test_sequence():
    """Run a test sequence to verify game functionality."""
    print("Starting game test controller...")
    print(f"Game: {GAME_EXE}")
    print(f"Screenshots: {SCREENSHOTS_DIR}")
    print()

    with GameController(GAME_EXE, SCREENSHOTS_DIR) as controller:
        # Test sequence
        print("[1/5] Taking initial screenshot...")
        controller.screenshot("01_initial")

        print("[2/5] Pressing Enter to start game...")
        controller.press_key("return")
        controller.wait(1)
        controller.screenshot("02_after_enter")

        print("[3/5] Clicking to place bridge...")
        # Game starts in Playing state after Enter, click somewhere on map
        controller.click(640, 400)
        controller.wait(0.5)
        controller.screenshot("03_after_click")

        print("[4/5] Pressing Escape to pause...")
        controller.press_key("escape")
        controller.wait(0.5)
        controller.screenshot("04_paused")

        print("[5/5] Pressing Escape again to resume...")
        controller.press_key("escape")
        controller.wait(0.5)
        controller.screenshot("05_resumed")

        print()
        print("Test sequence complete!")
        print(f"Screenshots saved to: {SCREENSHOTS_DIR}")

        # List screenshots
        print()
        print("Generated screenshots:")
        for f in sorted(SCREENSHOTS_DIR.glob("*.png")):
            print(f"  - {f.name}")


def analyze_screenshots():
    """Analyze generated screenshots (placeholder for MiniMax integration)."""
    print("\nAnalyzing screenshots...")

    screenshots = sorted(SCREENSHOTS_DIR.glob("*.png"))
    if not screenshots:
        print("No screenshots found!")
        return

    for i, screenshot in enumerate(screenshots[:3], 1):  # Analyze first 3
        print(f"\n[{i}] {screenshot.name}:")
        print(f"    Size: {screenshot.stat().st_size} bytes")

        # Try to analyze with PIL
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
        description="Game test controller with screenshots"
    )
    parser.add_argument(
        "action",
        choices=["test", "analyze", "clean"],
        help="Action: test (run sequence), analyze (show screenshot info), clean (delete screenshots)",
    )

    args = parser.parse_args()

    if args.action == "test":
        run_test_sequence()
    elif args.action == "analyze":
        analyze_screenshots()
    elif args.action == "clean":
        print(f"Cleaning screenshots in {SCREENSHOTS_DIR}...")
        for f in SCREENSHOTS_DIR.glob("*.png"):
            f.unlink()
            print(f"  Deleted: {f.name}")
        print("Done")


if __name__ == "__main__":
    main()
