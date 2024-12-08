#ifndef PATTERNS_H
#define PATTERNS_H

// system procedures_____________________________________

uint16_t XY_fibon_PLANAR(byte x, byte y)
{
  uint16_t ledsindex = pgm_read_word(FibonPlanarTable + y * NUM_COLS_PLANAR + x);
  return (ledsindex);
}

uint16_t XY_CYLINDER(byte x, byte y)
{ // calculate index in leds from XY coordinates for cylindrical mapping
  uint16_t ledsindex = pgm_read_word(FibonCilindrTable + y * NUM_COLS_CYLINDER + x);
  return (ledsindex);
}

void GammaCorrection()
{ // gamma correction function
  byte r, g, b;
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    r = leds[i].r;
    g = leds[i].g;
    b = leds[i].b;
    leds[i].r = pgm_read_byte(exp_gamma + r);
    leds[i].g = pgm_read_byte(exp_gamma + g);
    leds[i].b = pgm_read_byte(exp_gamma + b);
  }
}

// Original -> g_timeAccumulator conversion:

// millis() / 3 (faster)
// uint16_t a = (uint16_t)(g_timeAccumulator * 2.67);  // 8/3 = 2.67

// // millis() / 7 (slightly faster)
// uint16_t a = (uint16_t)(g_timeAccumulator * 1.14);  // 8/7 = 1.14

// // millis() / 12 (slower)
// uint16_t a = (uint16_t)(g_timeAccumulator * 0.67);  // 8/12 = 0.67

// // straight millis()
// uint16_t ms = (uint16_t)(g_timeAccumulator * 8.0);  // Multiply by 8 to get back to original speed

// // Standardized replacements:
// // For /6 (faster):
// uint16_t a = (uint16_t)(g_timeAccumulator * 1.33); // 8/6 = 1.33

// // For /8 (standard):
// uint16_t a = (uint16_t)g_timeAccumulator;

// // For >>3 (same as /8):
// uint16_t a = (uint16_t)g_timeAccumulator;

// Original: millis() / 24 (slower than standard /8)
// Factor: 8/24 = 0.333 (one-third speed)

// uint16_t a = (uint16_t)(g_timeAccumulator * 0.333);

//_________________________ planar map patterns

// DigitalRain_____________________________________

void changepattern()
{
  int rand1 = random16(NUM_LEDS_PLANAR);
  int rand2 = random16(NUM_LEDS_PLANAR);
  if (rain[rand1] && !rain[rand2])
  {
    rain[rand1] = 0;
    rain[rand2] = 1;
  }
} // changepattern

void raininit()
{ // init array of dots. run once
  for (int i = 0; i < NUM_LEDS_PLANAR; i++)
    rain[i] = 0;
  byte rainNumb = random8(20, 30); // 5..8 how many dots with some random variation
  for (int i = 0; i < rainNumb; i++)
    rain[random16(NUM_LEDS_PLANAR)] = 1;
} // raininit

void updaterain()
{
  static int speed = 1;

  for (byte j = 0; j < NUM_ROWS_PLANAR; j++)
  {
    int yindex = (j + speed) % NUM_ROWS_PLANAR * NUM_COLS_PLANAR;
    for (byte i = 0; i < NUM_COLS_PLANAR; i++)
    {
      byte layer = rain[yindex + i];
      if (layer)
        leds[XY_fibon_PLANAR((NUM_COLS_PLANAR - 1) - i, (NUM_ROWS_PLANAR - 1) - j)].setHue(100);
    }
  }

  fadeToBlackBy(leds, NUM_LEDS, 70);
  speed++;
} // updaterain

void DigitalRain()
{
  if (g_patternInitNeeded)
  {
    raininit();
    g_patternInitNeeded = 0;
    FastLED.clear();
  }
  EVERY_N_MILLISECONDS(80) { updaterain(); }
  EVERY_N_MILLISECONDS(15) { changepattern(); }
}

void DiagonalPattern()
{
  // Get rotation angle from time
  float angle = g_timeAccumulator * 0.001; // Adjust speed multiplier as needed
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
                                       ((uint8_t)rotX << 3) + ((uint8_t)rotY << 3) + (int)g_timeAccumulator, 255);
      leds[ledsindex] = newcolor;
    }
  }
}

// fire2021_____________________________________

void fire2021()
{
  int a = (int)(g_timeAccumulator * 8.0); // Multiply by 8 to get back to original speed
  int a1 = a / 3;
  for (byte j = 0; j < NUM_ROWS_PLANAR; j++)
  {
    for (byte i = 0; i < NUM_COLS_PLANAR; i++)
    {
      int ledsindex = XY_fibon_PLANAR(i, j);
      if (ledsindex != g_lastSafeIndex)
        leds[ledsindex] = HeatColor(qsub8(inoise8(i * 90, j * 90 + a, a1),
                                          abs8(j - (NUM_ROWS_PLANAR - 1)) * 255 / (NUM_ROWS_PLANAR + 4)));
    }
  }
}

// Distortion_Waves_plan_____________________________________

void Distortion_Waves_planar()
{
  byte speed = 5;
  uint8_t scale = 2;

  uint16_t a = (uint16_t)(g_timeAccumulator * 0.333);
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
      leds[index] = newcolor;
    }
  }
  GammaCorrection();
}

// RGB_hiphotic_____________________________________

void RGB_hiphotic()
{
  uint16_t a = (uint16_t)(g_timeAccumulator * 1.33);
  for (int x = 0; x < NUM_COLS_PLANAR; x++)
  {
    for (int y = 0; y < NUM_ROWS_PLANAR; y++)
    {
      int index = XY_fibon_PLANAR(x, y);
      leds[index].b = sin8((x - 8) * cos8((y + 20) * 4) / 4 + a);
      leds[index].g = (sin8(x * 16 + a / 3) + cos8(y * 8 + a / 2)) / 2;
      leds[index].r = sin8(cos8(x * 8 + a / 3) + sin8(y * 8 + a / 4) + a);
    }
  }
  GammaCorrection();
}

// PlasmaBall_____________________________________

void mydrawLine_PB(byte x1, byte y1)
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
    nblend(leds[index], color, 64);
    leds[index] %= (i * 255 / steps); // for draw gradient line
  }
}

void PlasmaBall()
{
  int8_t speed = 0;

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

  fadeToBlackBy(leds, NUM_LEDS, 15);

  mydrawLine_PB(x1, y1);
  mydrawLine_PB(x2, y2);
  mydrawLine_PB(x3, y3);
  mydrawLine_PB(x4, y4);
  mydrawLine_PB(x5, y5);
  mydrawLine_PB(x6, y6);
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

uint16_t buffXY(byte x, byte y)
{
  return (y * (NUM_COLS_PLANAR + 2) + x);
}

void fadecenter()
{
  uint16_t sum;
  for (byte x = 1; x < NUM_COLS_PLANAR + 1; x++)
  {
    for (byte y = 1; y < NUM_ROWS_PLANAR + 1; y++)
    {
      sum = rain[buffXY(x, y)];
      sum += rain[buffXY(x + 1, y)];
      sum += rain[buffXY(x - 1, y)];
      sum += rain[buffXY(x, y + 1)];
      sum += rain[buffXY(x, y - 1)];
      sum /= 5;
      rain[buffXY(x, y)] = sum;
    }
  }
}

void balls()
{
  byte speed = 10;
  byte bright = 200;

  byte x = beatsin8(10 + speed, 2, NUM_COLS_PLANAR - 2);
  byte y = beatsin8(15 + speed, 2, NUM_ROWS_PLANAR - 1);
  byte x1 = beatsin8(12 + speed, 2, NUM_COLS_PLANAR - 2);
  byte y1 = beatsin8(9 + speed, 2, NUM_ROWS_PLANAR - 1);
  byte x2 = beatsin8(15 + speed, 2, NUM_COLS_PLANAR - 2);
  byte y2 = beatsin8(11 + speed, 2, NUM_ROWS_PLANAR - 1);

  rain[buffXY(x, y)] = bright;
  rain[buffXY(x + 1, y)] = bright;
  rain[buffXY(x1, y1)] = bright;
  rain[buffXY(x1 + 1, y1)] = bright;
  rain[buffXY(x2, y2)] = bright;
  rain[buffXY(x2 + 1, y2)] = bright;
}

void toLeds()
{
  CRGBPalette256 myPal = firepal;

  uint16_t buffIndex = (NUM_COLS_PLANAR + 3);
  for (int y = 0; y < NUM_ROWS_PLANAR; y++)
  {
    for (int x = 0; x < NUM_COLS_PLANAR; x++)
    {
      int index = XY_fibon_PLANAR(x, (NUM_ROWS_PLANAR - 1) - y);
      CRGB color = ColorFromPalette(myPal, rain[buffIndex], 255);
      nblend(leds[index], color, 16);
      // leds[index] = color;
      buffIndex++;
    }
    buffIndex += 2;
  }
}

void FireComets()
{
  if (g_patternInitNeeded)
  {
    FastLED.clear();
    g_patternInitNeeded = 0;
  }
  balls();
  fadecenter();
  toLeds();
}

// F_lying_____________________________________

void mydrawLine_Fl(byte x, byte y, byte x1, byte y1, CRGB color, bool dot)
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
    leds[index] = color; // change to += for brightness look
  }

  if (dot)
  { // add white point at the ends of line
    leds[XY_fibon_PLANAR(x, y)] = CRGB ::White;
    leds[XY_fibon_PLANAR(x1, y1)] = CRGB ::White;
  }
}

void F_lying()
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

  fadeToBlackBy(leds, NUM_LEDS, 40);

  mydrawLine_Fl(x1, y1, x2, y2, color, 1);
  mydrawLine_Fl(x2, y2, x3, y3, color, 1);
  mydrawLine_Fl(x2, y2, x4, y4, color, 1);
  mydrawLine_Fl(x3, y3, x4, y4, color, 1);
  mydrawLine_Fl(x3, y3, x1, y1, color, 1);
  mydrawLine_Fl(x4, y4, x1, y1, color, 1);
}

// RGBTunnel_____________________________________

byte code(int x, int y, int t)
{                                                                                                                // put your formula here
  uint16_t outputcode = sqrt16((x -= sin8(x + t / 3) >> 4) * x + (y -= cos8(y + t >> 1) >> 4) * y) * 24 - t * 2; // moving tunnel variation
  return outputcode;
}

void RGBTunnel()
{
  uint16_t t = (uint16_t)g_timeAccumulator;

  for (byte y = 0; y < NUM_ROWS_PLANAR; y++)
  {
    for (byte x = 0; x < NUM_COLS_PLANAR; x++)
    {
      int ledindex = XY_fibon_PLANAR(x, y);
      if (ledindex == g_lastSafeIndex)
        continue;

      CRGB newcolor = ColorFromPalette(gCurrentPalette, code(x, y, t), 255);
      nblend(leds[ledindex], newcolor, 32);
    }
  }
}

//_________________________ cylindrical map patterns

// RGB_Caleidoscope1_____________________________________

void RGB_Caleidoscope1()
{
  uint16_t a = (uint16_t)g_timeAccumulator;

  for (int j = 0; j < NUM_ROWS_CYLINDER; j++)
  {
    for (int i = 0; i < NUM_COLS_CYLINDER; i++)
    {
      uint16_t index = XY_CYLINDER(i, j);
      if (index == g_lastSafeIndex)
        continue;

      byte colorIndex = (sin8(i * 16 + a) + cos8(j * 16 + a / 2)) / 2;
      CRGB newcolor = ColorFromPalette(gCurrentPalette, colorIndex, 255);

      leds[index] = newcolor;
    }
  }
  GammaCorrection();
}

// RGB_Caleidoscope2_____________________________________

void RGB_Caleidoscope2()
{
  uint16_t a = (uint16_t)g_timeAccumulator;

  for (int j = 0; j < NUM_ROWS_CYLINDER; j++)
  {
    for (int i = 0; i < NUM_COLS_CYLINDER; i++)
    {
      uint16_t index = XY_CYLINDER(i, j);
      if (index == g_lastSafeIndex)
        continue;

      byte colorIndex = (sin8(i * 28 + a) + cos8(j * 28 + a)) >> 1;
      CRGB newcolor = ColorFromPalette(gCurrentPalette, colorIndex, 255);

      leds[index] = newcolor;
    }
  }
  GammaCorrection();
}

// Distortion_Waves_cilindr_____________________________________

void Distortion_Waves_cylinder()
{
  byte speed = 5;
  uint8_t w = 2;
  uint8_t scale = 2;

  uint16_t a = (uint16_t)(g_timeAccumulator * 0.333);
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

      leds[index].setRGB(valueR, valueG, valueB);
    }
  }

  GammaCorrection();
}

// FireButterfly_____________________________________

void FireButterfly()
{
  uint16_t a = (uint16_t)(g_timeAccumulator * 2.67); // 8/3 = 2.67

  for (int j = 0; j < NUM_ROWS_CYLINDER; j++)
  {
    for (int i = 0; i < NUM_COLS_CYLINDER; i++)
    {
      uint16_t index = XY_CYLINDER(i, j);
      if (index == g_lastSafeIndex)
        continue;

      leds[index] = HeatColor(qsub8(inoise8(i * 60 + a, j * 5 + a, a / 3), abs8(j - (NUM_ROWS_CYLINDER - 1)) * 255 / (NUM_ROWS_CYLINDER + 2)));
    }
  }
}

// Sprite_Scroll_____________________________________

void DrawOneFrameSprite(uint16_t xspeed, uint16_t yspeed, byte fract, byte *sprite, byte spr_cols, byte spr_rows)
{
  CRGB color;

  uint16_t indextable = 0;
  uint16_t colsX3 = (spr_cols << 1) + spr_cols; //*3

  for (byte j = 0; j < NUM_ROWS_CYLINDER; j++)
  {
    uint16_t yoffs = ((j + yspeed) % spr_rows) * colsX3;

    for (byte i = 0; i < NUM_COLS_CYLINDER; i++)
    {
      uint16_t index = XY_CYLINDER(i, j);

      if (index == g_lastSafeIndex)
        continue;

      uint16_t xoffs = (i + xspeed) % spr_cols;
      xoffs = (xoffs << 1) + xoffs; //*3

      byte *SpriteAdr = sprite + yoffs + xoffs;

      color.r = pgm_read_byte(SpriteAdr);
      color.g = pgm_read_byte(++SpriteAdr);
      color.b = pgm_read_byte(++SpriteAdr);

      nblend(leds[index], color, fract);
    }
  }
}

// Swirl_____________________________________
// FIXME: This animation is a bit janky
void Swirl()
{
  uint16_t a = (uint16_t)(g_timeAccumulator * 1.14); // 8/7 = 1.14

  for (int j = 0; j < NUM_ROWS_CYLINDER; j++)
  {
    for (int i = 0; i < NUM_COLS_CYLINDER; i++)
    {
      uint16_t index = XY_CYLINDER((i + a / 32) % NUM_COLS_CYLINDER, j);

      if (index == g_lastSafeIndex)
        continue;
      // leds[index].setHue(i*54+(a>>2)+(sin8(j*16+a))>>1);
      byte hue = i * 56 + (a >> 2) + (sin8(j * 16 + a)) >> 1;
      nblend(leds[index], ColorFromPalette(gCurrentPalette, hue, 255), 16);
    }
  } // end cycles
}

// GPT o1-preview refactor of Swirl with floating-point math
// Doesn't solve the issue yet, more experimentation needed
void FloatingPointSwirl()
{
  // Use a floating-point variable for 'a'
  float a = g_timeAccumulator * 1.14f; // 8/7 = 1.14

  for (int j = 0; j < NUM_ROWS_CYLINDER; j++)
  {
    for (int i = 0; i < NUM_COLS_CYLINDER; i++)
    {
      // Calculate offset with floating-point division
      float offset = a / 32.0f;

      // Compute the new x position with floating-point addition
      float newX = fmod(i + offset, NUM_COLS_CYLINDER);

      // Calculate index with proper casting
      uint16_t index = XY_CYLINDER((uint8_t)newX, j);

      if (index == g_lastSafeIndex)
        continue;

      // Compute hue using floating-point arithmetic
      float hue = i * 56.0f + (a / 4.0f) + (sin8(j * 16 + (uint8_t)a) / 2.0f);

      // Use nblend for smooth color transitions
      nblend(leds[index], ColorFromPalette(gCurrentPalette, (uint8_t)hue, 255), 16);
    }
  }
}

// cylindrical_Pattern_____________________________________

void cylindrical_Pattern()
{
  uint16_t a = (uint16_t)(g_timeAccumulator * 0.67);
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
      nblend(leds[index], ColorFromPalette(gCurrentPalette, hue, 255), 16);
    }
    scale1 += scale;
  }
}

// Spiral_____________________________________

void Spiral()
{

  uint16_t a = (uint16_t)g_timeAccumulator;
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
      nblend(leds[index], newcolor, 16);
    }
    scale1 += scale;
  }
}

// Spiral2_____________________________________

void Spiral2()
{
  if (g_patternInitNeeded)
  {
    raininit();
    g_patternInitNeeded = 0;
    FastLED.clear();
  }

  uint16_t a = (uint16_t)(g_timeAccumulator * 1.33); // 8/6 = 1.33
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
      nblend(leds[index], newcolor, 16);
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
      nblend(leds[index], newcolor, 32);
    }
    scale1 += scale;
  }
}

// Flower_____________________________________

void Flower()
{
  uint16_t a = (uint16_t)g_timeAccumulator;

  for (int j = 0; j < NUM_ROWS_CYLINDER; j++)
  {
    for (int i = 0; i < NUM_COLS_CYLINDER; i++)
    {
      uint16_t index = XY_CYLINDER(i, j);
      if (index == g_lastSafeIndex)
        continue;
      CRGB newcolor = ColorFromPalette(gCurrentPalette, (j * 255 / (NUM_ROWS_CYLINDER - 1) + sin8((i * 8 + a)) + sin8(i * 30 - a) + a) / 2, 255);
      nblend(leds[index], newcolor, 16);
    }
  }
}

//_________________________ fibonacci map patterns

// pride_____________________________________

void pride()
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

  uint16_t ms = (uint16_t)(g_timeAccumulator * 8.0);
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
    nblend(leds[ledindex], newcolor, 64);
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

void colorwaves()
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

  uint16_t ms = (uint16_t)(g_timeAccumulator * 8.0);
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
    nblend(leds[ledindex], newcolor, 64);
  }
}

//_________________________ non map patterns

// SoftTwinkles_____________________________________

void SoftTwinkles()
{
  if (g_patternInitNeeded)
  {
    FastLED.clear();
    g_patternInitNeeded = 0;
  }

  static const CRGB lightcolor(0, 4, 4);
  static const CRGB darkColor(0, 2, 2);

  for (int i = 0; i < NUM_LEDS; i++)
  {
    if (!leds[i])
      continue; // skip black pixels
    if (leds[i].b & 1)
    {                       // is red odd?
      leds[i] -= darkColor; // darken if red is odd
    }
    else
    {
      leds[i] += lightcolor; // brighten if red is even
    }
  }

  int j = random16(NUM_LEDS);
  if (!leds[j])
  {
    leds[j].b = 2;
  }
  j = random16(NUM_LEDS);
  if (leds[j].r & 1)
    leds[j].b -= 1;
}

void spiralCylinderWave()
{
  // Constants for the animation
  const float speed = 0.1;       // Controls the speed of the wave
  const float waveLength = 3.0;  // Controls the number of waves around the cylinder
  const float amplitude = 127.5; // Amplitude for brightness modulation (half of 255)

  // Time variable for animation
  static float time = 0.0;
  time += speed;

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

      // Calculate wave pattern
      float wave = sin(angle + time + ((float)y / NUM_ROWS_CYLINDER) * PI);

      // Map wave to brightness (0-255)
      uint8_t brightness = (uint8_t)((wave + 1.0f) * amplitude); // wave ranges from -1 to 1

      // Get color from palette based on y position
      uint8_t colorIndex = map(y, 0, NUM_ROWS_CYLINDER - 1, 0, 255);
      CRGB color = ColorFromPalette(gCurrentPalette, colorIndex, brightness);

      // Set the LED color
      leds[index] = color;
    }
  }
}

void testCylinderMapping()
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
      leds[index] = CHSV(hue, 255, 255);
    }
  }
}

void testCylinderMapping2()
{
  // Rotation speed
  const float rotationSpeed = 0.001;

  // Calculate rotation angle based on time
  uint8_t scaledAngle = (uint8_t)(g_timeAccumulator * 0.333); // Adjust multiplier for speed

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
      leds[index] = CHSV(hue, 255, 255);
    }
  }
}

void hypnoticSpiral()
{
  // Animation parameters
  const float speed = 0.05;      // Speed of the inward movement
  const float frequency = 0.2;   // Controls the number of waves
  const float amplitude = 127.5; // Half of 255 for brightness calculation

  // Time variable for animation
  static float time = 0.0;
  time += speed;

  // Center coordinates
  float centerX = NUM_COLS_CYLINDER / 2.0;
  float centerY = NUM_ROWS_CYLINDER / 2.0;

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

      // Debugging: Check if this is the problematic LED
      // if (index == 85)
      // {
      //   Serial.print("LED 0 at index 85 found at x: ");
      //   Serial.print(x);
      //   Serial.print(", y: ");
      //   Serial.println(y);
      // }

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
      leds[index] = color;

      // Debugging: Print calculations for index 85
      // if (index == 85)
      // {
      //   Serial.print("distance: ");
      //   Serial.print(distance);
      //   Serial.print(", wave: ");
      //   Serial.print(wave);
      //   Serial.print(", brightness: ");
      //   Serial.println(brightness);
      //   Serial.print("colorIndex: ");
      //   Serial.print(colorIndex);
      //   Serial.print(", color: ");
      //   Serial.print(color.r);
      //   Serial.print(", ");
      //   Serial.print(color.g);
      //   Serial.print(", ");
      //   Serial.println(color.b);
      // }
    }
  }
}

// void identifyLEDs()
// {
//   // Static variables to keep track of time and the current LED
//   static uint32_t lastUpdateTime = 0;
//   static uint16_t currentLEDNumber = 0;

//   // Check if one second has passed
//   if (millis() - lastUpdateTime >= 500)
//   {
//     lastUpdateTime = millis();

//     // Clear all LEDs
//     FastLED.clear();

//     // Calculate x and y from the current LED number
//     uint8_t x = currentLEDNumber % NUM_COLS_CILINDR;
//     uint8_t y = currentLEDNumber / NUM_COLS_CILINDR;

//     // Get LED index from coordinates
//     uint16_t index = XY_CYLINDER(x, y);

//     // Check if the index is valid
//     if (index != g_lastSafeIndex && index < NUM_LEDS)
//     {
//       // Light up the LED with a bright color
//       leds[index] = CRGB::White;

//       // Print LED information to the Serial Monitor
//       Serial.print("Lighting up LED ");
//       Serial.print(currentLEDNumber);
//       Serial.print(" at index ");
//       Serial.print(index);
//       Serial.print(" (x: ");
//       Serial.print(x);
//       Serial.print(", y: ");
//       Serial.print(y);
//       Serial.println(")");
//     }
//     else
//     {
//       // Handle invalid index
//       Serial.print("Invalid LED at number ");
//       Serial.print(currentLEDNumber);
//       Serial.print(" (x: ");
//       Serial.print(x);
//       Serial.print(", y: ");
//       Serial.println(y);
//     }

//     // Update the LEDs
//     FastLED.show();

//     // Move to the next LED
//     currentLEDNumber++;

//     // Reset if we've reached the end
//     if (currentLEDNumber >= NUM_COLS_CILINDR * NUM_ROWS_CYLINDER)
//     {
//       currentLEDNumber = 0;
//     }
//   }
// }

// array pointers to used patterns______________________

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = // this is list of patterns
    {
        cylindrical_Pattern,
        // identifyLEDs,
        // findCenter,
        testCylinderMapping2,
        DiagonalPattern,
        // testCylinderMapping,
        hypnoticSpiral,
        spiralCylinderWave,
        PlasmaBall,
        FireComets,
        // F_lying,  // I don't like it enough.
        RGBTunnel,
        Flower,
        Distortion_Waves_cylinder,
        colorwaves,
        Distortion_Waves_planar,
        FireButterfly,
        SoftTwinkles,
        Spiral2,
        pride,
        RGB_Caleidoscope2,
        RGB_Caleidoscope1,
        Swirl,
        RGB_hiphotic,
        Spiral,
        DigitalRain,
        fire2021,
};

const char *patternNames[] = {
    "cylindrical_Pattern",
    // "identifyLEDs",
    // "findCenter",
    "testCylinderMapping2",
    "DiagonalPattern",
    // "testCylinderMapping",
    "hypnoticSpiral",
    "spiralCylinderWave",
    "PlasmaBall",
    "FireComets",
    // "F_lying",
    "RGBTunnel",
    "Flower",
    "Distortion_Waves_cylinder",
    "colorwaves",
    "Distortion_Waves_planar",
    "FireButterfly",
    "SoftTwinkles",
    "Spiral2",
    "pride",
    "RGB_Caleidoscope2",
    "RGB_Caleidoscope1",
    "Swirl",
    "RGB_hiphotic",
    "Spiral",
    "DigitalRain",
    "fire2021"};

#endif