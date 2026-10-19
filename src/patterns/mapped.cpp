// Costyn van Dongen's patterns using ldirko's planar and cylindrical maps.
#include "patterns.h"

void spiralCylinderWave(CRGB *ledBuffer)
{
    // Animation parameters
    const float waveLength = 3.0f;       // Controls the number of waves around the cylinder
    const float speedMultiplier = 0.03f; // Adjust this to control speed via g_timeAccumulator
    const float phase = fmod(g_timeAccumulator * speedMultiplier, 2 * PI);

    // Loop through cylinder coordinates
    for (uint8_t x = 0; x < NUM_COLS_CYLINDER; x++)
    {
        for (uint8_t y = 0; y < NUM_ROWS_CYLINDER; y++)
        {
            // Get LED index from coordinates
            uint16_t index = XY_CYLINDER(x, y);

            // Skip invalid indices
            if (index == g_lastSafeIndex || index >= NUM_LEDS)
                continue;

            // Calculate angle around the cylinder
            float angle = ((float)x / NUM_COLS_CYLINDER) * 2.0f * PI * waveLength;

            // Use g_timeAccumulator for time-based animation
            float wave = sin(angle + phase + ((float)y / NUM_ROWS_CYLINDER) * PI);

            // Map wave to brightness (0-255)
            uint8_t brightness = (uint8_t)((wave + 1.0f) * 127.5f); // wave ranges from -1 to 1

            // Get color from palette based on y position
            uint8_t colorIndex = map(y, 0, NUM_ROWS_CYLINDER - 1, 0, 255);
            CRGB color = ColorFromPalette(gCurrentPalette, colorIndex, brightness);

            // Set the LED color
            ledBuffer[index] = color;
        }
    }
}

void testCylinderMapping(CRGB *ledBuffer)
{
    for (uint8_t y = 0; y < NUM_ROWS_CYLINDER; y++)
    {
        for (uint8_t x = 0; x < NUM_COLS_CYLINDER; x++)
        {
            // Get LED index from coordinates
            uint16_t index = XY_CYLINDER(x, y);

            // Skip invalid indices
            if (index == g_lastSafeIndex || index >= NUM_LEDS)
                continue;

            // Set color based on x-position to create a horizontal gradient
            uint8_t hue = map(x, 0, NUM_COLS_CYLINDER - 1, 0, 255);

            // Set LED color using HSV hue
            ledBuffer[index] = CHSV(hue, 255, 255);
        }
    }
}

void testCylinderMapping2(CRGB *ledBuffer)
{
    // Calculate rotation angle based on time
    uint8_t scaledAngle = (uint8_t)animTime16(0.333); // Adjust multiplier for speed

    for (uint8_t y = 0; y < NUM_ROWS_CYLINDER; y++)
    {
        for (uint8_t x = 0; x < NUM_COLS_CYLINDER; x++)
        {
            uint16_t index = XY_CYLINDER(x, y);
            if (index == g_lastSafeIndex || index >= NUM_LEDS)
                continue;

            // Calculate rotated x position
            int16_t rotatedX = ((int16_t)x * cos8(scaledAngle) - (int16_t)y * sin8(scaledAngle)) >> 8;

            // Map rotated position to hue
            uint8_t hue = map(fmod(rotatedX, NUM_COLS_CYLINDER),
                              0, NUM_COLS_CYLINDER - 1,
                              0, 255);

            // Set LED color using HSV hue
            ledBuffer[index] = CHSV(hue, 255, 255); // 255 brightness is too much, 128 too little
        }
    }
    GammaCorrection(ledBuffer);
}

void hypnoticWings(CRGB *ledBuffer)
{
    // Animation parameters
    const float speedMultiplier = 0.025; // Speed of the inward movement
    const float frequency = 0.2;         // Controls the number of waves
    const float amplitude = 127.5;       // Half of 255 for brightness calculation

    // Time variable for animation
    float time = fmod(g_timeAccumulator * speedMultiplier, 2 * PI);

    // Center coordinates
    float centerX = NUM_COLS_CYLINDER / 0.9;
    float centerY = NUM_ROWS_CYLINDER / 0.9;

    // Loop through cylinder coordinates
    for (uint8_t x = 0; x < NUM_COLS_CYLINDER; x++)
    {
        for (uint8_t y = 0; y < NUM_ROWS_CYLINDER; y++)
        {
            // Get LED index from coordinates
            uint16_t index = XY_CYLINDER(x, y);

            // Skip invalid indices
            if (index == g_lastSafeIndex || index >= NUM_LEDS)
                continue;

            // Calculate distance from center
            float deltaX = x - centerX;
            float deltaY = y - centerY;
            float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

            // Calculate wave pattern moving inward
            float wave = sin(distance * frequency - time);

            // Map wave to brightness (0-255)
            uint8_t brightness = (uint8_t)((wave + 1.0f) * amplitude);

            // Get color from the palette based on distance
            uint8_t colorIndex = map(distance, 0, sqrt(centerX * centerX + centerY * centerY), 0, 255);
            CRGB color = ColorFromPalette(gCurrentPalette, colorIndex, brightness);

            // Set the LED color
            ledBuffer[index] = color;
        }
    }
}

void hypnoticWaves(CRGB *ledBuffer)
{
    // Animation parameters
    const float speedMultiplier = 0.025;                // Speed of the inward movement
    const float frequency = 1.0;                        // Controls the number of waves
    const float amplitude = g_currentBrightness * 0.5f; // Half of 255 for brightness calculation

    // Time variable for animation
    float time = fmod(g_timeAccumulator * speedMultiplier, 2 * PI);

    // Center coordinates
    float centerX = NUM_COLS_PLANAR / 2.125; // Changed from 2.1 to 2.0 for true center
    float centerY = NUM_ROWS_PLANAR / 2.125; // Changed from 2.1 to 2.0 for true center

    // Calculate max possible distance for proper scaling
    float maxDistance = sqrt((NUM_COLS_PLANAR * NUM_COLS_PLANAR) +
                             (NUM_ROWS_PLANAR * NUM_ROWS_PLANAR)) /
                        2.0;

    // Loop through cylinder coordinates
    for (uint8_t x = 0; x < NUM_COLS_PLANAR; x++)
    {
        for (uint8_t y = 0; y < NUM_ROWS_PLANAR; y++)
        {
            // Get LED index from coordinates
            uint16_t index = XY_fibon_PLANAR(x, y);

            // Skip invalid indices
            if (index == g_lastSafeIndex || index >= NUM_LEDS)
                continue;

            // Calculate distance from center with slight offset to avoid zero
            float deltaX = x - centerX;
            float deltaY = y - centerY;
            float distance = sqrt(deltaX * deltaX + deltaY * deltaY) + 0.5; // Added small offset

            // Calculate wave pattern moving inward
            float wave = sin(distance * frequency - time);

            // Map wave to brightness (0-255)
            uint8_t brightness = (uint8_t)((wave + 1.0f) * amplitude);

            // Get color from the palette based on normalized distance
            uint8_t colorIndex = map(distance * 16, 0, maxDistance * 16, 0, 255);

            // Set the LED color with full brightness at center
            CRGB color = ColorFromPalette(gCurrentPalette, colorIndex, brightness);
            ledBuffer[index] = color;
        }
    }
}
