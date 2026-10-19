# Architecture review (October 2026)

A review of the Uzumaru firmware as of commit `5795e36` (April 2025), what was changed on branch
`claude/uzumaru-restructure-review-eumy7o`, and what is left as recommendations. Nothing here has run on the real lamp yet:
everything was checked by compiling for the ESP32 and by rendering the pattern code on a desktop (see `tools/preview`).

## How it works

```
TaskScheduler loop
 ├─ runPattern (every 1 ms, i.e. as fast as it can)
 │    audioUpdate()                      beat clock (synthetic or beatsense)
 │    gPatterns[n1].render(buffer1)       both buffers render every frame
 │    gPatterns[n2].render(buffer2)
 │    blend(buffer1, buffer2 -> leds)     _bufferBlendAmount: 0 = buffer1, 255 = buffer2
 │    FastLED.show()                      ~7.7 ms for 256 WS2812B
 ├─ bufferCrossfade (255 steps x 15 ms)   ramps the blend, then swaps roles
 ├─ changePattern (every 123 s, or knob)  puts the next pattern in the hidden buffer
 ├─ changePalette / blendPalette          random palette every 19 s, blended smoothly
 ├─ changeToBrightness                    eases brightness to the target
 └─ readEncoder / encoder idle animation
```

The design is sound and suits the lamp:

- **Two persistent buffers with a crossfade** make every transition smooth, and they let "feedback" patterns (those that
  `nblend` or fade the previous frame) keep their own history.
- **A scaled animation clock** (`g_timeAccumulator`) gives one speed knob for every pattern, including running backwards.
- **Cooperative tasks** keep the knob, the palette blend and the brightness ramp independent of the render loop.

## Fixed on this branch

| # | Problem | Effect | Fix |
|---|---------|--------|-----|
| 1 | Dependencies unpinned | The code **no longer compiles**: espressif32 8.0 (released 19 Oct 2026, Arduino-ESP32 3.x) makes `round(int)` ambiguous, and FastLED / TaskScheduler moved on too | Pinned espressif32 6.10.0, FastLED 3.9.15, TaskScheduler 3.8.5 and the DuPPa commit, the versions current at the April 2025 build; fixed the `round()` call |
| 2 | Animation clock was a `float` | Increments get rounded: patterns run ~4% fast after ~12 h and **freeze after ~58 h** of uptime (measured) | `double` clock; `animTime16()` makes the 16-bit wrap explicit (a direct float to `uint16_t` cast is undefined once out of range) |
| 3 | Encoder counter limited to 0..254 with wrap off | After a net ~104 clicks one way, turning that way **stops doing anything** (the chip only reports min/max, which nothing listens to) | Re-centre the counter on every step |
| 4 | Turning the knob back in playlist mode | Also went to the *next* pattern | Counter-clockwise goes to the previous pattern |
| 5 | Pattern change during a crossfade | Fast turns re-picked the same pattern; the new pattern got no state reset | Step from the incoming pattern and set its init flag in `changePattern()` |
| 6 | `rain[]` defined in `patterns.h`, shared by DigitalRain and FireComets | Any second file including the header fails to link; the two patterns corrupt each other if both are on screen | Separate static buffers in `ldirko.cpp` |
| 7 | `Serial.printf("%s", String)` | Prints garbage (passes the object, not its text) | Pass a `const char *` |
| 8 | Encoder INT pin floating without the encoder | A missing or loose encoder (or Wokwi) reads INT as low and fires random events | `INPUT_PULLUP` (INT is open-drain) |
| 9 | Wokwi diagram wired LEDs to GPIO 26 | The simulator showed nothing | GPIO 22, unused parts removed |
| 10 | `patternNames[]` and `gPatterns[]` kept in sync by hand | An off-by-one reorder prints the wrong name, or reads past the array | One `Pattern {name, render}` table built with a macro |
| 11 | Patterns compared `ledBuffer == buffer1` to know when to reset | Every stateful pattern repeated the double-buffer internals | `patternNeedsInit(ledBuffer)` |
| 12 | `<TaskScheduler.h>` (which contains the implementation) included from `patterns.h` | Worked only because one file included it; a second include breaks the link | Included once, in `main.cpp`; others use the declarations header |
| 13 | Dead code | OneButton handlers, an unused power helper with a divide by zero, the sprite renderer, abandoned Swirl variants, unused libraries | Removed |

The restructure is behaviour-preserving: all 23 existing patterns render byte-identical frames before and after the file
split (checked with the desktop renderer). The clock change (#2) alters frames only by rounding (mean difference under
0.07/255). All patterns run clean under AddressSanitizer and UndefinedBehaviorSanitizer at normal, reversed and maximum
speed and after 60 h of simulated uptime.

## Recommendations (not changed)

These change how patterns look or need a test on the lamp, so they are left for you to decide.

1. **Cap the frame rate.** `runPattern` runs every 1 ms, so the frame rate is whatever the current pattern pair allows
   (an estimated 60-90 fps; worth logging on the lamp). Feedback patterns (`nblend(..., 16)`, `fadeToBlackBy`, SoftTwinkles, FireComets, PlasmaBall) and the
   `EVERY_N_MILLISECONDS` ones then run at a speed that depends on which other pattern shares the frame. A fixed 60 fps
   (16 ms task) would make them consistent, but would slow some down from what you're used to. The new Vortex shows the
   alternative: fade by elapsed time (`fadeByTime()` in `spiral.cpp`).
2. **Render only the visible buffer outside a crossfade.** It halves the CPU per frame (and with item 1 frees time for
   beatsense polling). Needs a `patternNeedsInit` on the incoming buffer, which `changePattern()` now sets.
3. **Some patterns ignore the speed knob**: fire2021 (on purpose), DigitalRain and F_lying (fixed `EVERY_N_MILLISECONDS`),
   SoftTwinkles (per frame), PlasmaBall and FireComets (the speed only shifts a `beatsin8` BPM, so they never run backwards). Worth
   marking per pattern, so the knob LED could show "speed not supported".
4. **hypnoticWaves scales by `g_currentBrightness`** while FastLED also applies the brightness, so it is squared: at
   brightness 50 the pattern peaks at about 4% output. If that's not intended, use 127.5 like hypnoticWings.
5. **`code()` in RGBTunnel** (`x -= ... ) * x`) is unsequenced, the compiler warns. It renders deterministically with this
   compiler, but may change with another; worth rewriting with temporaries if it ever looks different after an upgrade.
6. **Names with a leading underscore at global scope** (`_runner`, `_taskX`, `_renderBuffer`) are reserved identifiers in
   C++. Harmless in practice; rename to `g_` when convenient.
7. **Lookup tables are defined in headers** (`tables.h`, `wled_palettes.h`, `geometry.h`). They are `const`, so each file
   gets its own copy and unused copies are dropped by the linker. Fine at this size; move to `.cpp` files if more files
   start including them.

## Music reactivity (beatsense)

`src/audio.h` is shaped like a beatsense frame (tempo, beat phase, beat-in-bar, levels, onsets, energy, groove):

- **Wiring**: beatsense is an I2C slave at 0x42, so it shares the encoder's bus (SDA 33, SCL 23) with no new pins. The
  pull-ups live on the beatsense board. beatsense runs at 400 kHz; if the encoder is happy at that speed too,
  `Wire.setClock(400000)` would make the 18-byte read four times shorter.
- **Code**: `-D UZUMARU_BEATSENSE` makes `audioUpdate()` read and CRC-check a frame each render frame, falling back to the
  synthetic beat when there is no board or no signal. This compiles but has not run against a real board.
- **Patterns**: Heartbeat follows `beatPhase` and `beatInBar` and scales with the bass level; Ripples drops on each beat
  when live. The beat phase is state, so a pattern can draw "where in the beat we are" without missing beats.
- **Next steps**: decide which patterns react (a per-pattern flag), and use `energy`/`groove` to pick calmer or busier
  patterns. If polling costs too much frame time, poll every second frame; the phase is extrapolated on the board.

## Testing without the lamp

- `tools/preview` renders any pattern from the real code; `.github/workflows/build.yml` builds the firmware (with and
  without beatsense), checks `geometry.h` is up to date and renders every pattern on each push.
- **Wokwi**: the repo has `wokwi.toml` and `diagram.json` with Wokwi's own Splendida part. To run it in CI or from this
  environment, the Wokwi CLI needs a `WOKWI_CLI_TOKEN` (create one at wokwi.com, under CI tokens). The encoder has no Wokwi
  part, so the simulation shows the auto playlist only.
