#!/usr/bin/env bash
# One-time setup of a cloud (Linux) session for this repo. Run from the repo root:
#     bash tools/cloud/setup.sh
# Needs the owner's secret TW_BUILD_TOKEN in the environment (tools/cloud/README.md).
# Fetches: main.dol from the private build container; the compilers/tools (configure.py downloads
# them, as in CI); decomp-permuter and m2c (public GitHub) beside the repo.
set -euo pipefail
cd "$(dirname "$0")/../.."
PARENT="$(cd .. && pwd)"

command -v ninja >/dev/null || { echo "installing ninja"; pip install --quiet ninja; }
python3 -m pip install --quiet pycparser toml Levenshtein cxxfilt 2>/dev/null || true   # permuter deps

if [ ! -f orig/GW4E69/sys/main.dol ]; then
    python3 tools/cloud/fetch_orig.py
fi

mkdir -p "$PARENT/tools" "$PARENT/scratch/tw/agents" "$PARENT/tw2004-agents"
[ -d "$PARENT/tools/decomp-permuter" ] || git clone --quiet --depth 1 https://github.com/simonlindholm/decomp-permuter "$PARENT/tools/decomp-permuter"
[ -d "$PARENT/tools/m2c" ] || git clone --quiet --depth 1 https://github.com/matt-kempster/m2c "$PARENT/tools/m2c"

python3 configure.py
ninja | tail -1                                   # must print: build/GW4E69/main.dol: OK
ninja build/GW4E69/report.json >/dev/null
python3 tools/agents/status.py | head -1
