// globals.h - NEW FILE
#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <FastLED.h>
#include <TaskSchedulerDeclarations.h>

// Emulator
#ifndef M5ATOM
#define DATA_PIN 26           // set your leds datapin   change to 32 for m5 atom lite
#define ATOMLED_PIN 19        // set your leds datapin   change to 27 for m5 atom lite
#define BUTTON_PIN_INPUT 16   // button pin              change to 39 for m5 atom lite
#define EXTRA_BUTTON_PIN 22   // button pin              change to 39 for m5 atom lite
#define BRIGHTNESS_POT_PIN 12 // Brightness potentiometer pin
#define SPEED_POT_PIN 13      // Speed potentiometer pin
#endif

// Atom Matrix M5
#ifdef M5ATOM
#define DATA_PIN 26    // set your leds datapin   change to 32 for m5 atom lite
#define ATOMLED_PIN 21 // set your leds datapin   change to 27 for m5 atom lite
#endif

#define LED_TYPE WS2812B // leds type
#define COLOR_ORDER GRB  // color order of leds

#define MAX_POWER_MILLIAMPS 800 // write here your power in milliamps. default i set 800 mA for safety
#define MIN_BRIGHTNESS 1        // min brightness of leds
#define MAX_BRIGHTNESS 100      // max brightness of leds - will be limited by max power
#define BRIGHNESS_STEP 3        // brightness step
#define DEFAULT_BRIGHTNESS 60   // default brightness

#define NUM_COLS_PLANAR 20 // resolution of planar lookup table
#define NUM_ROWS_PLANAR 20 // resolution of planar lookup table
#define NUM_LEDS_PLANAR NUM_COLS_PLANAR *NUM_ROWS_PLANAR

#define NUM_COLS_CYLINDER 45 // resolution of cylindrical lookup table
#define NUM_ROWS_CYLINDER 11 // resolution of cinindrical lookup table
// #define NUM_LEDS_CYLINDER NUM_COLS_CILINDR *NUM_ROWS_CYLINDER

#define NUM_LEDS 256

#define MAX_ANIMATION_SPEED 0.4f
#define MIN_ANIMATION_SPEED -0.4f

// Animation Constants
#define SECONDS_PER_PALETTE 20
#define SECONDS_PER_PATTERN 60
#define BLEND_SPEED 16
#define BLEND_INTERVAL_MS 40

#define NUM_PATTERNS 25

typedef void (*SimplePatternList[])();

// Extern declarations of global variables
extern uint8_t g_targetBrightness;
extern uint8_t g_lastSafeIndex;
extern float g_animationSpeed;
extern CRGBPalette16 gCurrentPalette;
extern CRGBPalette16 gTargetPalette;
extern float g_timeAccumulator;
extern SimplePatternList gPatterns;
extern uint8_t gCurrentPatternNumber;
extern CRGB g_statusLed[];
extern byte g_patternInitNeeded;
extern uint8_t g_currentBrightness;
extern uint8_t g_fadeState;
extern CRGB leds[NUM_LEDS];
extern const char *patternNames[];
extern uint8_t gCurrentPaletteNumber;

extern uint8_t g_fadeStartBrightness;
extern uint8_t g_fadeTargetBrightness;
extern uint8_t g_fadeCurrentBrightness;

float fmap(float x, float a, float b, float c, float d);

enum FadeState
{
    FADE_NONE,
    FADING_OUT,
    FADING_IN
};

#endif
