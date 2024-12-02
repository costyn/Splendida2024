# Splendida 2024

![Splendida in Action](https://raw.githubusercontent.com/costyn/Splendida2024/refs/heads/main/extras/Seamless%20Paper%20Diffusor/Completed.jpeg)

## Introduction
This is a "fork" of https://github.com/ldirko/New-Colorful-Splendida-256-firmware- by LDirko, which I've turned into a PlatformIO project.

Also adapted this README for legibility.

# New-Colorful-Splendida-256-firmware-
This new Fall 2021 firmware for Splendida 256 by WOKWI includes new and more colorful effects.

The sketch uses a button but can work without it in Automode.

This sketch uses one button to control patterns and brightness. The ESP32 M5 Atom Lite with a built-in button is ideal for this sketch (yes, I use Atom Lite). Attach the button to any pin you want and ground, then change the default button pin to your chosen pin.

### Controls:

- The firmware starts with Automode ON, and if the button is not pressed, patterns change in a loop.
- If the button is clicked once, Automode is OFF, and you can change to the next pattern by clicking the button.
- If the button is double-clicked, the brightness changes in a loop from 0 to maximum brightness in 7 steps (I think this is the best method to control brightness). Double-clicking does not affect the Automode state.
- If the button is long-pressed, Automode is ON, and patterns change in a loop because the button is not pressed.

This is the second version of my demoreel for Splendida. Now the effects are more colorful with blended palettes.

The sketch uses two libraries: 
* [FastLED](https://github.com/FastLED/FastLED)
* [OneButton](https://github.com/mathertel/OneButton)

### Emulator on Wokwi.com
If you want to try making your code for Splendida 256, I have an emulator page for this firmware: [Wokwi Emulator](https://wokwi.com/arduino/projects/309967369971696194). Try it!

### What is Splendida 256?

- [Splendida GitHub with PCB](https://github.com/wokwi/splendida)
- [Author's Twitter](https://twitter.com/UriShaked)
- [Buy it here](https://www.tindie.com/products/wokwi/splendida/) (out of stock as of this update)


### 3D Models 
LDirko made a nice-looking shell with a diffuser for it. Look for it in the 3d-models subdirectory, or grab it [here](https://www.thingiverse.com/thing:4791860)
