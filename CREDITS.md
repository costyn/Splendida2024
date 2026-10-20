# Credits

Uzumaru is Costyn van Dongen's project. It stands on the following work.

## Hardware

**Splendida 256 PCB** by Uri Shaked / Wokwi: https://github.com/wokwi/splendida
Licensed [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/). The LED layout (a Fermat spiral of 256 LEDs) and
the files in `extras/2d images`, `extras/splendida-pcb.stl` and `extras/layouts` derive from it. Any modified PCB that is
published must keep this attribution and the CC BY-SA license.

**Ldirko's shell with diffusers** (`extras/Ldirkos 3dmodel shell with diffusers`) by Yaroslaw Turbin:
https://www.thingiverse.com/thing:4791860

The seamless paper diffuser, basic diffuser and stand in `extras/` are Costyn's.

## Firmware

**ldirko's "New Colorful Splendida 256" firmware** by Yaroslaw Turbin (ldirko), 2021, GPL-3.0:
https://github.com/ldirko/New-Colorful-Splendida-256-firmware-
Uzumaru started as a fork of it and is GPL-3.0 because of that. From it come the lookup tables in `src/tables.h`, the
patterns in `src/patterns/ldirko.cpp` (modified for double buffering and a variable speed) and the first palette playlist.

**pride and colorwaves** by Mark Kriegsman, from the FastLED examples
(Pride2015 and ColorWavesWithPalettes), MIT.

**Fibonacci256 firmware** by Jason Coon / Evil Genius Labs, GPL-3.0:
https://github.com/jasoncoon/esp8266-fastled-webserver (branch `fibonacci256`, commit 7f3ef47). `Emitter`,
`SwirlFibonacci`, `Pacifica` and `PolarNoise` in `src/patterns/fibonacci256.cpp` are adapted from its
`emitterFibonacci`, `swirlFibonacci`, `pacifica_fibonacci_loop` and `drawNoisePolar` (rewritten for the Uzumaru
geometry, double buffering and the speed-scaled clock). `Pacifica` is Mark Kriegsman's, with Mary Corey March's
Fibonacci variation (MIT, FastLED examples).

**Palettes**: gradient palettes from [cpt-city](http://seaviewsensing.com/pub/cpt-city/), converted for FastLED, and the
palette set in `src/wled_palettes.h` from [WLED](https://github.com/Aircoookie/WLED).

**Libraries**: [FastLED](https://github.com/FastLED/FastLED) (MIT),
[TaskScheduler](https://github.com/arkhipenko/TaskScheduler) (BSD-3-Clause),
[DuPPa Arduino library](https://github.com/Fattoresaimon/ArduinoDuPPaLib) (GPL-3.0).

**beatsense protocol header** (`lib/beatsense_protocol`), MIT, from https://github.com/costyn/beatsense.

## Written for Uzumaru

The runtime (crossfading buffers, speed control, knob handling), the patterns in `src/patterns/mapped.cpp` and
`src/patterns/spiral.cpp`, the geometry table generator and the preview tools, by Costyn van Dongen with help from Claude.
