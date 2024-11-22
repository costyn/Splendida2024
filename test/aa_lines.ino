#include <FastLED.h>

#define DATA_PIN    26                //set your datapin
#define LED_TYPE    WS2812B          //leds type
#define COLOR_ORDER GRB              //color order of leds

#define NUM_COLS_PLANAR 20            // resolution for planar lookup table
#define NUM_ROWS_PLANAR 20            // resolution for planar lookup table
#define NUM_COLS_SPIRALS 21           // resolution for spirals lookup table
#define NUM_ROWS_SPIRALS 13           // resolution for spirals lookup table
#define NUM_ROWS_CILINDR 11           // resolution for cinindrical lookup table
#define NUM_COLS_CILINDR 45           // resolution for cilindrical lookup table
#define NUM_LEDS_PLANAR  NUM_ROWS_PLANAR * NUM_COLS_PLANAR 
#define NUM_LEDS_SPIRALS NUM_ROWS_SPIRALS* NUM_COLS_SPIRALS  //not used yet. in future
#define NUM_LEDS_CILINDR NUM_ROWS_CILINDR* NUM_COLS_CILINDR //not used yet. in future

#define WIDTH 21
#define HEIGHT 21
#define NUM_LEDS  NUM_LEDS_SPIRALS
// #define NUM_LEDS (WIDTH * HEIGHT)

CRGB leds[NUM_LEDS + 1];
CRGBPalette16 currentPalette = {
  0xFF0000, 0x7F0000, 0xAB5500, 0x552A00, 0xABAB00, 0x555500, 0x00FF00, 0x007F00,
  0x00AB55, 0x00552A, 0x0000FF, 0x00007F, 0x5500AB, 0x2A0055, 0xAB0055, 0x55002A
};


static const uint16_t FibonPlanarTable[] PROGMEM ={    //lookup table for planar mapping on fibonacci layout
256,256,256,256,256,256,256,256,36,39,38,37,256,256,256,256,256,256,256,256,256,256,256,256,256,13,34,35,40,
256,58,59,60,61,256,256,256,256,256,256,256,256,256,256,14,33,256,41,56,57,68,67,66,65,64,63,256,256,256,256,
256,256,256,12,15,32,42,55,256,69,256,79,80,81,82,83,62,256,256,256,256,256,11,16,31,256,43,54,70,77,78,94,
93,92,91,90,84,85,256,256,256,255,10,17,30,44,53,71,76,256,95,256,101,102,103,104,89,88,256,256,256,254,9,18,
29,45,52,72,75,96,256,100,120,119,118,117,105,106,87,256,256,253,8,19,28,46,256,51,73,97,99,121,124,125,126,
256,116,256,107,86,232,252,7,20,256,27,47,256,50,74,122,123,145,144,256,127,256,115,256,108,233,251,6,256,21,
256,26,48,49,256,98,146,147,148,143,256,128,256,114,109,231,234,250,5,256,22,23,25,24,0,195,171,170,169,149,
142,256,129,113,110,230,235,256,249,4,3,2,1,244,243,256,194,172,256,168,150,141,130,112,256,256,229,236,256,
248,247,246,245,242,220,219,196,193,173,167,151,140,131,111,256,256,208,228,237,238,239,240,241,256,221,218,
197,192,174,166,152,139,132,256,256,256,256,209,227,226,256,256,256,222,256,217,198,191,175,165,153,138,133,
256,256,256,256,207,210,211,225,224,223,215,216,199,256,190,176,164,154,137,134,256,256,256,256,256,206,205,
212,213,214,201,200,256,189,177,163,256,155,136,256,256,256,256,256,256,256,256,204,203,202,256,187,188,178,
162,256,156,135,256,256,256,256,256,256,256,256,256,183,184,185,186,180,179,161,256,157,256,256,256,256,256,
256,256,256,256,256,256,256,256,182,181,159,160,256,158,256,256,256,256,256,256,256 
};  

static const uint16_t Fibon21Spirals[] PROGMEM ={    //lookup table for spirals mapping on fibonacci layout
255, 12, 13, 36, 37, 61, 62, 85, 86, 110, 111, 134, 135, 158, 159, 182, 183, 207, 208, 231, 232,
254, 11, 14, 35, 38, 60, 63, 84, 87, 109, 112, 133, 136, 157, 160, 181, 184, 206, 209, 230, 233,
253, 10, 15, 34, 39, 59, 64, 83, 88, 108, 113, 132, 137, 156, 161, 180, 185, 205, 210, 229, 234,
252, 9, 16, 33, 40, 58, 65, 82, 89, 107, 114, 131, 138, 155, 162, 179, 186, 204, 211, 228, 235,
251, 8, 17, 32, 41, 57, 66, 81, 90, 106, 115, 130, 139, 154, 163, 178, 187, 203, 212, 227, 236,
250, 7, 18, 31, 42, 56, 67, 80, 91, 105, 116, 129, 140, 153, 164, 177, 188, 202, 213, 226, 237,
249, 6, 19, 30, 43, 55, 68, 79, 92, 104, 117, 128, 141, 152, 165, 176, 189, 201, 214, 225, 238,
248, 5, 20, 29, 44, 54, 69, 78, 93, 103, 118, 127, 142, 151, 166, 175, 190, 200, 215, 224, 239,
247, 4, 21, 28, 45, 53, 70, 77, 94, 102, 119, 126, 143, 150, 167, 174, 191, 199, 216, 223, 240,
246, 3, 22, 27, 46, 52, 71, 76, 95, 101, 120, 125, 144, 149, 168, 173, 192, 198, 217, 222, 241,
245, 2, 23, 26, 47, 51, 72, 75, 96, 100, 121, 124, 145, 148, 169, 172, 193, 197, 218, 221, 242,
244, 1, 24, 25, 48, 50, 73, 74, 97, 99, 122, 123, 256, 147, 170, 256, 194, 196, 219, 220, 243,
0, 256, 256, 256, 49, 256, 256, 256, 98, 256, 256, 146, 256, 256, 171, 256, 195, 256, 256, 256, 256
};  

void setup() {
  FastLED.addLeds<LED_TYPE, 26, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
}

uint16_t XY(uint8_t x, uint8_t y) {
  if (x >= WIDTH) return NUM_LEDS;
  if (y >= HEIGHT) return NUM_LEDS;
  return y * WIDTH + x;
}


void loop()
{
  uint32_t ms = millis();

  // draw the background animation, just like the XYmatrix example
  // but with added distortion
  uint32_t yHueDelta = (int32_t)sin16(ms * 6) * 4;
  uint32_t xHueDelta = (int32_t)cos16(ms * 6) * 4;
  uint32_t startHue = ms << 8;
  uint32_t lineStartHue = startHue - (HEIGHT + 1) / 2 * yHueDelta;
  int16_t yd2 = sin16(ms * 3) / 4;
  int16_t xd2 = sin16(ms * 7) / 4;
  for (byte y = 0; y < HEIGHT; y++) {
    uint32_t pixelHue = lineStartHue - (WIDTH + 1) / 2 * xHueDelta;
    uint32_t xhd = xHueDelta;
    lineStartHue += yHueDelta;
    yHueDelta += yd2;
    for (byte x = 0; x < WIDTH; x++) {
      leds[XY_fibon_21spirals(x, y)] = ColorFromPaletteExtended(currentPalette, pixelHue >> 7, 255, LINEARBLEND);
      pixelHue += xhd;
      xhd += xd2;
    }
  }

   // change `effect` every second repeating: 0,1,2,3,4,5,0,1,2... 
  static uint8_t effect = 0;
  EVERY_N_MILLIS(1000) {
    if (++effect > 5) effect = 0;
  }


  FastLED.show();
}

// from: https://github.com/FastLED/FastLED/pull/202
CRGB ColorFromPaletteExtended(const CRGBPalette16& pal, uint16_t index, uint8_t brightness, TBlendType blendType) {
  // Extract the four most significant bits of the index as a palette index.
  uint8_t index_4bit = (index >> 12);
  // Calculate the 8-bit offset from the palette index.
  uint8_t offset = (uint8_t)(index >> 4);
  // Get the palette entry from the 4-bit index
  const CRGB* entry = &(pal[0]) + index_4bit;
  uint8_t red1   = entry->red;
  uint8_t green1 = entry->green;
  uint8_t blue1  = entry->blue;

  uint8_t blend = offset && (blendType != NOBLEND);
  if (blend) {
    if (index_4bit == 15) {
      entry = &(pal[0]);
    } else {
      entry++;
    }

    // Calculate the scaling factor and scaled values for the lower palette value.
    uint8_t f1 = 255 - offset;
    red1   = scale8_LEAVING_R1_DIRTY(red1,   f1);
    green1 = scale8_LEAVING_R1_DIRTY(green1, f1);
    blue1  = scale8_LEAVING_R1_DIRTY(blue1,  f1);

    // Calculate the scaled values for the neighbouring palette value.
    uint8_t red2   = entry->red;
    uint8_t green2 = entry->green;
    uint8_t blue2  = entry->blue;
    red2   = scale8_LEAVING_R1_DIRTY(red2,   offset);
    green2 = scale8_LEAVING_R1_DIRTY(green2, offset);
    blue2  = scale8_LEAVING_R1_DIRTY(blue2,  offset);
    cleanup_R1();

    // These sums can't overflow, so no qadd8 needed.
    red1   += red2;
    green1 += green2;
    blue1  += blue2;
  }
  if (brightness != 255) {
    // nscale8x3_video(red1, green1, blue1, brightness);
    nscale8x3(red1, green1, blue1, brightness);
  }
  return CRGB(red1, green1, blue1);
}


uint16_t XY_fibon(byte x, byte y) {             // calculate index in leds from XY coordinates for planar mapping
uint16_t ledsindex = pgm_read_word (FibonPlanarTable+y*NUM_COLS_PLANAR+x);
return (ledsindex);
}

uint16_t XY_fibon_21spirals(byte x, byte y) {   // calculate index in leds from XY coordinates for spirals mapping
uint16_t ledsindex = pgm_read_word (Fibon21Spirals+y*NUM_COLS_SPIRALS+x);
return (ledsindex);
}
