#!/usr/bin/env bash
# Boot the built firmware in the Wokwi cloud simulator and check its serial output.
#
#   tools/wokwi/smoke.sh                run the boot test (about 1 s of simulated time)
#   tools/wokwi/smoke.sh --patterns     also wait for the first palette change (about 20 s simulated)
#   tools/wokwi/smoke.sh --screenshot   additionally try to screenshot the LED part (see README: may be unsupported)
#   tools/wokwi/smoke.sh --lint         only validate diagram.json (works offline, needs no token)
#
# Needs: a built firmware (pio run -e m5stack_atommatrix), the wokwi-cli binary and a
# WOKWI_CLI_TOKEN (https://wokwi.com/dashboard/ci). Exits non-zero without the token.
#
# Environment: WOKWI_CLI (path to the binary), TIMEOUT_MS (default 10000, 40000 with
# --patterns), OUT_DIR (default .pio/wokwi, where serial.log and screenshot.png go).
set -euo pipefail

cd "$(dirname "$0")/../.."          # repo root: wokwi.toml lives here

MODE=boot
SCREENSHOT=0
for arg in "$@"; do
  case "$arg" in
    --patterns)   MODE=patterns ;;
    --screenshot) SCREENSHOT=1 ;;
    --lint)       MODE=lint ;;
    -h|--help)    sed -n '2,13p' "$0"; exit 0 ;;
    *) echo "unknown option: $arg (try --help)" >&2; exit 2 ;;
  esac
done

# --- find wokwi-cli -----------------------------------------------------------
CLI="${WOKWI_CLI:-}"
if [ -z "$CLI" ]; then
  for c in "$(command -v wokwi-cli || true)" "$HOME/.wokwi/bin/wokwi-cli" "$HOME/bin/wokwi-cli"; do
    if [ -n "$c" ] && [ -x "$c" ]; then CLI="$c"; break; fi
  done
fi
if [ -z "$CLI" ]; then
  echo "wokwi-cli not found. Install it with:  curl -L https://wokwi.com/ci/install.sh | sh" >&2
  echo "(then add ~/bin to PATH, or set WOKWI_CLI=/path/to/wokwi-cli)" >&2
  exit 2
fi

# --- diagram lint (offline, no token) -----------------------------------------
# The "undocumented type" infos for board-esp32-devkit-c-v4 / wokwi-splendida are expected.
"$CLI" lint --offline . || { echo "diagram.json failed lint" >&2; exit 1; }
[ "$MODE" = lint ] && exit 0

# --- preconditions ------------------------------------------------------------
if [ -z "${WOKWI_CLI_TOKEN:-}" ]; then
  cat >&2 <<'MSG'

WOKWI_CLI_TOKEN is not set, so the simulation cannot run (the Wokwi simulator is a cloud service).
Create a free token at https://wokwi.com/dashboard/ci, then:
    export WOKWI_CLI_TOKEN=wok_...
    tools/wokwi/smoke.sh
MSG
  exit 2
fi
FW=".pio/build/m5stack_atommatrix/firmware.bin"
ELF=".pio/build/m5stack_atommatrix/firmware.elf"
for f in "$FW" "$ELF"; do
  [ -f "$f" ] || { echo "$f not found - build first: pio run -e m5stack_atommatrix" >&2; exit 2; }
done

# --- run ------------------------------------------------------------------------
OUT_DIR="${OUT_DIR:-.pio/wokwi}"
mkdir -p "$OUT_DIR"

# printPatternAndPalette() prints "<time>: printPatternAndPalette(): Pattern: <name> \tPalette: <name>"
# once at the end of setup() and again on every palette/pattern change. Seeing it proves the
# bootloader, Arduino core, FastLED init and the TaskScheduler loop all ran.
BOOT_TEXT="printPatternAndPalette(): Pattern:"
FAIL_TEXT="Guru Meditation"          # ESP32 panic banner (wokwi-cli accepts one --fail-text)

args=(--fail-text "$FAIL_TEXT" --serial-log-file "$OUT_DIR/serial.log")
if [ "$MODE" = patterns ]; then
  # Scenario: boot print, then the next print at 19 s (SECONDS_PER_PALETTE) = tasks still alive.
  args+=(--timeout "${TIMEOUT_MS:-40000}" --scenario tools/wokwi/scenarios/patterns.test.yaml)
else
  args+=(--timeout "${TIMEOUT_MS:-10000}" --expect-text "$BOOT_TEXT")
fi

echo "== wokwi smoke test ($MODE) =="
"$CLI" "${args[@]}" .
echo "== PASSED. serial log: $OUT_DIR/serial.log =="

if [ "$SCREENSHOT" = 1 ]; then
  # Separate run: if Wokwi cannot screenshot an LED part this must not fail the test above.
  echo "== trying a screenshot of part 'neopixels' =="
  if "$CLI" --timeout 5000 --screenshot-part neopixels --screenshot-time 3000 \
       --screenshot-file "$OUT_DIR/screenshot.png" .; then
    echo "screenshot: $OUT_DIR/screenshot.png"
  else
    echo "screenshot not available for this part (see tools/wokwi/README.md)" >&2
  fi
fi
