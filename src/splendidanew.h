#ifndef SPLENDIDANEW_H
#define SPLENDIDANEW_H

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

#include "globals.h"
// #include "taskscheduler_wrapper.h" // Instead of <TaskScheduler.h>
#include "tables.h"
#include "palettes.h"

// #include "patterns.h"

void changeToBrightness();
void runPattern();

void fadeOut();
void fadeIn();
std::string timeToString();

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

void readEncoders();
void encoderSetup();

#endif // SPLENDIDANEW_H