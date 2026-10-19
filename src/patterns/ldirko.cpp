// Patterns from ldirko's "New Colorful Splendida 256" firmware
// https://github.com/ldirko/New-Colorful-Splendida-256-firmware-
// Yaroslaw Turbin (ldirko), 2021 - GPL-3.0
// https://vk.com/ldirko
// https://www.reddit.com/user/ldirko/
// https://twitter.com/ldir_ko
//
// pride() and colorwaves() are by Mark Kriegsman (FastLED examples).
//
// Modified by Costyn van Dongen 2024-2025: render into a caller-supplied buffer
// (for crossfading) and use g_timeAccumulator instead of millis() so the
// encoder can change the speed. The original millis() divisors map onto
// g_timeAccumulator (at default speed) as: millis()/8 -> 1.0x, /6 -> 1.33x,
// /3 -> 2.67x, /24 -> 0.333x, millis() -> 8x.

#include "patterns.h"

static byte rain[NUM_LEDS_PLANAR];                                   // DigitalRain dots
static byte cometHeat[(NUM_COLS_PLANAR + 2) * (NUM_ROWS_PLANAR + 2)]; // FireComets heat map

//_________________________ planar map patterns

// DigitalRain_____________________________________

static void changeRainPatter()
{
    int rand1 = random16(NUM_LEDS_PLANAR);
    int rand2 = random16(NUM_LEDS_PLANAR);
    if (rain[rand1] && !rain[rand2])
    {
        rain[rand1] = 0;
        rain[rand2] = 1;
    }
} // changeRainPatter

static void initializeRain()
{ // init array of dots. run once
    for (int i = 0; i < NUM_LEDS_PLANAR; i++)
        rain[i] = 0;
    byte rainNumb = random8(20, 30); // 5..8 how many dots with some random variation
    for (int i = 0; i < rainNumb; i++)
        rain[random16(NUM_LEDS_PLANAR)] = 1;
} // initializeRain

static void updaterain(CRGB *ledBuffer)
{
    static int speed = 1;

    for (byte j = 0; j < NUM_ROWS_PLANAR; j++)
    {
        int yindex = (j + speed) % NUM_ROWS_PLANAR * NUM_COLS_PLANAR;
        for (byte i = 0; i < NUM_COLS_PLANAR; i++)
        {
            byte layer = rain[yindex + i];
            if (layer)
                ledBuffer[XY_fibon_PLANAR((NUM_COLS_PLANAR - 1) - i, (NUM_ROWS_PLANAR - 1) - j)].setHue(100);
        }
    }

    fadeToBlackBy(ledBuffer, NUM_LEDS, 70);
    speed++;
} // updaterain

void DigitalRain(CRGB *ledBuffer)
{
    if (patternNeedsInit(ledBuffer))
    {
        initializeRain();
    }

    EVERY_N_MILLISECONDS(80) { updaterain(ledBuffer); }
    EVERY_N_MILLISECONDS(15) { changeRainPatter(); }
}

void DiagonalPattern(CRGB *ledBuffer)
{
    // Get rotation angle from time
    float angle = fmod(g_timeAccumulator * 0.001, 2 * PI); // Adjust speed multiplier as needed
    float centerX = NUM_COLS_PLANAR / 2.0;
    float centerY = NUM_ROWS_PLANAR / 2.0;

    for (byte j = 0; j < NUM_ROWS_PLANAR; j++)
    {
        for (byte i = 0; i < NUM_COLS_PLANAR; i++)
        {
            // Calculate rotated coordinates
            float dx = i - centerX;
            float dy = j - centerY;

            float rotX = dx * cos(angle) - dy * sin(angle) + centerX;
            float rotY = dx * sin(angle) + dy * cos(angle) + centerY;

            // Ensure coordinates stay in bounds
            rotX = constrain(rotX, 0, NUM_COLS_PLANAR - 1);
            rotY = constrain(rotY, 0, NUM_ROWS_PLANAR - 1);

            int ledsindex = XY_fibon_PLANAR(i, j);
            if (ledsindex == g_lastSafeIndex)
                continue;

            // Use rotated coordinates for color calculation
            CRGB newcolor = ColorFromPalette(gCurrentPalette,
                                             ((uint8_t)rotX << 3) + ((uint8_t)rotY << 3) + animTime16(1), 255);
            ledBuffer[ledsindex] = newcolor;
        }
    }
}

// fire2021_____________________________________

void fire2021(CRGB *ledBuffer)
{
    // we don't want to use the g_timeAccumulator here,
    // because the fire simulation doesn't look as good, when slowed or sped up.
    int a = millis();
    int a1 = a / 3;
    for (byte j = 0; j < NUM_ROWS_PLANAR; j++)
    {
        for (byte i = 0; i < NUM_COLS_PLANAR; i++)
        {
            int ledsindex = XY_fibon_PLANAR(i, j);
            if (ledsindex != g_lastSafeIndex)
                ledBuffer[ledsindex] = HeatColor(qsub8(inoise8(i * 90, j * 90 + a, a1),
                                                       abs8(j - (NUM_ROWS_PLANAR - 1)) * 255 / (NUM_ROWS_PLANAR + 4)));
        }
    }
}

// Distortion_Waves_plan_____________________________________

void Distortion_Waves_planar(CRGB *ledBuffer)
{
    byte speed = 5;
    uint8_t scale = 2;

    uint16_t a = animTime16(0.333);
    uint16_t a2 = a / 2;
    uint16_t a3 = a / 3;

    uint16_t cx = beatsin8(10 - speed, 0, NUM_COLS_PLANAR) * scale;
    uint16_t cy = beatsin8(12 - speed, 0, NUM_ROWS_PLANAR) * scale;
    uint16_t cx1 = beatsin8(13 - speed, 0, NUM_COLS_PLANAR) * scale;
    uint16_t cy1 = beatsin8(15 - speed, 0, NUM_ROWS_PLANAR) * scale;
    uint16_t cx2 = beatsin8(17 - speed, 0, NUM_COLS_PLANAR) * scale;
    uint16_t cy2 = beatsin8(14 - speed, 0, NUM_ROWS_PLANAR) * scale;

    uint16_t xoffs = 0;

    for (int x = 0; x < NUM_COLS_PLANAR; x++)
    {
        xoffs += scale;
        uint16_t yoffs = 0;

        for (int y = 0; y < NUM_ROWS_PLANAR; y++)
        {
            uint16_t index = XY_fibon_PLANAR(x, y);
            yoffs += scale;

            byte rdistort = cos_wave[(cos_wave[((x << 3) + a) & 255] + cos_wave[((y << 3) - a2) & 255] + a3) & 255] >> 1;
            byte gdistort = cos_wave[(cos_wave[((x << 3) - a2) & 255] + cos_wave[((y << 3) + a3) & 255] + a + 32) & 255] >> 1;
            byte bdistort = cos_wave[(cos_wave[((x << 3) + a3) & 255] + cos_wave[((y << 3) - a) & 255] + a2 + 64) & 255] >> 1;

            byte valueR = rdistort + (a - (((xoffs - cx) * (xoffs - cx) + (yoffs - cy) * (yoffs - cy)) >> 7));
            byte valueG = gdistort + (a2 - (((xoffs - cx1) * (xoffs - cx1) + (yoffs - cy1) * (yoffs - cy1)) >> 7));
            byte valueB = bdistort + (a3 - (((xoffs - cx2) * (xoffs - cx2) + (yoffs - cy2) * (yoffs - cy2)) >> 7));

            valueR = cos_wave[valueR];
            valueG = cos_wave[valueG];
            valueB = cos_wave[valueB];

            CRGB newcolor = ColorFromPalette(gCurrentPalette, (valueR + valueG + valueB) / 3, 255);
            ledBuffer[index] = newcolor;
        }
    }
    GammaCorrection(ledBuffer);
}

// RGB_hiphotic_____________________________________

void RGB_hiphotic(CRGB *ledBuffer)
{
    uint16_t a = animTime16(1.33);
    for (int x = 0; x < NUM_COLS_PLANAR; x++)
    {
        for (int y = 0; y < NUM_ROWS_PLANAR; y++)
        {
            int index = XY_fibon_PLANAR(x, y);
            ledBuffer[index].b = sin8((x - 8) * cos8((y + 20) * 4) / 4 + a);
            ledBuffer[index].g = (sin8(x * 16 + a / 3) + cos8(y * 8 + a / 2)) / 2;
            ledBuffer[index].r = sin8(cos8(x * 8 + a / 3) + sin8(y * 8 + a / 4) + a);
        }
    }
    GammaCorrection(ledBuffer);
}

// PlasmaBall_____________________________________

static void mydrawLine_PB(byte x1, byte y1, CRGB *ledBuffer)
{ // draw line frim center
    CRGB color;
    byte xsteps = abs8(NUM_COLS_PLANAR / 2 - x1) + 1;
    byte ysteps = abs8(NUM_ROWS_PLANAR / 2 - y1) + 1;
    byte steps = xsteps >= ysteps ? xsteps : ysteps;

    for (uint16_t i = 1; i <= steps; i++)
    {
        byte dx = lerp8by8(NUM_COLS_PLANAR / 2, x1, i * 255 / steps);
        byte dy = lerp8by8(NUM_ROWS_PLANAR / 2, y1, i * 255 / steps);
        uint16_t index = XY_fibon_PLANAR(dx, dy);
        color = CHSV(240 - i * 8, 255, 255);
        nblend(ledBuffer[index], color, 64);
        ledBuffer[index] %= (i * 255 / steps); // for draw gradient line
    }
}

void PlasmaBall(CRGB *ledBuffer)
{
    int8_t speed = (int)fmap(g_animationSpeed, MIN_ANIMATION_SPEED, MAX_ANIMATION_SPEED, 0, 10);

    byte x1 = beatsin8(18 + speed, 0, (NUM_COLS_PLANAR - 1));
    byte x2 = beatsin8(23 + speed, 0, (NUM_COLS_PLANAR - 1));
    byte x3 = beatsin8(27 + speed, 0, (NUM_COLS_PLANAR - 1));
    byte x4 = beatsin8(31 + speed, 0, (NUM_COLS_PLANAR - 1));
    byte x5 = beatsin8(33 + speed, 0, (NUM_COLS_PLANAR - 1));
    byte x6 = beatsin8(25 + speed, 0, (NUM_COLS_PLANAR - 1));

    byte y1 = beatsin8(20 + speed, 0, (NUM_ROWS_PLANAR - 1));
    byte y2 = beatsin8(26 + speed, 0, (NUM_ROWS_PLANAR - 1));
    byte y3 = beatsin8(15 + speed, 0, (NUM_ROWS_PLANAR - 1));
    byte y4 = beatsin8(27 + speed, 0, (NUM_ROWS_PLANAR - 1));
    byte y5 = beatsin8(30 + speed, 0, (NUM_ROWS_PLANAR - 1));
    byte y6 = beatsin8(19 + speed, 0, (NUM_ROWS_PLANAR - 1));

    fadeToBlackBy(ledBuffer, NUM_LEDS, 15);

    mydrawLine_PB(x1, y1, ledBuffer);
    mydrawLine_PB(x2, y2, ledBuffer);
    mydrawLine_PB(x3, y3, ledBuffer);
    mydrawLine_PB(x4, y4, ledBuffer);
    mydrawLine_PB(x5, y5, ledBuffer);
    mydrawLine_PB(x6, y6, ledBuffer);
}

// Fire comets_____________________________________

DEFINE_GRADIENT_PALETTE(firepal){
    // define fire palette
    0, 0, 0, 0, // black
    50, 0, 0, 0,
    64, 255, 0, 0,      // red
    100, 255, 255, 0,   // yellow
    255, 255, 255, 255, // white
};

static uint16_t buffXY(byte x, byte y)
{
    return (y * (NUM_COLS_PLANAR + 2) + x);
}

static void fadecenter()
{
    uint16_t sum;
    for (byte x = 1; x < NUM_COLS_PLANAR + 1; x++)
    {
        for (byte y = 1; y < NUM_ROWS_PLANAR + 1; y++)
        {
            sum = cometHeat[buffXY(x, y)];
            sum += cometHeat[buffXY(x + 1, y)];
            sum += cometHeat[buffXY(x - 1, y)];
            sum += cometHeat[buffXY(x, y + 1)];
            sum += cometHeat[buffXY(x, y - 1)];
            sum /= 5;
            cometHeat[buffXY(x, y)] = sum;
        }
    }
}

static void balls()
{
    uint8_t speed = (int)fmap(g_animationSpeed, MIN_ANIMATION_SPEED, MAX_ANIMATION_SPEED, 0, 20);
    // byte speed = 10;
    byte bright = 200;

    byte x = beatsin8(10 + speed, 2, NUM_COLS_PLANAR - 2);
    byte y = beatsin8(15 + speed, 2, NUM_ROWS_PLANAR - 1);
    byte x1 = beatsin8(12 + speed, 2, NUM_COLS_PLANAR - 2);
    byte y1 = beatsin8(9 + speed, 2, NUM_ROWS_PLANAR - 1);
    byte x2 = beatsin8(15 + speed, 2, NUM_COLS_PLANAR - 2);
    byte y2 = beatsin8(11 + speed, 2, NUM_ROWS_PLANAR - 1);

    cometHeat[buffXY(x, y)] = bright;
    cometHeat[buffXY(x + 1, y)] = bright;
    cometHeat[buffXY(x1, y1)] = bright;
    cometHeat[buffXY(x1 + 1, y1)] = bright;
    cometHeat[buffXY(x2, y2)] = bright;
    cometHeat[buffXY(x2 + 1, y2)] = bright;
}

static void toLeds(CRGB *ledBuffer)
{
    CRGBPalette256 myPal = firepal;

    uint16_t buffIndex = (NUM_COLS_PLANAR + 3);
    for (int y = 0; y < NUM_ROWS_PLANAR; y++)
    {
        for (int x = 0; x < NUM_COLS_PLANAR; x++)
        {
            int index = XY_fibon_PLANAR(x, (NUM_ROWS_PLANAR - 1) - y);
            CRGB color = ColorFromPalette(myPal, cometHeat[buffIndex], 255);
            nblend(ledBuffer[index], color, 16);
            // ledBuffer[index] = color;
            buffIndex++;
        }
        buffIndex += 2;
    }
}

void FireComets(CRGB *ledBuffer)
{
    balls();
    fadecenter();
    toLeds(ledBuffer);
}

// F_lying_____________________________________

static void mydrawLine_Fl(byte x, byte y, byte x1, byte y1, CRGB color, bool dot, CRGB *ledBuffer)
{ // my ugly line draw function )))

    byte xsteps = abs8(x - x1) + 1;
    byte ysteps = abs8(y - y1) + 1;
    byte steps = xsteps >= ysteps ? xsteps : ysteps;

    for (byte i = 1; i <= steps; i++)
    {
        byte dx = lerp8by8(x, x1, i * 255 / steps);
        byte dy = lerp8by8(y, y1, i * 255 / steps);
        int index = XY_fibon_PLANAR(dx, dy);
        if (index >= g_lastSafeIndex)
            continue;
        ledBuffer[index] = color; // change to += for brightness look
    }

    if (dot)
    { // add white point at the ends of line
        ledBuffer[XY_fibon_PLANAR(x, y)] = CRGB ::White;
        ledBuffer[XY_fibon_PLANAR(x1, y1)] = CRGB ::White;
    }
}

void F_lying(CRGB *ledBuffer)
{
    static byte hue = 0;
    EVERY_N_MILLISECONDS(30) { hue++; } // 30 - speed of hue change

    byte x1 = beatsin8(18, 0, (NUM_COLS_PLANAR - 1));
    byte x2 = beatsin8(23, 0, (NUM_COLS_PLANAR - 1));
    byte x3 = beatsin8(27, 0, (NUM_COLS_PLANAR - 1));
    byte x4 = beatsin8(31, 0, (NUM_COLS_PLANAR - 1));
    byte x5 = beatsin8(33, 0, (NUM_COLS_PLANAR - 1));

    byte y1 = beatsin8(20, 0, (NUM_ROWS_PLANAR - 1));
    byte y2 = beatsin8(26, 0, (NUM_ROWS_PLANAR - 1));
    byte y3 = beatsin8(15, 0, (NUM_ROWS_PLANAR - 1));
    byte y4 = beatsin8(27, 0, (NUM_ROWS_PLANAR - 1));
    byte y5 = beatsin8(30, 0, (NUM_ROWS_PLANAR - 1));

    CRGB color = CHSV(hue, 255, 255);

    fadeToBlackBy(ledBuffer, NUM_LEDS, 40);

    mydrawLine_Fl(x1, y1, x2, y2, color, 1, ledBuffer);
    mydrawLine_Fl(x2, y2, x3, y3, color, 1, ledBuffer);
    mydrawLine_Fl(x2, y2, x4, y4, color, 1, ledBuffer);
    mydrawLine_Fl(x3, y3, x4, y4, color, 1, ledBuffer);
    mydrawLine_Fl(x3, y3, x1, y1, color, 1, ledBuffer);
    mydrawLine_Fl(x4, y4, x1, y1, color, 1, ledBuffer);
}

// RGBTunnel_____________________________________

static byte code(int x, int y, int t)
{                                                                                                                  // put your formula here
    uint16_t outputcode = sqrt16((x -= sin8(x + t / 3) >> 4) * x + (y -= cos8(y + t >> 1) >> 4) * y) * 24 - t * 2; // moving tunnel variation
    return outputcode;
}

void RGBTunnel(CRGB *ledBuffer)
{
    uint16_t t = animTime16(0.8);

    for (byte y = 0; y < NUM_ROWS_PLANAR; y++)
    {
        for (byte x = 0; x < NUM_COLS_PLANAR; x++)
        {
            int ledindex = XY_fibon_PLANAR(x, y);
            if (ledindex == g_lastSafeIndex)
                continue;

            CRGB newcolor = ColorFromPalette(gCurrentPalette, code(x, y, t), 255);
            nblend(ledBuffer[ledindex], newcolor, 32);
        }
    }
}

//_________________________ cylindrical map patterns
// TODO: Too dark!
// RGB_Caleidoscope1_____________________________________

void RGB_Caleidoscope1(CRGB *ledBuffer)
{
    uint16_t a = animTime16(1);

    for (int j = 0; j < NUM_ROWS_CYLINDER; j++)
    {
        for (int i = 0; i < NUM_COLS_CYLINDER; i++)
        {
            uint16_t index = XY_CYLINDER(i, j);
            if (index == g_lastSafeIndex)
                continue;

            byte colorIndex = (sin8(i * 16 + a) + cos8(j * 16 + a / 2)) / 2;
            CRGB newcolor = ColorFromPalette(gCurrentPalette, colorIndex, 255);

            ledBuffer[index] = newcolor;
        }
    }
    GammaCorrection(ledBuffer);
}

// RGB_Caleidoscope2_____________________________________

void RGB_Caleidoscope2(CRGB *ledBuffer)
{
    uint16_t a = animTime16(1);

    for (int j = 0; j < NUM_ROWS_CYLINDER; j++)
    {
        for (int i = 0; i < NUM_COLS_CYLINDER; i++)
        {
            uint16_t index = XY_CYLINDER(i, j);
            if (index == g_lastSafeIndex)
                continue;

            byte colorIndex = (sin8(i * 28 + a) + cos8(j * 28 + a)) >> 1;
            CRGB newcolor = ColorFromPalette(gCurrentPalette, colorIndex, 255);

            ledBuffer[index] = newcolor;
        }
    }
    GammaCorrection(ledBuffer);
}

// Distortion_Waves_cilindr_____________________________________

void Distortion_Waves_cylinder(CRGB *ledBuffer)
{
    byte speed = 5;
    uint8_t w = 2;
    uint8_t scale = 2;

    uint16_t a = animTime16(0.333);
    uint16_t a2 = a / 2;
    uint16_t a3 = a / 3;

    uint16_t cx = beatsin8(10 - speed, 0, NUM_COLS_CYLINDER) * scale;
    uint16_t cy = beatsin8(12 - speed, 0, NUM_ROWS_CYLINDER) * scale;
    uint16_t cx1 = beatsin8(13 - speed, 0, NUM_COLS_CYLINDER) * scale;
    uint16_t cy1 = beatsin8(15 - speed, 0, NUM_ROWS_CYLINDER) * scale;
    uint16_t cx2 = beatsin8(17 - speed, 0, NUM_COLS_CYLINDER) * scale;
    uint16_t cy2 = beatsin8(14 - speed, 0, NUM_ROWS_CYLINDER) * scale;

    uint16_t xoffs = 0;

    for (int x = 0; x < NUM_COLS_CYLINDER; x++)
    {

        xoffs += scale;
        uint16_t yoffs = 0;

        for (int y = 0; y < NUM_ROWS_CYLINDER; y++)
        {
            uint16_t index = XY_CYLINDER(x, y);
            yoffs += scale;

            if (index == g_lastSafeIndex)
                continue;

            // byte rdistort = cos_wave [((x+y)*8+a2)&255]>>1;
            // byte gdistort = cos_wave [((x+y)*8+a3+32)&255]>>1;
            // byte bdistort = cos_wave [((x+y)*8+a+64)&255]>>1;

            byte rdistort = cos_wave[(cos_wave[((x << 3) + a) & 255] + cos_wave[((y << 3) - a2) & 255] + a3) & 255] >> 1;
            byte gdistort = cos_wave[(cos_wave[((x << 3) - a2) & 255] + cos_wave[((y << 3) + a3) & 255] + a + 32) & 255] >> 1;
            byte bdistort = cos_wave[(cos_wave[((x << 3) + a3) & 255] + cos_wave[((y << 3) - a) & 255] + a2 + 64) & 255] >> 1;

            byte valueR = rdistort + w * (a - (((xoffs - cx) * (xoffs - cx) + (yoffs - cy) * (yoffs - cy)) >> 7));
            byte valueG = gdistort + w * (a2 - (((xoffs - cx1) * (xoffs - cx1) + (yoffs - cy1) * (yoffs - cy1)) >> 7));
            byte valueB = bdistort + w * (a3 - (((xoffs - cx2) * (xoffs - cx2) + (yoffs - cy2) * (yoffs - cy2)) >> 7));

            valueR = cos_wave[valueR];
            valueG = cos_wave[valueG];
            valueB = cos_wave[valueB];

            ledBuffer[index].setRGB(valueR, valueG, valueB);
        }
    }

    GammaCorrection(ledBuffer);
}

// FireButterfly_____________________________________

void FireButterfly(CRGB *ledBuffer)
{
    uint16_t a = animTime16(2.67); // 8/3 = 2.67

    for (int j = 0; j < NUM_ROWS_CYLINDER; j++)
    {
        for (int i = 0; i < NUM_COLS_CYLINDER; i++)
        {
            uint16_t index = XY_CYLINDER(i, j);
            if (index == g_lastSafeIndex)
                continue;

            ledBuffer[index] = HeatColor(qsub8(inoise8(i * 60 + a, j * 5 + a, a / 3), abs8(j - (NUM_ROWS_CYLINDER - 1)) * 255 / (NUM_ROWS_CYLINDER + 2)));
        }
    }
}

// Swirl_____________________________________
void Swirl(CRGB *ledBuffer)
{
    uint16_t a = animTime16(1.2);

    for (int j = 0; j < NUM_ROWS_CYLINDER; j++)
    {
        for (int i = 0; i < NUM_COLS_CYLINDER; i++)
        {
            uint16_t index = XY_CYLINDER(i, j);
            if (index != g_lastSafeIndex)
            {
                // ledBuffer[index].setHue(i * 24 + (sin8(j * 16 + a)) >> 1);
                byte hue = i * 56 + (a >> 2) + (sin8(j * 16 + a)) >> 1;
                nblend(ledBuffer[index], ColorFromPalette(gCurrentPalette, hue, 255), 16);
            }
        }
    }
}

// TODO add to playlist
void SwirlPlanar(CRGB *ledBuffer)
{

    uint16_t a = millis() / 6;

    for (int j = 0; j < NUM_ROWS_PLANAR; j++)
    {
        for (int i = 0; i < NUM_COLS_PLANAR; i++)
        {
            uint16_t index = XY_fibon_PLANAR(i, j);
            ledBuffer[index].setHue(i * 24 + (sin8(j * 16 + a)) >> 1);
        }
    }
}

// cylindrical_Pattern_____________________________________

void cylindrical_Pattern(CRGB *ledBuffer)
{
    uint16_t a = animTime16(0.5);
    float scale = (sin(a / 32 * PI / 180) * 16) + 32;

    float scale1 = 0;
    for (int i = 0; i < NUM_COLS_CYLINDER; i++)
    {
        for (int j = 0; j < NUM_ROWS_CYLINDER; j++)
        {
            uint16_t index = XY_CYLINDER(i, j);
            if (index == g_lastSafeIndex)
                continue;

            // byte hue = (sin8((i*(int)scale)+a)>>1)+(sin8((j*16)+a))>>1;
            byte hue = (sin8(i + (int)scale1 + a) + sin8((j * 16) + a)) / 2;
            nblend(ledBuffer[index], ColorFromPalette(gCurrentPalette, hue, 255), 16);
        }
        scale1 += scale;
    }
}

// Spiral_____________________________________

void Spiral(CRGB *ledBuffer)
{
    uint16_t a = animTime16(1);
    float scale = (sin(a / 32 * PI / 180) * 18) - 6;

    float scale1 = 0;
    for (int i = 0; i < NUM_COLS_CYLINDER; i++)
    {
        for (int j = 0; j < NUM_ROWS_CYLINDER; j++)
        {
            uint16_t index = XY_CYLINDER(i, j);
            if (index == g_lastSafeIndex)
                continue;

            CRGB newcolor = ColorFromPalette(gCurrentPalette, (i * 255 / (NUM_COLS_CYLINDER - 1) + j * 255 / (NUM_ROWS_CYLINDER - 1)) + (int)scale1 + a + sin16(a) / 16384, 255);
            nblend(ledBuffer[index], newcolor, 16);
        }
        scale1 += scale;
    }
}

// Spiral2_____________________________________

void Spiral2(CRGB *ledBuffer)
{
    uint16_t a = animTime16(1.33); // 8/6 = 1.33
    float scale = sin(a / 32 * PI / 180) * 12;

    float scale1 = 0;
    for (int i = 0; i < NUM_COLS_CYLINDER; i++)
    {
        for (int j = 0; j < NUM_ROWS_CYLINDER / 2; j++)
        {
            uint16_t index = XY_CYLINDER(i, j);
            if (index == g_lastSafeIndex)
                continue;

            CRGB newcolor = ColorFromPalette(gCurrentPalette, (i * 255 / (NUM_COLS_CYLINDER - 1) * 3 + j * 255 / (NUM_ROWS_CYLINDER - 1) / 8) + a + sin16(a) / 16384, 255);
            nblend(ledBuffer[index], newcolor, 16);
        }
        scale1 += scale;
    }

    for (int i = 0; i < NUM_COLS_CYLINDER; i++)
    {
        for (int j = NUM_ROWS_CYLINDER / 2; j < NUM_ROWS_CYLINDER; j++)
        {
            uint16_t index = XY_CYLINDER(i, j);
            if (index == g_lastSafeIndex)
                continue;

            CRGB newcolor = ColorFromPalette(gCurrentPalette, (i * 255 / (NUM_COLS_CYLINDER - 1) * 3 - j * 255 / (NUM_ROWS_CYLINDER - 1) / 8) - a + sin16(a) / 16384, 255);
            nblend(ledBuffer[index], newcolor, 32);
        }
        scale1 += scale;
    }
}

// Flower_____________________________________

void Flower(CRGB *ledBuffer)
{
    uint16_t a = animTime16(1);

    for (int j = 0; j < NUM_ROWS_CYLINDER; j++)
    {
        for (int i = 0; i < NUM_COLS_CYLINDER; i++)
        {
            uint16_t index = XY_CYLINDER(i, j);
            if (index == g_lastSafeIndex)
                continue;
            CRGB newcolor = ColorFromPalette(gCurrentPalette, (j * 255 / (NUM_ROWS_CYLINDER - 1) + sin8((i * 8 + a)) + sin8(i * 30 - a) + a) / 2, 255);
            nblend(ledBuffer[index], newcolor, 16);
        }
    }
}

//_________________________ fibonacci map patterns

// pride_____________________________________

void pride(CRGB *ledBuffer)
// Pride2015
// Animated, ever-changing rainbows.
// by Mark Kriegsman
{
    static uint16_t sPseudotime = 0;
    static uint16_t sLastMillis = 0;
    static uint16_t sHue16 = 0;

    uint8_t sat8 = beatsin88(87, 220, 250);
    uint8_t brightdepth = beatsin88(341, 96, 224);
    uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
    uint8_t msmultiplier = beatsin88(147, 23, 60);

    uint16_t hue16 = sHue16; // gHue * 256;
    uint16_t hueinc16 = beatsin88(113, 1, 3000);

    uint16_t ms = animTime16(8.0);
    uint16_t deltams = ms - sLastMillis;
    sLastMillis = ms;
    sPseudotime += deltams * msmultiplier;
    sHue16 += deltams * beatsin88(400, 5, 9);
    uint16_t brightnesstheta16 = sPseudotime;

    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
        hue16 += hueinc16;
        uint8_t hue8 = hue16 / 256;

        brightnesstheta16 += brightnessthetainc16;
        uint16_t b16 = sin16(brightnesstheta16) + 32768;

        uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
        uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
        bri8 += (255 - brightdepth);

        CRGB newcolor = CHSV(hue8, sat8, bri8);

        uint16_t pixelnumber = i;
        pixelnumber = (NUM_LEDS - 1) - pixelnumber;
        // uint16_t ledindex = pgm_read_byte (FibonCilindrTable+pixelnumber);

        uint16_t ledindex = pgm_read_byte(fibonacciToPhysical + i);
        nblend(ledBuffer[ledindex], newcolor, 64);
    }
}

// colorwaves_____________________________________

// ColorWavesWithPalettes
// Animated shifting color waves, with several cross-fading color palettes.
// by Mark Kriegsman, August 2015
//
// Color palettes courtesy of cpt-city and its contributors:
//   http://soliton.vm.bytemark.co.uk/pub/cpt-city/
//
// Color palettes converted for FastLED using "PaletteKnife" v1:
//   http://fastled.io/tools/paletteknife/
//

void colorwaves(CRGB *ledBuffer)
{
    static uint16_t sPseudotime = 0;
    static uint16_t sLastMillis = 0;
    static uint16_t sHue16 = 0;

    uint8_t sat8 = beatsin88(87, 220, 250);
    uint8_t brightdepth = beatsin88(341, 96, 224);
    uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
    uint8_t msmultiplier = beatsin88(147, 23, 60);

    uint16_t hue16 = sHue16; // gHue * 256;
    uint16_t hueinc16 = beatsin88(113, 300, 1500);

    uint16_t ms = animTime16(8.0);
    uint16_t deltams = ms - sLastMillis;
    sLastMillis = ms;
    sPseudotime += deltams * msmultiplier;
    sHue16 += deltams * beatsin88(400, 5, 9);
    uint16_t brightnesstheta16 = sPseudotime;

    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
        hue16 += hueinc16;
        uint8_t hue8 = hue16 / 256;
        uint16_t h16_128 = hue16 >> 7;
        if (h16_128 & 0x100)
        {
            hue8 = 255 - (h16_128 >> 1);
        }
        else
        {
            hue8 = h16_128 >> 1;
        }

        brightnesstheta16 += brightnessthetainc16;
        uint16_t b16 = sin16(brightnesstheta16) + 32768;

        uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
        uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
        bri8 += (255 - brightdepth);

        uint8_t index = hue8;
        // index = triwave8( index);
        index = scale8(index, 240);

        CRGB newcolor = ColorFromPalette(gCurrentPalette, index, bri8);

        uint16_t pixelnumber = i;
        pixelnumber = (NUM_LEDS - 1) - pixelnumber;

        byte ledindex = pgm_read_byte(fibonacciToPhysical + pixelnumber);
        nblend(ledBuffer[ledindex], newcolor, 64);
    }
}

//_________________________ non map patterns

// SoftTwinkles_____________________________________

void SoftTwinkles(CRGB *ledBuffer)
{
    static const CRGB lightcolor(0, 4, 4);
    static const CRGB darkColor(0, 2, 2);

    if (patternNeedsInit(ledBuffer))
    {
        fill_solid(ledBuffer, NUM_LEDS, CRGB::Black);
    }

    for (int i = 0; i < NUM_LEDS; i++)
    {
        if (!ledBuffer[i])
            continue; // skip black pixels
        if (ledBuffer[i].b & 1)
        {                              // is red odd?
            ledBuffer[i] -= darkColor; // darken if red is odd
        }
        else
        {
            ledBuffer[i] += lightcolor; // brighten if red is even
        }
    }

    int j = random16(NUM_LEDS);
    if (!ledBuffer[j])
    {
        ledBuffer[j].b = 2;
    }
    j = random16(NUM_LEDS);
    if (ledBuffer[j].r & 1)
        ledBuffer[j].b -= 1;
}
