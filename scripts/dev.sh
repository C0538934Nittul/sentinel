#!/usr/bin/env bash
# dev.sh -- documents (does not automate) running all three processes for local development.
# Component: scripts
# Status: complete. This script intentionally does NOT background/orchestrate anything --
#         run each command in its own terminal tab so logs stay readable and Ctrl-C stops
#         only the process you're looking at.
set -euo pipefail

cat <<'EOF'
Sentinel local development -- run each of these in its own terminal tab:

  1) MongoDB (if not already running as a background service):
       brew services start mongodb-community

  2) C++ analyzer (build once, rebuild after changes):
       scripts/build-analyzer.sh debug

  3) API (from api/, after `cp .env.example .env` and `npm install`):
       cd api && npm run dev

  4) Client (from client/, after `cp .env.local.example .env.local` and `npm install`):
       cd client && npm run dev

The client (http://localhost:3000) talks only to the API (http://localhost:3001), which is the
only thing that talks to MongoDB and spawns the analyzer binary.
EOF
