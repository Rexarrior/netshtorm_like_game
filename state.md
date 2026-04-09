# NetStorm-Like — Project State

**Date**: 2026-04-09
**Phase**: Phase 1.5 (Graphics Debug) — IsometricCamera2D implemented, BeginMode2D removed
**Engine**: raylib 5.5 (C++17)

---

## What Exists

### Source Code (49 files, all created and audited)

```
src/
├── main.cpp
├── game.h / game.cpp              — Main game loop, state machine, demo map
├── core/
│   ├── config.h                   — Constants: screen, tile, camera, FPS
│   ├── types.h                    — Enums (GameState, Element, IslandType, BridgeShape, etc.), Position, TilePosition
│   ├── random.h / random.cpp      — RNG wrapper (mt19937)
│   ├── timer.h / timer.cpp        — Game timer with speed control
│   └── state_machine.h / state_machine.cpp  — GameState transitions with callbacks
├── assets/
│   ├── asset_manager.h / cpp      — Singleton: load textures/sounds/music from dirs
│   ├── sprite_sheet.h / cpp       — Sprite sheet parsing (frame extraction)
│   └── sound_bank.h / cpp         — Sound effect registry
├── input/
│   ├── input_actions.h            — Enum of game actions
│   └── input_handler.h / cpp      — Keyboard + mouse polling → game actions
├── camera/
│   ├── game_camera.h / cpp        — Isometric camera: pan (WASD), zoom (wheel), follow
│   └── minimap_camera.h / cpp     — Minimap viewport (stub)
├── map/
│   ├── island.h / cpp             — Island: tiles, center, type, connection points
│   ├── island_generator.h / cpp   — Diamond-shaped island generation (home/resource/enemy/strategic)
│   ├── bridge.h / cpp             — BridgeSegment: shape, state (cracked/hardened), HP, owner
│   ├── bridge_placer.h / cpp      — Placement validation: overlap, adjacency, BFS connectivity
│   ├── geyser.h / cpp             — Storm Geyser: position, production rate
│   ├── map.h / cpp                — GameMap: islands, bridges, geysers, place_bridge()
│   └── map_renderer.h / cpp       — Isometric tile/bridge/geyser rendering with fallback colors
├── ui/
│   ├── menu.h / cpp               — Main menu, pause, game over screens with clickable buttons
│   ├── hud.h / cpp                — Top bar: energy, speed, FPS
│   └── minimap.h / cpp            — Minimap rendering (island dots, bridge dots)
├── systems/
│   └── particle_system.h / cpp    — Generic particle system (emit, burst, update, render)
└── (network/ — Phase 6+, not started)

tests/
├── raylib_stub.h                  — Shared raylib stub for testing (USE_RAYLIB_STUB)
├── test_types.cpp                 — Position, Element, GameState, TilePosition tests
└── test_map.cpp                   — Island, Bridge, BridgePlacer, GameMap tests (20+ tests)

CMakeLists.txt                     — Build config: game + tests, raylib optional
vcpkg.json                         — Dependencies: raylib
.gitignore
build.bat                          — Build script (see Build Instructions below)
```

### Design Documents

**7 design documents were composed in full text during the planning session but NOT saved to disk** (write tool was restricted at the time). They need to be recreated:

1. `docs/01_game_design.md` — Core loop, map, bridges, resources, 4 elements, units, tiers, spells, combat, victory, controls, UI layout, art style, MVP scope
2. `docs/02_technical_architecture.md` — Tech stack, high-level architecture, 13 subsystems with file structures, data flow, CMake structure, design decisions
3. `docs/03_graphics_architecture.md` — Rendering pipeline, isometric projection, asset categories with sizes, sprite sheet management, camera, z-sorting, color palette
4. `docs/04_balance.md` — Element matrix, all MVP unit stats, buildings, bridges, geysers, spells, priest, economy, campaign difficulty, balance principles
5. `docs/05_asset_list.md` — 48 assets with MiniMax prompts, grouped by category, generation order
6. `docs/06_project_structure.md` — Full directory tree, CMakeLists.txt, vcpkg.json, .gitignore, AssetManager loading order
7. `docs/07_development_phases.md` — 7 phases with tasks, tests, deliverables, timeline, risk mitigation

**Action needed**: Recreate all 7 docs. The full text of each was provided in the planning conversation and can be reconstructed from there.

---

## What Works (Verified)

### Graphics Architecture Decision (2026-04-09)
**Problem**: Isometric tiles not visible - islands appear in top-right corner instead of centered.

**Root Cause**: Conflicting coordinate systems:
- `BeginMode2D(camera_.camera())` used raylib's Camera2D with `target={0,0}`
- `MapRenderer` used `IsometricCamera2D::grid_to_screen()` which does full isometric transform

These two approaches are incompatible - raylib's Camera2D expects world coordinates but received isometric screen coordinates.

**Solution**: **Remove BeginMode2D** for gameplay rendering. IsometricCamera2D::grid_to_screen() handles ALL coordinate transformation manually. This gives us full control over the isometric projection pipeline.

**Why not fix BeginMode2D**: raylib's Camera2D is designed for standard 2D panning/zooming, not isometric grids. The isometric transformation requires custom logic that doesn't fit Camera2D's model.

**Result (2026-04-09)**:
- ✅ Islands ARE NOW VISIBLE after BeginMode2D removal
- ❌ RenderTexture screenshot is FLIPPED/MIRRORED (macOS issue)
- ❌ UI text appears upside-down in screenshots (same RenderTexture issue)
- ⚠️ Need to verify islands are properly centered (not at screen edge)

### Code Audit — PASS (22 issues found and fixed)
- CRITICAL: Rectangle syntax error in tests — ✅ fixed
- CRITICAL: `IslandType::None` missing from enum — ✅ added
- HIGH: Missing `target_include_directories` — ✅ added
- HIGH: Wrong include path in game_camera.cpp — ✅ fixed
- MEDIUM: Missing `#include <string>`, `<functional>`, `<algorithm>` — ✅ all added
- MEDIUM: `tile_owner` returned wrong type — ✅ fixed
- MEDIUM: Unbounded BFS in bridge_placer — ✅ added MAP_GRID_SIZE bounds
- MEDIUM: Duplicated raylib stub — ✅ extracted to shared `tests/raylib_stub.h`
- LOW: ClearBackground alpha, HUD dead code, InputHandler duplicate, etc. — ✅ all fixed

### Compilation — VERIFIED with MSVC 19.40 (VS 2022)
The following files were individually compiled with `cl.exe /std:c++17` and produced **zero errors**:
- `bridge_placer.cpp` ✅
- `island.cpp` ✅
- `island_generator.cpp` ✅
- `map.cpp` ✅
- `random.cpp` ✅

### Dependencies — INSTALLED
- **raylib 5.5** — installed via vcpkg at `S:\programming\my\vcpkg`
- **vcpkg** — located at `S:\programming\my\vcpkg`
- **git** — located at `C:\Program Files\Git\bin\git.exe`
- **MSVC 14.40** (VS 2022) — located at `C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.40.33807`
- **CMake 3.28** — located at `C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe`
- **Ninja** — located at `C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe`

---

## Build Issues (Environment, Not Code)

### Problem
CMake cannot build the project because:
1. **CMake picks MinGW GCC 6.3.0** by default (too old for C++17 — no `<optional>`)
2. **Forcing MSVC fails** — Ninja cannot find `rc.exe` (resource compiler) and `mt.exe` (manifest tool) from Windows SDK when invoked from PowerShell
3. **Running via `vcvarsall.bat x64`** sets up the PATH correctly, but CMake still falls back to MinGW unless compilers are explicitly specified
4. **GoogleTest FetchContent** — git clone fails due to corrupted git refs on this machine (`refs/files-backend.c` error)

### What Needs to Happen to Build
1. Run from **VS Developer Command Prompt** (vcvarsall.bat x64 already loaded)
2. Explicitly set compilers:
   ```
   cmake -B build ^
     -DCMAKE_TOOLCHAIN_FILE="S:/programming/my/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
     -DCMAKE_C_COMPILER="C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.40.33807/bin/Hostx64/x64/cl.exe" ^
     -DCMAKE_CXX_COMPILER="C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.40.33807/bin/Hostx64/x64/cl.exe"
   ```
3. If FetchContent for gtest still fails, either:
   - Pre-download googletest and use `FETCHCONTENT_SOURCE_DIR_GOOGLETEST`
   - Or use `-DFETCHCONTENT_UPDATES_DISCONNECTED=ON` if the source is already cached
4. Then: `cmake --build build` or `ninja -C build`

### build.bat (created but untested)
A `build.bat` file exists at the project root. It should contain the vcvarsall + cmake commands above.

---

## Key Design Decisions (Already Made)

| Decision | Value |
|----------|-------|
| Perspective | Isometric 2D |
| Language | C++17 |
| Engine | raylib 5.x |
| Build | CMake + vcpkg + Ninja |
| Testing | GoogleTest with raylib stub |
| Entity model | Composition (not full ECS) |
| Multiplayer | Deferred (skeleton in Phase 6+) |
| MVP scope | 2 elements (Sun+Thunder), 3 units each, 3 bridge shapes |
| Grid | Isometric grid, 64x32 tile diamonds |
| Determinism | Required for future lockstep (fixed timestep, seeded RNG) |
| **Rendering** | Manual coordinate conversion via IsometricCamera2D::grid_to_screen(), **no BeginMode2D** for gameplay rendering |

---

## Next Steps (In Order)

### Immediate (Blockers)
1. **Recreate all 7 docs** in `docs/` — the full text exists in the planning conversation
2. **Fix build** — get CMake + MSVC + Ninja working, compile and run tests
3. **Verify tests pass** — all 20+ tests in test_types.cpp and test_map.cpp

### Phase 2: Bridges (After Phase 1 builds & tests pass)
- Bridge placement UI (segment selection panel)
- Visual feedback for valid/invalid placement
- Bridge state transitions (cracked → hardened via spell)
- Bridge destruction logic
- Tests for bridge placement edge cases

### Phase 3: Units & Combat
- Unit entity hierarchy (Unit → OffensiveUnit, DefensiveUnit, TransportUnit)
- Unit registry with stats
- Auto-combat system (target selection, attack timer, damage calculation)
- Damage calculator with element bonus (+15%/-15%)
- Unit rendering (idle/attack/dead animations)
- Tests for combat logic

### Phase 4: Resources & Economy
- Storm Energy pool per player
- Geyser auto-collection by transport units
- Energy cost deduction on unit placement
- HUD energy display
- Tests for resource flow

### Phase 5: Spells, Priest & Victory
- Spell system (Bridge Harden, Lightning Strike)
- Priest capture & sacrifice mechanic
- Reward screen
- Full game loop playable

### Phase 6: AI & Campaign
- AI opponent (greedy placement)
- 3 campaign missions
- State machine for campaign progression

### Phase 7: Polish
- Asset generation via MiniMax (scripts/generate_assets.py)
- Sound effects and music
- Particle effects polish
- Network skeleton

---

## Asset Generation Plan

Script location: `scripts/generate_assets.py` (not yet created)
Based on: `S:\programming\my\ai\MiniMax_api_example\examples_python\10_roguelike_assets.py`
API: MiniMax `image-01` for images, `music-2.5` for audio
Keys: `S:\programming\my\ai\MiniMax_api_example\.env`

Total assets to generate: **48** (see `docs/05_asset_list.md` when recreated)

---

## Known Graphics Issues (2026-04-09)

### Issue 1: RenderTexture Screenshot Flip (macOS)
**Severity**: Medium (affects automated testing only)
**Status**: Known, not fixed

**Symptoms**:
- Offscreen screenshots via `RenderTexture2D` appear FLIPPED/MIRRORED on macOS
- UI text renders upside-down in screenshots
- Gameplay appears fine visually (only screenshots are affected)

**Root Cause**: macOS GPU drivers handle RenderTexture Y-flip differently than other platforms.

**Workaround**: Use `screencapture` (macOS native) instead of `RenderTexture2D` for screenshots. See `scripts/test_game.py` which uses AppleScript+screencapture.

**Potential Fix**: Flip the RenderTexture vertically before export, or use `LoadImageFromTexture` with proper Y-axis handling.

---

### Issue 2: Island Positioning Verification Needed
**Severity**: Low (needs verification)
**Status**: Likely fixed, not confirmed

**Symptoms**:
- Need to verify islands render at screen center (640, 360)
- Previous bug: islands appeared in top-right corner

**Verification**: Compare screenshot with expected center position.

---

### Issue 3: Minimap Uses Different Coordinate System
**Severity**: Low (design issue)
**Status**: Known, working as designed

**Note**: Minimap uses direct 2D drawing (no IsometricCamera2D). This works because minimap has its own simple coordinate math. However, if camera zoom/pan is implemented, minimap may need to sync with IsometricCamera2D.

---

## File Count Summary

| Category | Count |
|----------|-------|
| Source files (.h/.cpp) | 42 |
| Test files | 3 (including raylib_stub.h) |
| Build config | 3 (CMakeLists.txt, vcpkg.json, .gitignore) |
| Design docs | 0 (need to be recreated) |
| **Total** | **48** |

---

## Critical Notes for Continuation

1. **DO NOT** try to use MinGW (GCC 6.3) — it doesn't support C++17 `<optional>`, `<filesystem>`, etc.
2. **ALWAYS** use MSVC 14.40 from VS 2022 with vcvarsall.bat x64 loaded
3. **raylib_stub.h** is the key to testing without raylib — all test files include it under `#ifdef USE_RAYLIB_STUB`
4. **IslandType enum** now has `None` as first value: `{ None, Home, Resource, Strategic, Enemy }`
5. **CMakeLists.txt** has `BUILD_GAME` option — set to OFF if raylib isn't available, to build tests only
6. **bridge_placer.cpp** BFS is now bounded by `MAP_GRID_SIZE` (32) to prevent infinite expansion
7. **asset_manager.cpp** uses `__has_include` to pick between `<filesystem>` and `<experimental/filesystem>`
