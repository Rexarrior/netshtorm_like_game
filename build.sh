#!/bin/bash
# Build script for NetStorm-Like (macOS/Linux)

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

echo "=== NetStorm-Like Build Script ==="
echo "Directory: $SCRIPT_DIR"
echo ""

# Clean build directory if --clean flag
if [ "$1" = "--clean" ]; then
    echo "Cleaning build directory..."
    rm -rf build
fi

# Create build directory
mkdir -p build

# Configure with CMake
echo "[1/2] Configuring..."
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo "[2/2] Building..."
cmake --build . -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "=== Build Complete ==="
echo "Executable: $SCRIPT_DIR/build/netstorm_like"
echo "Tests:      $SCRIPT_DIR/build/netstorm_tests"
echo ""
echo "Run game:   ./build/netstorm_like"
echo "Run tests:  ./build/netstorm_tests"
