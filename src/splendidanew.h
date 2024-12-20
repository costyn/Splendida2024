#ifndef SPLENDIDA_H
#define SPLENDIDA_H

// Splendida 256 NEW demoreel with palletes
// fastled fibonacci 256 leds demo
// Yaroslaw Turbin
// https://vk.com/ldirko
// https://www.reddit.com/user/ldirko/
// https://twitter.com/ldir_ko

// controls: from start automode is enable and patterns change in loop
// one g_patternButton click change pattern to next and automode is OFF
// double click change bright in loop 0..maxbright with 7 steps. not affect to Automode
// long press activate Automode ON

#define M5ATOM

#include <Arduino.h>
#include "OneButton.h" // https://github.com/mathertel/OneButton
#include <FastLED.h>
#include <Smooth.h>
#include <TaskScheduler.h>
#include <driver/gpio.h>
#include <driver/adc.h>

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
#define DATA_PIN 26           // set your leds datapin   change to 32 for m5 atom lite
#define ATOMLED_PIN 21        // set your leds datapin   change to 27 for m5 atom lite
#define BUTTON_PIN_INPUT 39   // button pin              change to 39 for m5 atom lite
#define EXTRA_BUTTON_PIN 22   // button pin              change to 39 for m5 atom lite
#define BRIGHTNESS_POT_PIN 32 // Brightness potentiometer pin
#define SPEED_POT_PIN 33      // Speed potentiometer pin
#endif

#define LED_TYPE WS2812B // leds type
#define COLOR_ORDER GRB  // color order of leds

#define MAX_POWER_MILLIAMPS 800 // write here your power in milliamps. default i set 800 mA for safety
#define MAX_BRIGHTNESS 100      // max brightness of leds - will be limited by max power

#define NUM_COLS_PLANAR 20 // resolution of planar lookup table
#define NUM_ROWS_PLANAR 20 // resolution of planar lookup table
#define NUM_LEDS_PLANAR NUM_COLS_PLANAR *NUM_ROWS_PLANAR

#define NUM_COLS_CYLINDER 45 // resolution of cylindrical lookup table
#define NUM_ROWS_CYLINDER 11 // resolution of cinindrical lookup table
// #define NUM_LEDS_CYLINDER NUM_COLS_CILINDR *NUM_ROWS_CYLINDER

#define NUM_LEDS 256

// Animation Constants
#define SECONDS_PER_PALETTE 20
#define SECONDS_PER_PATTERN 60
#define BLEND_SPEED 16
#define BLEND_INTERVAL_MS 40

// Potentiometers
#define SMOOTHED_SAMPLE_SIZE 10
Smooth g_smoothedSpeedPot(SMOOTHED_SAMPLE_SIZE);
Smooth g_smoothedBrightnessPot(SMOOTHED_SAMPLE_SIZE);

uint8_t calculatePowerScaledBrightness(uint8_t targetBrightness);

uint16_t g_lastSafeIndex = 256;

CRGB leds[NUM_LEDS + 1];
CRGB g_statusLed[1];

byte rain[(NUM_COLS_PLANAR + 2) * (NUM_ROWS_PLANAR + 2)];

OneButton g_patternButton(BUTTON_PIN_INPUT, true);

byte g_patternInitNeeded = 1;

uint8_t g_fadeStartBrightness = 0;
uint8_t g_fadeTargetBrightness = 0;
uint8_t g_fadeCurrentBrightness = 0;

uint8_t g_currentBrightness = 0; // start dark
uint8_t g_targetBrightness = g_currentBrightness;

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

#define MAX_ANIMATION_SPEED 0.4f
#define MIN_ANIMATION_SPEED -0.4f
float g_timeAccumulator = 0.0f;
float g_animationSpeed = 0.2f; // Between -0.4 and 0.4

float fmap(float x, float a, float b, float c, float d);

enum FadeState
{
    FADE_NONE,
    FADING_OUT,
    FADING_IN
};

FadeState g_fadeState = FADE_NONE;

void changeToBrightness();
void runPattern();

static void oneClick();
static void doubleClick();
static void longPress();
void fadeOut();
void fadeIn();
std::string timeToString();

#include "palettes.h"
#include "tables.h"

// Function Prototypes
void initializeGPIO();
void initializeSerial();
void initializeLEDs();
void initializeButton();
void printPatternAndPalette();
void readPotentiometers();
void handleButton();
void changePalette();
void blendPalette();
void changePattern();
void startFadeOut();
void startFadeIn();
void fade();

Scheduler _runner;
Task _taskChangeToBrightness(10 * TASK_MILLISECOND, TASK_FOREVER, &changeToBrightness);
Task _taskRunPattern(1 * TASK_MILLISECOND, TASK_FOREVER, &runPattern);
Task _taskChangePalette(SECONDS_PER_PALETTE *TASK_SECOND, TASK_FOREVER, &changePalette);
Task _taskChangePattern(SECONDS_PER_PATTERN *TASK_SECOND, TASK_FOREVER, &changePattern);
Task _taskHandleButton(10 * TASK_MILLISECOND, TASK_FOREVER, &handleButton);
Task _taskReadPotentiometers(100 * TASK_MILLISECOND, TASK_FOREVER, &readPotentiometers);
Task _taskBlendPalette(BLEND_INTERVAL_MS *TASK_MILLISECOND, TASK_FOREVER, &blendPalette);
Task _taskFade(10 * TASK_MILLISECOND, TASK_FOREVER, &fade);

#include "patterns.h"

#endif