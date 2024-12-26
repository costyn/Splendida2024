#ifndef PATTERNS_H
#define PATTERNS_H

// A couple function prototypes
// void changePalette();

// system procedures_____________________________________

#include <Arduino.h>
#include <FastLED.h>

#include "globals.h"
#include "tasks.h" // Add tasks.h include
#include "tables.h"

typedef PatternFunction SimplePatternList[];

byte rain[(NUM_COLS_PLANAR + 2) * (NUM_ROWS_PLANAR + 2)];

extern const char *patternNames[];
extern const uint8_t gPatternCount;

void SoftTwinkles(CRGB *buffer);
void cylindrical_Pattern(CRGB *buffer);
void FireComets(CRGB *buffer);
void hypnoticWaves(CRGB *buffer);
void testCylinderMapping2(CRGB *buffer);
void DiagonalPattern(CRGB *buffer);
void hypnoticWings(CRGB *buffer);
void spiralCylinderWave(CRGB *buffer);
void PlasmaBall(CRGB *buffer);
void RGBTunnel(CRGB *buffer);
void Flower(CRGB *buffer);
void Distortion_Waves_cylinder(CRGB *buffer);
void colorwaves(CRGB *buffer);
void Distortion_Waves_planar(CRGB *buffer);
void FireButterfly(CRGB *buffer);
void Spiral2(CRGB *buffer);
void pride(CRGB *buffer);
void RGB_Caleidoscope2(CRGB *buffer);
void RGB_Caleidoscope1(CRGB *buffer);
void Swirl(CRGB *buffer);
void RGB_hiphotic(CRGB *buffer);
void Spiral(CRGB *buffer);
void DigitalRain(CRGB *buffer);
void fire2021(CRGB *buffer);

void balls();
void initializeRain();
void updaterain(CRGB *buffer);
void changeRainPatter();

#endif