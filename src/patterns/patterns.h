// Shared declarations for all pattern files.
//
//   ldirko.cpp   patterns from ldirko's Splendida 256 firmware (and Mark
//                Kriegsman's pride/colorwaves), adapted to double buffering
//   mapped.cpp   Costyn's patterns on ldirko's planar/cylindrical maps
//   spiral.cpp   Uzumaru patterns built on the exact LED geometry (geometry.h)
//   playlist.cpp the order patterns are shown in
#ifndef PATTERNS_H
#define PATTERNS_H

#include <Arduino.h>
#include <FastLED.h>

#include "globals.h"
#include "palettes.h"
#include "tables.h"

// Coordinate mapping into the LED buffer (index NUM_LEDS when there is no LED)
uint16_t XY_fibon_PLANAR(byte x, byte y);
uint16_t XY_CYLINDER(byte x, byte y);
void GammaCorrection(CRGB *ledBuffer);

// True once after a pattern has been (re)assigned to ledBuffer, so it can
// reset its state. Use instead of comparing against buffer1/buffer2.
bool patternNeedsInit(CRGB *ledBuffer);

// ldirko.cpp
void DigitalRain(CRGB *buffer);
void DiagonalPattern(CRGB *buffer);
void fire2021(CRGB *buffer);
void Distortion_Waves_planar(CRGB *buffer);
void RGB_hiphotic(CRGB *buffer);
void PlasmaBall(CRGB *buffer);
void FireComets(CRGB *buffer);
void F_lying(CRGB *buffer);
void RGBTunnel(CRGB *buffer);
void RGB_Caleidoscope1(CRGB *buffer);
void RGB_Caleidoscope2(CRGB *buffer);
void Distortion_Waves_cylinder(CRGB *buffer);
void FireButterfly(CRGB *buffer);
void Swirl(CRGB *buffer);
void SwirlPlanar(CRGB *buffer);
void cylindrical_Pattern(CRGB *buffer);
void Spiral(CRGB *buffer);
void Spiral2(CRGB *buffer);
void Flower(CRGB *buffer);
void pride(CRGB *buffer);
void colorwaves(CRGB *buffer);
void SoftTwinkles(CRGB *buffer);

// mapped.cpp
void spiralCylinderWave(CRGB *buffer);
void testCylinderMapping(CRGB *buffer);
void testCylinderMapping2(CRGB *buffer);
void hypnoticWings(CRGB *buffer);
void hypnoticWaves(CRGB *buffer);

#endif
