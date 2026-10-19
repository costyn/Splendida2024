// Coordinate mapping helpers shared by all patterns.
#include "patterns.h"

// 90 degree counter clockwise rotated
uint16_t XY_fibon_PLANAR(byte x, byte y)
{
    // newX, newY are the "rotated" coordinates
    byte newY = (NUM_COLS_PLANAR - 1) - x;
    byte newX = y;

    uint16_t ledsindex = pgm_read_word(
        FibonPlanarTable + (newY * NUM_COLS_PLANAR + newX));
    return ledsindex;
}

uint16_t XY_CYLINDER(byte x, byte y)
{ // calculate index in leds from XY coordinates for cylindrical mapping
    uint16_t ledsindex = pgm_read_word(FibonCilindrTable + y * NUM_COLS_CYLINDER + x);
    return (ledsindex);
}

void GammaCorrection(CRGB *ledBuffer)
{ // gamma correction function
    byte r, g, b;
    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
        r = ledBuffer[i].r;
        g = ledBuffer[i].g;
        b = ledBuffer[i].b;
        ledBuffer[i].r = pgm_read_byte(exp_gamma + r);
        ledBuffer[i].g = pgm_read_byte(exp_gamma + g);
        ledBuffer[i].b = pgm_read_byte(exp_gamma + b);
    }
}

bool patternNeedsInit(CRGB *ledBuffer)
{
    if (ledBuffer == buffer1 && g_buffer1InitNeeded)
    {
        g_buffer1InitNeeded = 0;
        return true;
    }
    if (ledBuffer == buffer2 && g_buffer2InitNeeded)
    {
        g_buffer2InitNeeded = 0;
        return true;
    }
    return false;
}
