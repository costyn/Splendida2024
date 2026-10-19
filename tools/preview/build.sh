#!/usr/bin/env bash
# Build the desktop preview renderer.  Usage: tools/preview/build.sh [out-dir]
# Uses the FastLED copy PlatformIO already downloaded (run `pio run` once),
# or clones the pinned version if it is missing.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
OUT="${1:-$ROOT/.pio/preview}"
FASTLED_VERSION="3.9.15"
FASTLED="$(ls -d "$ROOT"/.pio/libdeps/*/FastLED 2>/dev/null | head -1 || true)"
if [ -z "$FASTLED" ]; then
  FASTLED="$OUT/FastLED"
  [ -d "$FASTLED" ] || git clone -q --depth 1 --branch "$FASTLED_VERSION" https://github.com/FastLED/FastLED.git "$FASTLED"
fi
INC="${PREVIEW_GLOBALS:-host_globals.inc}"
mkdir -p "$OUT/fastled-obj"
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -O2 -DFASTLED_STUB_IMPL -w)
# FastLED itself (cached between runs); our own clock replaces the stub one.
for f in $(cd "$FASTLED" && find src -name '*.cpp' ! -name led_sysdefs_stub.cpp); do
  o="$OUT/fastled-obj/$(echo "$f" | tr / _).o"
  [ -f "$o" ] || "$CXX" "${FLAGS[@]}" -I"$FASTLED/src" -c "$FASTLED/$f" -o "$o"
done
# Everything except the firmware entry point and the encoder driver.
SOURCES=$(find "$ROOT/src" -name '*.cpp' ! -name 'main.cpp' ! -name 'splendidanew.cpp' ! -name 'encoder*.cpp' | sort)
"$CXX" "${FLAGS[@]}" -DHOST_GLOBALS="\"$INC\"" -DHOST_PREVIEW -include "$HERE/stubs/Arduino.h" \
  -I"$HERE/stubs" -I"$HERE" -I"$FASTLED/src" -I"$ROOT/src" -I"$ROOT/lib/beatsense_protocol" \
  "$HERE/host_main.cpp" $SOURCES "$OUT"/fastled-obj/*.o -o "$OUT/preview" -lpthread
echo "$OUT/preview"
