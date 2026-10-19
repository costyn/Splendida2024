// The order in which patterns are shown. Add, remove or reorder entries here.
#include "patterns.h"

#define PATTERN(fn) {#fn, fn}

const Pattern gPatterns[] = {
    PATTERN(hypnoticWings),
    PATTERN(hypnoticWaves),
    PATTERN(DigitalRain),
    PATTERN(SoftTwinkles),
    PATTERN(cylindrical_Pattern),
    PATTERN(FireComets),
    PATTERN(Swirl),
    PATTERN(testCylinderMapping2),
    PATTERN(DiagonalPattern),
    PATTERN(spiralCylinderWave),
    // PATTERN(PlasmaBall),
    // PATTERN(F_lying),  // I don't like it enough.
    PATTERN(RGBTunnel),
    PATTERN(Flower),
    PATTERN(Distortion_Waves_cylinder),
    PATTERN(colorwaves),
    PATTERN(Distortion_Waves_planar),
    PATTERN(FireButterfly),
    PATTERN(Spiral2),
    PATTERN(pride),
    PATTERN(RGB_Caleidoscope2),
    PATTERN(RGB_Caleidoscope1),
    PATTERN(RGB_hiphotic),
    PATTERN(Spiral),
    PATTERN(fire2021),
};

const uint8_t gPatternCount = sizeof(gPatterns) / sizeof(gPatterns[0]);
