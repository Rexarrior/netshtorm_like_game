#!/usr/bin/env python3
"""Remove background from generated pixel art assets using edge color sampling.

Logic:
1. Sample the color from image corners (assuming uniform background)
2. Find all pixels with color close to the sampled edge color
3. Make those pixels transparent (alpha = 0)
4. Save as PNG with transparency
"""

from __future__ import annotations

import os
import sys
import math
from pathlib import Path
from typing import Optional, Tuple

from PIL import Image


def get_edge_color(
    img: Image.Image, edge_sample_size: int = 10
) -> Tuple[int, int, int]:
    """Sample color from corners of the image, assuming uniform background."""
    pixels = img.load()
    w, h = img.size

    # Sample from all 4 corners
    corner_colors = []
    for x in range(min(edge_sample_size, w)):
        for y in range(min(edge_sample_size, h)):
            corner_colors.append(pixels[x, y])
            corner_colors.append(pixels[w - 1 - x, y])
            corner_colors.append(pixels[x, h - 1 - y])
            corner_colors.append(pixels[w - 1 - x, h - 1 - y])

    # Average the corner colors
    r = sum(c[0] for c in corner_colors) // len(corner_colors)
    g = sum(c[1] for c in corner_colors) // len(corner_colors)
    b = sum(c[2] for c in corner_colors) // len(corner_colors)

    return (r, g, b)


def color_distance(c1: Tuple[int, int, int], c2: Tuple[int, int, int]) -> float:
    """Calculate Euclidean distance between two RGB colors."""
    return math.sqrt((c1[0] - c2[0]) ** 2 + (c1[1] - c2[1]) ** 2 + (c1[2] - c2[2]) ** 2)


def remove_background(
    img: Image.Image, tolerance: float = 50.0, edge_sample_size: int = 10
) -> Image.Image:
    """Remove uniform background by making edge-colored pixels transparent."""
    # Convert to RGBA if not already
    if img.mode != "RGBA":
        img = img.convert("RGBA")

    # Get edge color
    edge_color = get_edge_color(img, edge_sample_size)
    print(f"  Edge color: {edge_color}", file=sys.stderr)

    pixels = img.load()
    w, h = img.size

    # Track changes for stats
    changed = 0
    unchanged = 0

    for y in range(h):
        for x in range(w):
            pixel = pixels[x, y]
            if len(pixel) < 4:
                # No alpha channel, treat as fully opaque
                current_color = pixel[:3]
            else:
                current_color = pixel[:3]

            dist = color_distance(current_color, edge_color)

            if dist < tolerance:
                # Make transparent
                pixels[x, y] = (pixel[0], pixel[1], pixel[2], 0)
                changed += 1
            else:
                unchanged += 1

    print(f"  Changed {changed} pixels, kept {unchanged} pixels", file=sys.stderr)
    return img


def process_image(
    input_path: Path,
    output_path: Optional[Path] = None,
    tolerance: float = 50.0,
    edge_sample_size: int = 10,
) -> bool:
    """Process a single image to remove background."""
    try:
        print(f"Processing: {input_path}", file=sys.stderr)

        img = Image.open(input_path)
        print(f"  Mode: {img.mode}, Size: {img.size}", file=sys.stderr)

        # Remove background
        img_clean = remove_background(img, tolerance, edge_sample_size)

        # Save
        if output_path is None:
            output_path = input_path

        img_clean.save(output_path, "PNG")
        print(f"  Saved: {output_path}", file=sys.stderr)

        return True

    except Exception as e:
        print(f"  ERROR: {e}", file=sys.stderr)
        return False


def process_directory(
    dir_path: Path,
    tolerance: float = 50.0,
    extensions: tuple = (".png", ".jpg", ".jpeg"),
) -> int:
    """Process all images in a directory."""
    success = 0
    failed = 0

    dir_path = Path(dir_path)
    for ext in extensions:
        for img_path in dir_path.rglob(f"*{ext}"):
            if process_image(img_path, tolerance=tolerance):
                success += 1
            else:
                failed += 1

    return success - failed


def main() -> None:
    import argparse

    parser = argparse.ArgumentParser(
        description="Remove background from pixel art assets"
    )
    parser.add_argument("path", type=Path, help="Image file or directory to process")
    parser.add_argument(
        "-o", "--output", type=Path, help="Output file (for single file mode)"
    )
    parser.add_argument(
        "-t",
        "--tolerance",
        type=float,
        default=50.0,
        help="Color distance tolerance for background removal (default: 50.0)",
    )
    parser.add_argument(
        "-e",
        "--edge-sample",
        type=int,
        default=10,
        help="Number of edge pixels to sample (default: 10)",
    )
    parser.add_argument(
        "-r", "--recursive", action="store_true", help="Process directories recursively"
    )

    args = parser.parse_args()

    path = args.path

    if not path.exists():
        print(f"ERROR: Path does not exist: {path}", file=sys.stderr)
        sys.exit(1)

    if path.is_file():
        process_image(path, args.output, args.tolerance, args.edge_sample)
    elif path.is_dir():
        if args.recursive:
            result = process_directory(path, args.tolerance)
            print(f"Processed directory recursively: {result >= 0}", file=sys.stderr)
        else:
            for ext in (".png", ".jpg", ".jpeg"):
                for img_path in path.glob(f"*{ext}"):
                    process_image(img_path, tolerance=args.tolerance)


if __name__ == "__main__":
    main()
