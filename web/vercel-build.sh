#!/usr/bin/env bash
# Full web build (used by Vercel): vendors xterm.js and compiles the C++ to WebAssembly.
set -euo pipefail
cd "$(dirname "$0")/.."

mkdir -p web/public/vendor
cp node_modules/@xterm/xterm/lib/xterm.js node_modules/@xterm/xterm/css/xterm.css \
   node_modules/@xterm/addon-fit/lib/addon-fit.js web/public/vendor/

WASI_VERSION=33
if [ -z "${WASI_SDK:-}" ]; then
  WASI_SDK="/tmp/wasi-sdk-${WASI_VERSION}.0-x86_64-linux"
  if [ ! -x "$WASI_SDK/bin/clang++" ]; then
    echo "Downloading wasi-sdk ${WASI_VERSION}…"
    curl -fsSL "https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-${WASI_VERSION}/wasi-sdk-${WASI_VERSION}.0-x86_64-linux.tar.gz" \
      | tar xz -C /tmp
  fi
fi
WASI_SDK="$WASI_SDK" bash web/build.sh
