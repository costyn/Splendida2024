// Splendida 256 NEW demoreel with palletes
// fastled fibonacci 256 leds demo
// Yaroslaw Turbin 2021
// https://vk.com/ldirko
// https://www.reddit.com/user/ldirko/
// https://twitter.com/ldir_ko

// Costyn van Dongen 2024
// https://github.com/costyn

// controls: from start automode is enable and patterns change in loop
// one button click change pattern to next and automode is OFF
// double click change bright in loop 0..maxbright with 7 steps. not affect to Automode
// long press activate Automode ON

// TODO:
// Crossfade between patterns

#include "splendidanew.h"

// Setup function
void setup()
{
  M5.begin(true, false, true);
  pinMode(GPIO_NUM_34, INPUT);
  gpio_pulldown_dis(GPIO_NUM_23);
  gpio_pullup_dis(GPIO_NUM_23);
  pinMode(GPIO_NUM_33, INPUT);

  pinMode(BRIGHTNESS_POT_PIN, INPUT);
  pinMode(SPEED_POT_PIN, INPUT);

  initializeSerial();
  initializeLEDs();
  initializeButton();
}

// Loop function
void loop()
{
  handleButton();
  changePalette();
  blendPalette();
  readPotentiometers();

  EVERY_N_SECONDS(SECONDS_PER_PATTERN)
  {
    if (automode)
    {
      fadingBrightness = true;
      FadeOut(150);                                                                // fade out current effect
      gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns); // next effect
      printPatternAndPalette();
      InitNeeded = 1; // flag if init something need
      FadeIn(200);    // fade in current effect
      fadingBrightness = false;
    }
  }

  gPatterns[gCurrentPatternNumber](); // play current pattern
  statled[0].fadeToBlackBy(1);

  FastLED.show();
  // FastLED.delay(1000 / 60); // Add a delay to control the frame rate
}

void printPatternAndPalette()
{
  Serial.print("Pattern: ");
  Serial.print(patternNames[gCurrentPatternNumber]);
  Serial.print("\tPalette: ");
  Serial.println(paletteNames[gCurrentPaletteNumber]);
}

// Initialize Serial Communication
void initializeSerial()
{
  Serial.begin(115200);
  if (automode)
    Serial.println("automode On");
  else
    Serial.println("automode Off");
}

// Initialize LEDs
void initializeLEDs()
{
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, ATOMLED_PIN, COLOR_ORDER>(statled, 1);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MILLIAMPS);
  FastLED.clear();
  gTargetPalette = gGradientPalettes[random8(gGradientPaletteCount)]; // Choose random palette on start
}

// Initialize Button
void initializeButton()
{
  button.attachClick(oneClick);
  button.attachDoubleClick(doubleClick);
  button.attachDuringLongPress(longPress);
  // button.setDebounceTicks(80);
}

// Handle Button Events
void handleButton()
{
  button.tick();
}

// Change Palette Periodically
void changePalette()
{
  EVERY_N_SECONDS(SECONDS_PER_PALETTE)
  {
    gCurrentPaletteNumber = random8(gGradientPaletteCount);
    gTargetPalette = gGradientPalettes[gCurrentPaletteNumber];

    printPatternAndPalette();
  }
}

// Blend Current Palette to Target Palette
void blendPalette()
{
  EVERY_N_MILLISECONDS(BLEND_INTERVAL_MS)
  {
    nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, BLEND_SPEED);
  }
}

static void doubleClick()
{
  Serial.println("DoubleClicked! next bright");
  static byte brIndex = 3;
  static byte bright[] = {0, 16, 32, 64, 96, 128, 170}; // 7 steps
  brIndex = (brIndex + 1) % 7;
  splendidaBrightness = bright[brIndex];
  FastLED.setBrightness(splendidaBrightness);
  Serial.println(splendidaBrightness);
  statled[0].setHue(150);
}

static void oneClick()
{
  Serial.println("Clicked! Next pattern. automode OFF");

  gCurrentPatternNumber = (gCurrentPatternNumber + 1 + ARRAY_SIZE(gPatterns)) % ARRAY_SIZE(gPatterns); // next effect
  InitNeeded = 1;
  printPatternAndPalette();
  previousMillis = millis();
  automode = false;
  statled[0].setHue(0);
}

static void longPress()
{
  Serial.println("Long press!");
  Serial.println("AutomodeOn");
  previousMillis = currentMillis;
  automode = true;
  statled[0].setHue(100);
}

void FadeOut(byte steps)
{
  for (int i = 0; i <= steps; i++)
  {
    gPatterns[gCurrentPatternNumber]();
    byte fadeOut = lerp8by8(splendidaBrightness, 0, 255 * i / steps);
    FastLED.setBrightness(fadeOut);
    FastLED.show();
    delay(10);
  }
}

void FadeIn(byte steps)
{
  for (int i = steps + 1; i--; i >= 0)
  {
    gPatterns[gCurrentPatternNumber]();
    byte fadeOut = lerp8by8(splendidaBrightness, 0, 255 * i / steps);
    FastLED.setBrightness(fadeOut);
    FastLED.show();
    delay(10);
  }
}

void readPotentiometers()
{
  static int lastMappedBrightness = 0;

  int speedPot = analogRead(SPEED_POT_PIN);
  int brightnessPot = analogRead(BRIGHTNESS_POT_PIN);

  smoothedBrightnessPot += brightnessPot;

  int mappedBrightness = map(smoothedBrightnessPot.get_avg(), 4096, 0, 0, 150);
  if (lastMappedBrightness != mappedBrightness && !fadingBrightness)
  {
    FastLED.setBrightness(lastMappedBrightness);
    lastMappedBrightness = mappedBrightness;
    Serial.print("Setting Brightness: ");
    Serial.println(mappedBrightness);
  }

  // Serial.println(smoothedSpeed);
}
