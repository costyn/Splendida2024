#ifndef SPLENDIDA_H
#define SPLENDIDA_H

// Splendida 256 NEW demoreel with palletes
// fastled fibonacci 256 leds demo
// Yaroslaw Turbin
// https://vk.com/ldirko
// https://www.reddit.com/user/ldirko/
// https://twitter.com/ldir_ko

// controls: from start automode is enable and patterns change in loop
// one button click change pattern to next and automode is OFF
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
#define DATA_PIN 26                // set your leds datapin   change to 32 for m5 atom lite
#define ATOMLED_PIN 27             // set your leds datapin   change to 27 for m5 atom lite
#define BUTTON_PIN_INPUT 39        // button pin              change to 39 for m5 atom lite
#define EXTRA_BUTTON_PIN 22        // button pin              change to 39 for m5 atom lite
#define BRIGHTNESS_POT_PIN 32      // Brightness potentiometer pin
#define ADC_CHANNEL ADC1_CHANNEL_4 // Corresponding ADC channel for GPIO32
#define SPEED_POT_PIN GPIO_NUM_33  // Speed potentiometer pin
#endif

#define LED_TYPE WS2812B // leds type
#define COLOR_ORDER GRB  // color order of leds

#define MAX_POWER_MILLIAMPS 800 // write here your power in milliamps. default i set 800 mA for safety

#define NUM_COLS_PLANAR 20 // resolution of planar lookup table
#define NUM_ROWS_PLANAR 20 // resolution of planar lookup table
#define NUM_LEDS_PLANAR NUM_COLS_PLANAR *NUM_ROWS_PLANAR

#define NUM_COLS_CILINDR 45 // resolution of cilindrical lookup table
#define NUM_ROWS_CILINDR 11 // resolution of cinindrical lookup table
#define NUM_LEDS_CILINDR NUM_COLS_CILINDR *NUM_ROWS_CILINDR

#define NUM_LEDS 256

// Animation Constants
#define SECONDS_PER_PALETTE 10
#define SECONDS_PER_PATTERN 45
#define BLEND_SPEED 16
#define BLEND_INTERVAL_MS 40

// Potentiometers
#define SMOOTHED_SAMPLE_SIZE 20
#define BRIGHTNESS_THRESHOLD 2 // Adjust as needed
Smooth smoothedSpeedPot(SMOOTHED_SAMPLE_SIZE);
Smooth smoothedBrightnessPot(SMOOTHED_SAMPLE_SIZE);

uint16_t lastSafeIndex = 256;

CRGB leds[NUM_LEDS + 1];
CRGB statled[1];

byte rain[(NUM_COLS_PLANAR + 2) * (NUM_ROWS_PLANAR + 2)];

OneButton button(BUTTON_PIN_INPUT, true);

boolean automode = true; // change to false if you dont want automode on start
byte InitNeeded = 1;

uint8_t fadeStartBrightness = 0;
uint8_t fadeTargetBrightness = 0;
uint8_t fadeCurrentBrightness = 0;

uint8_t _currentBrightness = 0; // start dark
uint8_t _targetBrightness = _currentBrightness;

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

float gTimeAccumulator = 0;
uint8_t gSpeed = 8; // Adjust range as needed, controlled by pot

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
#include "patterns.h"

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

enum FadeState
{
    FADE_NONE,
    FADING_OUT,
    FADING_IN
};

FadeState fadeState = FADE_NONE;

Scheduler _runner;
Task _taskChangeToBrightness(10 * TASK_MILLISECOND, TASK_FOREVER, &changeToBrightness);
Task _taskRunPattern(1 * TASK_MILLISECOND, TASK_FOREVER, &runPattern);
Task _taskChangePalette(SECONDS_PER_PALETTE *TASK_SECOND, TASK_FOREVER, &changePalette);
Task _taskChangePattern(SECONDS_PER_PATTERN *TASK_SECOND, TASK_FOREVER, &changePattern);
Task _taskHandleButton(10 * TASK_MILLISECOND, TASK_FOREVER, &handleButton);
Task _taskReadPotentiometers(100 * TASK_MILLISECOND, TASK_FOREVER, &readPotentiometers);
Task _taskBlendPalette(BLEND_INTERVAL_MS *TASK_MILLISECOND, TASK_FOREVER, &blendPalette);
Task _taskFade(10 * TASK_MILLISECOND, TASK_FOREVER, &fade);

#endif