# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Splendida 2024 is a PlatformIO-based ESP32 firmware project for the Splendida 256 LED fibonacci spiral PCB. It's a fork of LDirko's colorful LED animation firmware, converted to use PlatformIO for better development workflow. The project creates animated LED patterns on a fibonacci spiral of WS2812B LEDs using FastLED library.

User control is through a single Duppa I2C rotary encoder.

## Development Commands

### Build and Upload
- **Build**: `pio run` or `pio run -e m5stack_atomlite_s3`
- **Upload**: `pio run -t upload` or `pio run -e m5stack_atomlite_s3 -t upload`
- **Monitor Serial**: `pio device monitor -b 115200`
- **Clean Build**: `pio run -t clean`

### Target Environments
- **m5stack_atomlite**: M5Stack Atom Lite
- **m5stack_atomlite_s3**: Default target, M5Stack Atom Lite S3 board

### Custom Build Targets
- **Merge Binary**: `pio run -t mergebin` - Creates a single flash image using `merge-bin.py`

## Architecture Overview

### Core Structure
- **src/splendidanew.cpp**: Main Arduino sketch with setup() and loop()
- **src/globals.h**: Central configuration and global variable declarations
- **src/patterns.h/cpp**: LED animation pattern implementations
- **src/palettes.h/cpp**: Color palette definitions and management
- **src/encoders.h**: I2C encoder interface (for hardware control knobs)
- **src/tasks.h**: TaskScheduler task definitions
- **src/boards/**: Board-specific pin configurations

### Animation System
The firmware uses a dual-buffer crossfading system:
- **buffer1[NUM_LEDS]** and **buffer2[NUM_LEDS]**: Two animation buffers
- **TaskScheduler**: Handles multiple concurrent tasks:
  - `_taskRunPattern`: Renders patterns at 1ms intervals
  - `_taskBufferCrossfade`: Smooth transitions between patterns (4 second crossfades)
  - `_taskChangeToBrightness`: Smooth brightness transitions
  - `_taskChangePalette`: Automatic palette changes every 19 seconds

### Hardware Configuration
- **NUM_LEDS**: 256 in a Fibonacci spiral
- **LED_TYPE**: WS2812B with GRB color order
- **DATA_PIN**: 22 (for M5Atom), configurable per board
- **Power Limiting**: MAX_POWER_MILLIAMPS = 800mA
- **Brightness**: 1-250 range, default 50

### Dependencies
Key libraries used:
- **FastLED**: LED control and effects
- **TaskScheduler**: Multi-threaded task management
- **ArduinoDuPPaLib**: Duppa I2C RGB Rotary Encoder
- **I2SClocklessLedDriver**: High-speed LED driving

### Board Variants
The project supports multiple hardware configurations:
- Emulator mode (Wokwi): Different pin assignments
- Board-specific headers in `src/boards/` define pin configurations

### Pattern Development
- Patterns are function pointers stored in `gPatterns[]` array
- Each pattern function signature: `void patternName(CRGB *ledBuffer)`
- Patterns use `g_animationSpeed` and `g_timeAccumulator` for timing
- Color palettes automatically blend and change during runtime

### Testing
The project includes Wokwi emulator support:
- **wokwi.toml**: Configuration for web-based simulation
- Emulator available at: https://wokwi.com/arduino/projects/309967369971696194

## File Organization
- **extras/**: 3D models, diffusers, and documentation
- **.vscode/**: VS Code configuration for PlatformIO; ignore this directory
- **test/**: Test files (if any)
- **diagram.json**: Wokwi circuit diagram