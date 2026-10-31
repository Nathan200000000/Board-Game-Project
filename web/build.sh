#!/usr/bin/env bash
# Builds the game to WebAssembly (WASI) for the browser version.
# Usage: WASI_SDK=/path/to/wasi-sdk web/build.sh
set -euo pipefail
cd "$(dirname "$0")/.."
: "${WASI_SDK:?Set WASI_SDK to your wasi-sdk install directory}"
"$WASI_SDK/bin/clang++" --target=wasm32-wasip1 -std=c++17 -O2 -fno-exceptions -Wl,--strip-all \
  -include web/wasi_shim.h \
  main.cpp Game.cpp Player.cpp \
  -o web/public/game.wasm
echo "Built web/public/game.wasm"
