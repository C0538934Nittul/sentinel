#!/usr/bin/env bash
# build-analyzer.sh -- configure and build the C++ analyzer using the "debug" CMake preset.
# Component: scripts
# Status: complete. Usage: scripts/build-analyzer.sh [debug|release]
set -euo pipefail

PRESET="${1:-debug}"
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "Configuring analyzer (preset: $PRESET)..."
cmake --preset "$PRESET" -S "$REPO_ROOT/analyzer" -B "$REPO_ROOT/analyzer/build/$PRESET"

echo "Building analyzer (preset: $PRESET)..."
cmake --build --preset "$PRESET"

echo "Done. Binary (once rule logic is implemented) will be at:"
echo "  $REPO_ROOT/analyzer/build/$PRESET/sentinel-analyzer"
