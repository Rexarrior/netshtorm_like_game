#!/usr/bin/env python3
"""Generate pixel art assets for NetStorm-like game using MiniMax image-01 API."""

from __future__ import annotations

import json
import os
import sys
import time
import shutil
from pathlib import Path

# Add minimax_explore path for minimax_http module
MINIMAX_EXPLORE = Path("/Users/rexarrior/programming/my/minimax_explore")
sys.path.insert(0, str(MINIMAX_EXPLORE / "examples_python"))

import minimax_http as mh

# Project paths
PROJECT_ROOT = Path("/Users/rexarrior/programming/my/netshtorm_like_game")
ASSETS_DIR = PROJECT_ROOT / "assets"
OUTPUT_DIR = MINIMAX_EXPLORE / "out" / "netstorm_assets"
OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

# Art style constants
PIXEL_ART_STYLE = (
    "16-bit SNES style pixel art, vibrant colors, clear game-ready sprites"
)
ISOMETRIC_STYLE = f"{PIXEL_ART_STYLE}, isometric 2.5D perspective, top-down view"
ICON_STYLE = f"{PIXEL_ART_STYLE}, icon-sized, clear outline, isolated on transparency"

# Aspect ratio for isometric tiles (1:1 works well for diamond tiles)
ASPECT_RATIO = "1:1"

ASSETS = [
    # === TILES (island tops) ===
    (
        "tile_island_grass",
        (
            f"{ISOMETRIC_STYLE} game tile of a grassy island, diamond/rhombus isometric shape viewed from above, "
            "lush green grass texture with small details, darker green edges for depth, "
            "3D appearance showing grass top with slight depth/thickness on sides, "
            "seamless tileable edges, bright cheerful game art style."
        ),
    ),
    (
        "tile_island_stone",
        (
            f"{ISOMETRIC_STYLE} game tile of a rocky stone island, diamond/rhombus isometric shape viewed from above, "
            "grey stone surface with cracks and moss patches, darker grey edges for depth, "
            "3D appearance showing stone top with thickness on sides, "
            "seamless tileable edges, weathered natural rock texture."
        ),
    ),
    # === BRIDGES (straight, L-shape, T-shape) x (hardened, cracked) ===
    (
        "bridge_straight_hardened",
        (
            f"{ISOMETRIC_STYLE} game bridge segment, straight horizontal bridge, hardened reinforced state, "
            "thick stone/concrete structure, metal reinforcements visible, dark grey color with orange-hot seams, "
            "isometric top-down view, 3D appearance with side thickness, "
            "game-ready sprite, clean pixel art."
        ),
    ),
    (
        "bridge_straight_cracked",
        (
            f"{ISOMETRIC_STYLE} game bridge segment, straight horizontal bridge, cracked damaged state, "
            "stone structure with visible cracks and gaps, some pieces missing, lighter grey color, "
            "isometric top-down view, 3D appearance with side thickness, "
            "game-ready sprite, clean pixel art."
        ),
    ),
    (
        "bridge_l_shape_hardened",
        (
            f"{ISOMETRIC_STYLE} game bridge segment, L-shaped corner bridge piece, hardened reinforced state, "
            "thick stone/concrete structure forming 90-degree corner, metal reinforcements, dark grey with orange seams, "
            "isometric top-down view, 3D appearance with side thickness, "
            "game-ready sprite, clean pixel art."
        ),
    ),
    (
        "bridge_l_shape_cracked",
        (
            f"{ISOMETRIC_STYLE} game bridge segment, L-shaped corner bridge piece, cracked damaged state, "
            "stone structure with visible cracks, corner piece with gaps, lighter grey color, "
            "isometric top-down view, 3D appearance with side thickness, "
            "game-ready sprite, clean pixel art."
        ),
    ),
    (
        "bridge_t_shape_hardened",
        (
            f"{ISOMETRIC_STYLE} game bridge segment, T-shaped junction bridge piece, hardened reinforced state, "
            "thick stone/concrete structure forming T-junction, metal reinforcements, dark grey with orange seams, "
            "isometric top-down view, 3D appearance with side thickness, "
            "game-ready sprite, clean pixel art."
        ),
    ),
    (
        "bridge_t_shape_cracked",
        (
            f"{ISOMETRIC_STYLE} game bridge segment, T-shaped junction bridge piece, cracked damaged state, "
            "stone structure with visible cracks at junction, pieces missing, lighter grey color, "
            "isometric top-down view, 3D appearance with side thickness, "
            "game-ready sprite, clean pixel art."
        ),
    ),
    # === GEYSERS ===
    (
        "geyser_storm",
        (
            f"{ISOMETRIC_STYLE} volcanic steam geyser effect sprite, erupting steam cloud with lightning, "
            "dark grey rock base with hole, white steam billowing upward, occasional lightning sparks, "
            "isometric top-down view, animated feel with particle effects suggestion, "
            "game-ready sprite, clean pixel art, dramatic stormy atmosphere."
        ),
    ),
    # === BACKGROUNDS ===
    (
        "bg_menu",
        (
            f"{PIXEL_ART_STYLE} dark volcanic island menu background for strategy game, "
            "dramatic scene with floating islands connected by bridges over water, "
            "active volcano in background with glowing lava, dark stormy sky with lightning, "
            "water surrounding islands with reflections, atmospheric fog layer, "
            "epic moody atmosphere, dark color palette with orange/red lava accents, "
            "leaves empty space in center for menu text and buttons. "
            "No characters, no UI elements. Full scene background."
        ),
    ),
    # === UI ELEMENTS ===
    (
        "ui_button",
        (
            f"{PIXEL_ART_STYLE} game UI button sprite, rounded rectangle stone button, "
            "grey stone texture with carved border, raised 3D appearance with highlight on top edge, "
            "shadow on bottom edge, can have text or icon on top, "
            "game-ready UI element, clean pixel art, isolated on transparency."
        ),
    ),
    (
        "ui_panel",
        (
            f"{PIXEL_ART_STYLE} game UI panel/slate sprite, rectangular stone panel, "
            "grey stone texture with beveled edges, raised frame border, "
            "dark interior surface for displaying text/icons, "
            "game-ready UI element, clean pixel art, isolated on transparency."
        ),
    ),
    # === WATER TILE ===
    (
        "tile_water",
        (
            f"{ISOMETRIC_STYLE} game water tile, top-down view of ocean water, "
            "deep blue color with lighter blue wave patterns, subtle animated ripple suggestion, "
            "seamless tileable edges, dark edges to suggest depth, "
            "game-ready water tile, clean pixel art."
        ),
    ),
    # === UNITS (basic soldier) ===
    (
        "unit_soldier",
        (
            f"{ISOMETRIC_STYLE} game unit sprite, basic infantry soldier, "
            "top-down view, red team color markings, holding a spear/pike weapon, "
            "simple but recognizable humanoid shape, "
            "4 directions: standing idle, walking, attacking, dying poses in 2x2 grid, "
            "each pose in colored panel: blue=idle, green=walk, red=attack, purple=dead, "
            "clean pixel art, game-ready sprite sheet."
        ),
    ),
    # === BUILDINGS ===
    (
        "building_tower",
        (
            f"{ISOMETRIC_STYLE} game building sprite, defensive watch tower on island, "
            "circular stone tower with flag on top, small arrow slits, "
            "isometric top-down view showing tower from above, "
            "3D appearance with walls and roof visible, "
            "game-ready building, clean pixel art."
        ),
    ),
]


def copy_to_project_assets(filename: Path) -> None:
    """Copy generated asset to appropriate project assets directory.

    The filename stem includes a prefix (e.g., 'tile_island_grass'), but the code
    automatically adds prefix when loading, so we strip it here.
    E.g.: 'tile_island_grass' -> 'island_grass' (prefix 'tile' is added by code)

    Also converts JPEG data stored as .png to actual PNG format using sips.
    """
    name = filename.stem  # e.g., "tile_island_grass"

    # Determine subdirectory and prefix strip based on prefix
    if name.startswith("tile_"):
        subdir = "tiles"
        dest_name = name[5:]  # strip "tile_"
    elif name.startswith("bridge_"):
        subdir = "bridges"
        dest_name = name[7:]  # strip "bridge_"
    elif name.startswith("geyser_"):
        subdir = "geysers"
        dest_name = name[7:]  # strip "geyser_"
    elif name.startswith("unit_"):
        subdir = "units"
        dest_name = name[5:]  # strip "unit_"
    elif name.startswith("building_"):
        subdir = "buildings"
        dest_name = name[9:]  # strip "building_"
    elif name.startswith("bg_"):
        subdir = "backgrounds"
        dest_name = name[3:]  # strip "bg_"
    elif name.startswith("ui_"):
        subdir = "ui"
        dest_name = name[3:]  # strip "ui_"
    else:
        subdir = "misc"
        dest_name = name

    dest_dir = ASSETS_DIR / subdir
    dest_dir.mkdir(parents=True, exist_ok=True)

    # Use stripped name for destination (code adds prefix when loading)
    dest = dest_dir / f"{dest_name}.png"

    # Copy file first, then convert if needed
    shutil.copy2(filename, dest)

    # Check if it's actually JPEG data stored as PNG and convert
    import subprocess

    try:
        file_type = subprocess.check_output(
            ["file", "-b", str(dest)], text=True
        ).strip()
        if "JPEG image data" in file_type or "JFIF" in file_type:
            print(f"  Converting JPEG to PNG: {dest}", file=sys.stderr)
            # Convert using sips (macOS built-in)
            subprocess.run(
                ["sips", "-s", "format", "png", str(dest), "--out", str(dest) + ".tmp"],
                check=True,
            )
            Path(str(dest) + ".tmp").rename(dest)
    except Exception as e:
        print(f"  Warning: Could not verify/convert file type: {e}", file=sys.stderr)

    print(f"  Copied to: {dest}")


def generate_all() -> None:
    total = len(ASSETS)
    print(
        f"Starting generation of {total} assets for NetStorm-like game...",
        file=sys.stderr,
    )
    print(f"Output directory: {OUTPUT_DIR}", file=sys.stderr)
    print(f"Project assets dir: {ASSETS_DIR}", file=sys.stderr)
    print("", file=sys.stderr)

    success_count = 0
    fail_count = 0

    for idx, (filename, prompt) in enumerate(ASSETS, start=1):
        print(f"[{idx}/{total}] Generating: {filename}", file=sys.stderr)

        body = {
            "model": "image-01",
            "prompt": prompt,
            "aspect_ratio": ASPECT_RATIO,
            "response_format": "url",
            "n": 1,
            "prompt_optimizer": False,
        }

        try:
            d = mh.api_request("POST", "/v1/image_generation", body)
            mh.require_base_ok(d)
            urls = (d.get("data") or {}).get("image_urls") or []

            if not urls:
                print(
                    f"  WARNING: No image URLs in response for {filename}",
                    file=sys.stderr,
                )
                fail_count += 1
                continue

            for i, u in enumerate(urls):
                dest = OUTPUT_DIR / f"{filename}.png"
                mh.download_url_to_file(str(u), dest)
                print(f"  Saved: {dest}", file=sys.stderr)

                # Copy to project assets directory
                copy_to_project_assets(dest)
                success_count += 1

            if idx < total:
                time.sleep(2)  # Rate limiting

        except Exception as e:
            print(f"  ERROR generating {filename}: {e}", file=sys.stderr)
            fail_count += 1
            continue

    print("", file=sys.stderr)
    print(
        f"Done! Generated {success_count} assets, {fail_count} failed.", file=sys.stderr
    )
    print(f"Assets saved to: {OUTPUT_DIR}", file=sys.stderr)
    print(f"Copied to project: {ASSETS_DIR}", file=sys.stderr)


if __name__ == "__main__":
    # Verify API key is available
    try:
        mh.api_key()
    except SystemExit as e:
        print(f"FATAL: {e}", file=sys.stderr)
        sys.exit(1)

    generate_all()
