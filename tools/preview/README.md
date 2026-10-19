# Pattern preview

Renders the firmware's patterns on your computer. The pattern sources are compiled unchanged against the real FastLED (its
stub platform) with a simulated clock, so what you see is what the code computes; only the LEDs and the diffuser are
approximated.

```
pio run -e m5stack_atommatrix                  # once: downloads the current FastLED into .pio/libdeps
tools/preview/build.sh                         # builds .pio/preview/preview

python3 tools/preview/make_preview.py html preview.html            # every pattern, animated
python3 tools/preview/make_preview.py html preview.html Uzumaki     # just some
python3 tools/preview/make_preview.py png  still.png Bloom 1 3 6    # stills at 1, 3 and 6 s
```

Environment variables: `PALETTE` (palette index, default 7 = Sunset), `SPEED` (default 0.08), `PREVIEW_SECONDS` (clip
length, default 10).

The renderer itself writes raw frames (256 x RGB per frame) to stdout:

```
.pio/preview/preview --list
.pio/preview/preview <pattern> [seconds] [palette] [frame-ms] [stride] [speed] [uptime-hours]
```

`uptime-hours` starts the animation clock as if the lamp had been on that long, to check long-running behaviour.

It renders a single buffer, so it does not show crossfades. Files: `host_main.cpp` (the driver), `host_globals.inc` (the
globals `main.cpp` normally owns), `stubs/` (just enough Arduino and TaskScheduler), `player.html` (page template).
