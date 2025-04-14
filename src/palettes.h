#ifndef PALETTES_H
#define PALETTES_H

// Gradient Color Palette definitions for 33 different cpt-city color palettes.
//     956 bytes of PROGMEM for all of the palettes together,
//    +618 bytes of PROGMEM for gradient palette code (AVR).
//   1,494 bytes total for all 34 color palettes and associated code.

#include <FastLED.h>
#include <wled_palettes.h>

extern const TProgmemRGBGradientPaletteRef gGradientPalettes[];

// Forward declarations of an array of cpt-city gradient palettes, and
// a count of how many there are.  The actual color palette definitions
// are at the bottom of this file.
extern const uint8_t gGradientPaletteCount;
// Current palette number from the 'playlist' of color palettes
extern uint8_t gCurrentPaletteNumber;
extern CRGBPalette16 gCurrentPalette;
// CRGBPalette16 gTargetPalette ( gGradientPalettes[random8(gGradientPaletteCount)] );
extern CRGBPalette16 gTargetPalette; // move initialisation with random8 to setup()
extern const char *paletteNames[];

#endif