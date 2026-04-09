# Testing Guide

## Test Controller

The game supports test mode via a named pipe (FIFO) for automated testing and screenshot capture.

### Building with Test Support

```bash
# With screenshot capture enabled (default)
cmake -B build && cmake --build build

# Without screenshot capture (better performance)
cmake -B build -DENABLE_SCREENSHOT_CAPTURE=OFF && cmake --build build
```

### Running in Test Mode

```bash
GAME_TEST_MODE=1 ./build/netstorm_like
```

A control pipe `.game_control_pipe` will be created in the working directory.

### Available Commands

| Command | Description | Example |
|---------|-------------|---------|
| `start_game` | Transition from menu to gameplay | `start_game` |
| `screenshot <name>` | Take an offscreen screenshot | `screenshot after_bridge` |
| `click <x> <y>` | Simulate mouse click (reserved) | `click 640 360` |
| `key <keyname>` | Simulate key press (reserved) | `key SPACE` |
| `place_bridge <gx> <gy>` | Place bridge at grid position (reserved) | `place_bridge 4 4` |
| `quit` | Request game quit | `quit` |

### Python Test Client

```python
import time
pipe = open('.game_control_pipe', 'w')

# Start game from menu
pipe.write('start_game\n')
pipe.flush()
time.sleep(2)

# Take screenshot
pipe.write('screenshot gameplay\n')
pipe.flush()
time.sleep(2)

pipe.close()
```

Screenshots are saved to `test_screenshots/` directory as PNG files.

### How It Works

1. `TestController` runs in the same process as the game
2. Commands are read from the FIFO pipe each frame via `process_commands()`
3. Screenshot commands set a `pending_screenshot_name_` flag
4. After `EndDrawing()`, `render()` checks for pending screenshots
5. If enabled, `take_offscreen_screenshot()` renders to `RenderTexture2D` and exports

### Offscreen Rendering

Screenshots use `RenderTexture2D` (offscreen FBO) to capture the framebuffer. This is required on macOS because:

- `TakeScreenshot()` reads from the front buffer which may be empty on background/unfocused windows
- `LoadImageFromScreen()` has the same limitation
- Offscreen rendering bypasses macOS window compositor and captures the actual rendered content

The offscreen texture is created once and reused. Set `ENABLE_SCREENSHOT_CAPTURE=OFF` to disable this entirely and improve FPS.

### Current Known Issues

- Screenshots may appear flipped/mirrored when using `RenderTexture2D` on some configurations
- FPS may drop when `ENABLE_SCREENSHOT_CAPTURE` is ON (due to `LoadImageFromTexture` readback)
