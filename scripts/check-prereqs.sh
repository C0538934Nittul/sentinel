#!/usr/bin/env bash
# check-prereqs.sh -- report which prerequisites for this project are missing.
# Component: scripts
# Status: complete. Installs nothing -- reports only.
set -euo pipefail

echo "Sentinel prerequisite check"
echo "----------------------------"

check() {
  local name="$1"
  local cmd="$2"
  if command -v "$cmd" >/dev/null 2>&1; then
    echo "[ok]      $name ($(command -v "$cmd"))"
  else
    echo "[missing] $name -- '$cmd' not found on PATH"
  fi
}

check "Xcode Command Line Tools (clang)" clang
check "CMake" cmake
check "Node.js" node
check "npm" npm
check "MongoDB shell (mongosh)" mongosh
check "Homebrew" brew

echo ""
if ! command -v mongod >/dev/null 2>&1; then
  echo "note: 'mongod' not found -- MongoDB Community server may not be installed."
  echo "      Install with: brew tap mongodb/brew && brew install mongodb-community"
fi

echo ""
echo "This script only reports status -- it does not install or modify anything."
