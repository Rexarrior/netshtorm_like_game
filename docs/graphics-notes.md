# Graphics Development Notes

## Current Status: Known Issues

The game has a **pre-existing rendering bug** where isometric tiles (islands, water) are not visible in the main game viewport. Only the minimap correctly shows island positions.

### Issue: Isometric Positioning Bug

**Symptoms:**
- Islands appear in top-right corner of screen instead of centered
- Water tiles render as solid blue background without island overlay
- Storm cloud (geyser) visible but positioned incorrectly

**Root Cause:**
raylib's `Camera2D` works with standard screen coordinates, not isometric grid coordinates. The isometric transformation formula:
```
screen_x = (gx - gy) * TILE_WIDTH / 2
screen_y = (gx + gy) * TILE_HEIGHT / 2
```
produces world-space coordinates that don't align correctly with raylib's camera transform.

When `camera.target = (0, 128)` for grid position (4,4), raylib interprets this as a world position, but the isometric coordinate system has different semantics.

**Debug Evidence:**
```
cam.target=(0,128) cam.offset=(640,360)
Tile gx=3 gy=3 screen=(0,96) transformed=(640,328)
```
Tiles ARE being rendered at correct screen positions, but the camera offset doesn't center them properly.

### Attempted Fixes

1. **Camera Y-axis Inversion** - Did not fix the issue
2. **Offscreen Rendering with manual offset** - Textures render but positions still wrong
3. **Direct screen coordinate calculation** - Would require removing Camera2D entirely

## Solutions in Place

### 1. Screenshot Capture (macOS workaround)

macOS doesn't allow `TakeScreenshot()` on unfocused windows or windows on other Spaces. Solution: **offscreen rendering via `RenderTexture2D`**.

**Implementation:**
- `RenderTexture2D` created once and reused
- Scene rendered via `BeginTextureMode(rt)` / `EndTextureMode()`
- Frame exported via `LoadImageFromTexture()` + `ExportImage()`

**Code Location:** `src/game.cpp` - `take_offscreen_screenshot()`

**CMake Flag:** `ENABLE_SCREENSHOT_CAPTURE` (default ON, set to OFF for better FPS)

### 2. Test Controller with FIFO Pipe

Commands sent via named pipe `.game_control_pipe`:
- `start_game` - transitions from menu to gameplay
- `screenshot <name>` - captures current frame

**Code Location:** `src/test/test_controller.cpp`

### 3. Asset Generation

Assets generated via MiniMax API in `scripts/generate_assets.py`. Background cleanup via edge-detection in `scripts/cleanup_assets.py`.

## Pending Fixes Needed

### High Priority

1. **Isometric Rendering Pipeline**
   - Option A: Remove `Camera2D` entirely, use manual screen coordinate calculation with fixed screen offset
   - Option B: Use raylib's built-in 2D rendering without isometric transform
   - Option C: Implement isometric view with proper camera offset calculation

2. **Water Texture** 
   - Currently loaded from `assets/tiles/water.png` but NOT rendered in game
   - Need to tile water across visible map area

3. **Bridge Rendering Scale**
   - Bridges rendered at 32x16 instead of full 64x32 tile size

### Medium Priority

4. **Depth Sorting**
   - Objects should be sorted by `gx + gy` for proper isometric overlap
   - Currently islands, bridges, geysers render in separate loops

5. **Geyser Scale**
   - Hardcoded 32x64 instead of using actual texture dimensions

6. **Z-Fighting Prevention**
   - Add small z-offset based on y-position

## Architecture Notes

### Rendering Pipeline (Current)
```
render_scene():
  ClearBackground()
  BeginMode2D(camera_)
    MapRenderer::render()  // Islands, bridges, geysers
    ParticleSystem::render()
  EndMode2D()
  HUD::render()
  Minimap::render()  // Uses direct 2D, NOT Camera2D
```

### Minimap Works Correctly

The minimap uses direct 2D drawing (no `Camera2D`) and correctly positions islands. This proves the grid-to-screen math is correct, but the camera transform is incompatible.

**Key difference:**
- Minimap: `mx = offset_x + (island.center.x - MAP_GRID_SIZE/2) * scale`
- MapRenderer: `transformed = GetWorldToScreen2D(isometric_pos, camera_)`

### Texture Keys

Assets loaded with prefixes:
- Tiles: `tile_<name>` (e.g., `tile_island_grass`, `tile_water`)
- Bridges: `bridge_<shape>_<state>` (e.g., `bridge_straight_cracked`)
- Geysers: `geyser_storm`
- Backgrounds: `bg_<name>` (e.g., `bg_menu`)

### Config Values

From `src/core/config.h`:
- `TILE_WIDTH = 64`
- `TILE_HEIGHT = 32`
- `TILE_DEPTH = 16`
- `MAP_GRID_SIZE = 64`
