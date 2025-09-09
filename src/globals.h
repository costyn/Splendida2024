// globals.h - NEW FILE
#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <FastLED.h>
#include <TaskSchedulerDeclarations.h>

// // Emulator
// #ifndef M5ATOM
// #define DATA_PIN 26           // set your leds datapin   change to 32 for m5 atom lite
// #define ATOMLED_PIN 19        // set your leds datapin   change to 27 for m5 atom lite
// #define BUTTON_PIN_INPUT 16   // button pin              change to 39 for m5 atom lite
// #define EXTRA_BUTTON_PIN 22   // button pin              change to 39 for m5 atom lite
// #define BRIGHTNESS_POT_PIN 12 // Brightness potentiometer pin
// #endif

// Can't define DATA_PIN in boards/ header files unfortunately.
#ifdef M5STACK_ATOM_S3
#define DATA_PIN 8
#endif

#ifdef M5STACK_ATOM
#define DATA_PIN 32
#endif

// Default in case the others dont work
#ifndef DATA_PIN
#define DATA_PIN 8
#endif

#define LED_TYPE WS2812B // leds type
#define COLOR_ORDER GRB  // color order of leds

#define MAX_POWER_MILLIAMPS 800 // write here your power in milliamps. default i set 800 mA for safety
#define MIN_BRIGHTNESS 1        // min brightness of leds
#define MAX_BRIGHTNESS 250      // max brightness of leds - will be limited by max power
#define BRIGHNESS_STEP 3        // brightness step
#define DEFAULT_BRIGHTNESS 50   // default brightness

#define NUM_COLS_PLANAR 20 // resolution of planar lookup table
#define NUM_ROWS_PLANAR 20 // resolution of planar lookup table
#define NUM_LEDS_PLANAR NUM_COLS_PLANAR *NUM_ROWS_PLANAR

#define NUM_COLS_CYLINDER 45 // resolution of cylindrical lookup table
#define NUM_ROWS_CYLINDER 11 // resolution of cinindrical lookup table
// #define NUM_LEDS_CYLINDER NUM_COLS_CILINDR *NUM_ROWS_CYLINDER

#define NUM_LEDS 256

#define MAX_ANIMATION_SPEED 0.4f
#define MIN_ANIMATION_SPEED -0.4f
#define DEFAULT_ANIMATION_SPEED 0.08f

// Animation Constants
#define SECONDS_PER_PALETTE 19
#define SECONDS_PER_PATTERN 123
#define BLEND_SPEED 16
#define BLEND_INTERVAL_MS 40
#define CROSSFADE_TIME 4000 // milliseconds
#define CROSSFADE_STEPS 255 // 255 = max (8 bit)

// Now you can see how long the crossfade is still active
#define ENCODER_ANIMATION_IDLE_TIMEOUT CROSSFADE_TIME

enum RenderBuffer
{
    BUFFER1,
    BUFFER2
};

typedef void (*PatternFunction)(CRGB *ledBuffer);
typedef PatternFunction SimplePatternList[];

// Extern declarations of global variables
extern uint8_t g_targetBrightness;
extern uint16_t g_lastSafeIndex;
extern float g_animationSpeed;
extern CRGBPalette16 gCurrentPalette;
extern CRGBPalette16 gTargetPalette;
extern float g_timeAccumulator;
extern SimplePatternList gPatterns;
extern const char *patternNames[];
extern const uint8_t gPatternCount;
extern uint8_t gBuffer1PatternNumber;
extern uint8_t gBuffer2PatternNumber;
extern byte g_buffer1InitNeeded;
extern byte g_buffer2InitNeeded;

extern uint8_t g_currentBrightness;
extern CRGB leds[NUM_LEDS];
extern const char *patternNames[];
extern uint8_t gCurrentPaletteNumber;

extern CRGB buffer1[NUM_LEDS + 1];
extern CRGB buffer2[NUM_LEDS + 1];
extern uint8_t _bufferBlendAmount;
extern RenderBuffer _renderBuffer;

float fmap(float x, float a, float b, float c, float d);

#endif
