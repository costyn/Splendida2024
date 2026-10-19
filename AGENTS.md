# Uzumaru: notes for coding agents

Uzumaru is a round LED lamp: 256 WS2812B LEDs on a Fibonacci (Vogel/sunflower) spiral PCB (the Splendida 256), an M5Stack
Atom Matrix (ESP32-PICO-D4), and one DuPPa I2C RGB rotary encoder as the only control. PlatformIO, Arduino framework,
FastLED, TaskScheduler. Read `README.md` for the user-facing picture and `docs/ARCHITECTURE_REVIEW.md` for how the
runtime works and what is still open.

## The owner usually cannot test on hardware

Assume nobody can flash or watch the lamp during a session. So:

- Never say something "works" or was "tested" unless it ran. Say what was checked: compiled for the ESP32, rendered on
  the desktop, ran under sanitizers, simulated in Wokwi. Hardware-only behaviour (LED timing, the encoder, I2C, power)
  stays "untested on hardware" until the owner confirms it.
- Prefer changes you can verify with the tools below. Keep risky runtime changes small, separate, and listed in the
  commit message or PR so the owner knows what to try first when they're back at the lamp.

## Build and verify

```
pio run -e m5stack_atommatrix                                    # firmware (must stay green)
PLATFORMIO_BUILD_FLAGS=-DUZUMARU_BEATSENSE pio run -e m5stack_atommatrix   # with the beatsense reader
tools/preview/build.sh                                           # desktop renderer -> .pio/preview/preview
python3 tools/preview/make_preview.py png out.png <Pattern> 1 3 6     # stills to look at
python3 tools/preview/make_preview.py html out.html [Pattern ...]     # animated page (publish as an artifact for the owner)
python3 tools/gen_geometry.py > src/geometry.h                   # only if tables.h changes; CI checks it is current
```

- The renderer compiles the real pattern sources against the real FastLED (stub platform) with a simulated clock. Use it
  to look at every pattern you write or change: render PNG stills at a few times and actually view them.
- **Refactors must be behaviour-preserving**: render every pattern before and after (`preview <pattern> 6 7 12 1 > x.bin`)
  and compare the bytes. A pure refactor gives identical output.
- `preview <pattern> <seconds> <palette> <frame-ms> <stride> <speed> <uptime-hours>`: also try negative speed and a long
  uptime (e.g. 60) for anything time-based.
- For memory/overflow bugs, build the renderer with `-fsanitize=address,undefined` (see the g++ line in `build.sh`) and
  run all patterns.
- CI (`.github/workflows/build.yml`) builds both firmware variants, checks `geometry.h`, and renders every pattern.

## Code layout and conventions

- `src/main.cpp` owns the runtime: TaskScheduler tasks, render loop, the two-buffer crossfade, brightness, palettes.
  `<TaskScheduler.h>` (the implementation) is included **only** there; everything else includes
  `<TaskSchedulerDeclarations.h>` via `globals.h`.
- `src/config.h`: pins and tuning constants. `src/globals.h`: shared state and the `Pattern {name, render}` type.
- Patterns are `void Name(CRGB *ledBuffer)` and must draw into the buffer they are given, never into `leds[]` or a
  specific buffer: two patterns render at once while crossfading. Register them in `src/patterns/playlist.cpp` with
  `PATTERN(Name)` and declare them in `src/patterns/patterns.h`.
  - Buffers have `NUM_LEDS + 1` entries; the lookup tables return `NUM_LEDS` for "no LED here", so that write is harmless.
  - Stateful patterns reset with `if (patternNeedsInit(ledBuffer)) { ... }`.
  - Time: use the speed-scaled clock, never `millis()` (unless the pattern deliberately ignores the speed knob).
    `animTime16(k)` for 16-bit phase math, `phase16(hz)` in `spiral.cpp`, `fmod(g_timeAccumulator * k, 2 * PI)` for
    `sin()`. The clock can run backwards (negative speed); particles use `forwardMs()`.
  - Prefer elapsed-time fades (`fadeByTime()` in `spiral.cpp`) over per-frame `fadeToBlackBy`: the frame rate is not fixed.
- `src/patterns/ldirko.cpp` is ported from ldirko's firmware; `mapped.cpp` is the owner's patterns on ldirko's maps;
  `spiral.cpp` uses the exact geometry. New geometry-based patterns go in `spiral.cpp` (or a new file listed in
  `patterns.h`'s header comment).
- `src/geometry.h` (generated): per physical LED `ledX`, `ledY` (0-255, y down, centre 128), `ledAngle` (0 = up, clockwise),
  `ledRadius`, `ledRank` (seed order from the centre). Spiral arms: `rank % F` is the arm of family F (8, 13, 21, 34),
  `rank / F` the position along it, `(rank * q) % F` the arm's order round the disc (q = 3, 5, 8, 13 for F = 8, 13, 21,
  34). The 21 arms wind clockwise outward, the 34 counter-clockwise; the PCB is wired along the 21 arms.
- `src/audio.h`: `g_audio` is a beatsense frame (tempo, beat phase, levels). Without a board it is a synthetic beat set by
  the speed knob. `-D UZUMARU_BEATSENSE` polls a real board at I2C 0x42 on the encoder's bus (untested on hardware).
  `lib/beatsense_protocol` is copied from github.com/costyn/beatsense; update it by copying, don't edit.
- Match the surrounding style: Allman braces, 4-space indent in pattern files (2 in `main.cpp`), short comments that say
  why. Keep FastLED integer math (`sin8`, `scale8`, `qadd8`) in per-LED loops; avoid `double` there (software-emulated
  on the ESP32).

## Dependencies

The owner prefers current versions. `platformio.ini` tracks the latest releases; pin a dependency only when a release
actually breaks the build or behaviour, with a comment saying why and what would unpin it. CI runs weekly to catch
upstream breakage.

## Attribution and licenses

GPL-3.0 firmware (inherited from ldirko's). Keep the original author headers in `ldirko.cpp`, `tables.h` and the palette
files, and say what was modified. The PCB design is CC BY-SA 4.0 (Uri Shaked / Wokwi). Update `CREDITS.md` when adding
third-party code, palettes or assets.

## Working style

- Use subagents for self-contained work (broad searches, dependency upgrades, research, tool setup), with a cheaper model
  where the task allows, to save tokens. Give them exact paths and tell them which files they may touch.
- Develop on the session's feature branch; don't push to `main` and don't open PRs unless asked.
- Commit messages: a short imperative subject, then what changed and why, and what was verified (and how).
