// Splendida 256 NEW demoreel with palletes
// fastled fibonacci 256 leds demo
// Yaroslaw Turbin 2021
// https://vk.com/ldirko
// https://www.reddit.com/user/ldirko/
// https://twitter.com/ldir_ko

// Costyn van Dongen 2024
// https://github.com/costyn

// controls: from start automode is enable and patterns change in loop
// one g_patternButton click change pattern to next and automode is OFF
// double click change bright in loop 0..maxbright with 7 steps. not affect to Automode
// long press activate Automode ON

#include "splendidanew.h"
#include "encoders.h"

uint8_t g_targetBrightness = DEFAULT_BRIGHTNESS;
uint8_t g_currentBrightness = 0;
float g_animationSpeed = 0.15f;
float g_timeAccumulator = 0.0f;
uint8_t gCurrentPatternNumber = 0;
CRGB g_statusLed[1];
byte g_patternInitNeeded = 1;
uint8_t g_fadeState = FADE_NONE;
CRGB leds[NUM_LEDS];
uint8_t g_lastSafeIndex = 255;
uint8_t g_fadeStartBrightness = 0;
uint8_t g_fadeTargetBrightness = 0;
uint8_t g_fadeCurrentBrightness = 0;
CRGBPalette16 gTargetPalette = gGradientPalettes[random8(gGradientPaletteCount)]; // Choose random palette on start

Scheduler _runner;
Task _taskChangeToBrightness(10 * TASK_MILLISECOND, TASK_FOREVER, &changeToBrightness);
Task _taskRunPattern(1 * TASK_MILLISECOND, TASK_FOREVER, &runPattern);
Task _taskChangePalette(SECONDS_PER_PALETTE *TASK_SECOND, TASK_FOREVER, &changePalette);
Task _taskChangePattern(SECONDS_PER_PATTERN *TASK_SECOND, TASK_FOREVER, &changePattern);
Task _taskBlendPalette(BLEND_INTERVAL_MS *TASK_MILLISECOND, TASK_FOREVER, &blendPalette);
Task _taskFade(10 * TASK_MILLISECOND, TASK_FOREVER, &fade);
Task _taskReadEncoders(10 * TASK_MILLISECOND, TASK_FOREVER, &readEncoders);

// Setup function
void setup()
{

  initializeSerial();
  initializeLEDs();
  encoderSetup();

  _runner.init();
  _runner.addTask(_taskChangeToBrightness);
  _runner.addTask(_taskRunPattern);
  _runner.addTask(_taskChangePalette);
  _runner.addTask(_taskChangePattern);
  _runner.addTask(_taskBlendPalette);
  _runner.addTask(_taskReadEncoders);
  _runner.addTask(_taskFade);

  _taskChangeToBrightness.enable();
  _taskRunPattern.enable();
  _taskChangePalette.enable();
  _taskChangePattern.enable();
  _taskBlendPalette.enable();
  _taskReadEncoders.enable();

  // Seed random number generator with noise from analog pin
  randomSeed(analogRead(32));
  // Also seed FastLED's random
  random16_set_seed(random());
  // Pick a random palette on start
  changePalette();
}

// Loop function
void loop()
{
  _runner.execute();
}

void runPattern()
{
  // Update global time accumulator
  // Use g_animationSpeed to control the speed of the patterns
  static unsigned long lastUpdate = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdate > 0)
  {
    g_timeAccumulator += (float)(currentMillis - lastUpdate) * g_animationSpeed;
    lastUpdate = currentMillis;
  }

  // Run pattern
  gPatterns[gCurrentPatternNumber]();
  g_statusLed[0].fadeToBlackBy(1);
  FastLED.show();
}

void printPatternAndPalette()
{
  constexpr const char *SGN = "printPatternAndPalette()";
  Serial.printf("%s: %s: Pattern: %s \tPalette: %s\n", timeToString().c_str(), SGN, patternNames[gCurrentPatternNumber], paletteNames[gCurrentPaletteNumber]);
}

// Initialize Serial Communication
void initializeSerial()
{
  Serial.begin(115200);
}

// Initialize LEDs
void initializeLEDs()
{
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, ATOMLED_PIN, COLOR_ORDER>(g_statusLed, 1);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MILLIAMPS);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setDither(BINARY_DITHER);
  FastLED.clear();
  gTargetPalette = gGradientPalettes[random8(gGradientPaletteCount)]; // Choose random palette on start
}

// Change Palette Periodically
void changePalette()
{
  gCurrentPaletteNumber = random8(gGradientPaletteCount);
  gTargetPalette = gGradientPalettes[gCurrentPaletteNumber];
  printPatternAndPalette();
}

// Blend Current Palette to Target Palette
void blendPalette()
{
  nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, BLEND_SPEED);
}

// TODO
static void oneClick()
{
  constexpr const char *SGN = "oneClick()";
  Serial.printf("%s: %s: Clicked! Next pattern. automode OFF\n", timeToString().c_str(), SGN);

  printPatternAndPalette();
  _taskChangePattern.disable();
  changePattern(); // Change immediately
  g_statusLed[0].setHue(0);
}

// TODO
static void longPress()
{
  constexpr const char *SGN = "longPress()";
  Serial.printf("%s: %s: Long press! Automode ON\n", timeToString().c_str(), SGN);
  _taskChangePattern.enableIfNot();
  g_statusLed[0].setHue(100);
}

// Usage example in pattern transition:
void changePattern()
{
  startFadeOut();
}

void fade()
{
  constexpr const char *SGN = "fade()";

  if (g_fadeState == FADING_OUT)
  {
    if (g_fadeCurrentBrightness > g_fadeTargetBrightness)
    {
      g_fadeCurrentBrightness--;
      // uint8_t scaledBrightness = calculatePowerScaledBrightness(g_fadeCurrentBrightness);
      FastLED.setBrightness(g_fadeCurrentBrightness);
      FastLED.show();
    }
    else
    {
      // Fade out complete
      Serial.printf("%s: %s: Fade out complete\n", timeToString().c_str(), SGN);
      _taskFade.disable();

      // Change pattern here
      gCurrentPatternNumber = (gCurrentPatternNumber + 1) % NUM_PATTERNS;
      g_patternInitNeeded = 1;
      printPatternAndPalette();

      // Start fade in
      startFadeIn();
    }
  }
  else if (g_fadeState == FADING_IN)
  {
    if (g_fadeCurrentBrightness < g_fadeTargetBrightness)
    {
      g_fadeCurrentBrightness++;
      FastLED.setBrightness(g_fadeCurrentBrightness);
      FastLED.show();
    }
    else
    {
      // Fade in complete
      Serial.printf("%s: %s: Fade in complete\n", timeToString().c_str(), SGN);
      _taskFade.disable();
      g_fadeState = FADE_NONE;

      // Re-enable tasks
      _taskChangeToBrightness.enable();
    }
  }
}

void startFadeOut()
{
  constexpr const char *SGN = "startFadeOut()";
  Serial.printf("%s: %s: Starting\n", timeToString().c_str(), SGN);

  g_fadeState = FADING_OUT;
  g_fadeStartBrightness = g_currentBrightness;
  g_fadeTargetBrightness = 0;
  g_fadeCurrentBrightness = g_fadeStartBrightness;

  // Disable interfering tasks
  _taskChangeToBrightness.disable();

  _taskFade.enable();
}

void startFadeIn()
{
  constexpr const char *SGN = "startFadeIn()";
  Serial.printf("%s: %s: Starting\n", timeToString().c_str(), SGN);
  g_fadeState = FADING_IN;
  g_fadeStartBrightness = 0;
  g_fadeTargetBrightness = g_currentBrightness;
  g_fadeCurrentBrightness = g_fadeStartBrightness;

  _taskFade.enable();
}

boolean changeToTarget(uint8_t target, uint8_t &current)
{
  if (target < current)
  {
    current--;
  }
  else if (target > current)
  {
    current++;
  }

  return target == current;
}

// Generic function that can be used for other parameters too
void changeToBrightness()
{

  constexpr const char *SGN = "ChangeToBrightness()";
  // Serial.printf("%s: %s: Adjusting Brightness: %u -> %u\n", timeToString().c_str(), SGN, g_currentBrightness, g_targetBrightness);

  if (changeToTarget(g_targetBrightness, g_currentBrightness))
  {
    _taskChangeToBrightness.disable();
    Serial.printf("%s: %s: Brightness adjusted to %u\n", timeToString().c_str(), SGN, g_currentBrightness);
  }

  // this doesn't work reliably yet
  // uint8_t scaledBrightness = calculatePowerScaledBrightness(g_currentBrightness);
  // Serial.printf("%s: %s: Brightness %u \tScaled: %u\n", timeToString().c_str(), SGN, g_currentBrightness, scaledBrightness);

  FastLED.setBrightness(g_currentBrightness);
}

// Calculate the scaled brightness based on the current power usage
// In theory this should keep brightness levels between animations consistent
uint8_t calculatePowerScaledBrightness(uint8_t targetBrightness)
{
  constexpr const char *SGN = "calculatePowerScaledBrightness()";
  uint32_t maxPower = calculate_max_brightness_for_power_mW(leds, NUM_LEDS, g_currentBrightness, 800);
  uint32_t usedPower = calculate_unscaled_power_mW(leds, NUM_LEDS);
  Serial.printf("%s: %s: Max power: %u, used power: %u\n", timeToString().c_str(), SGN, maxPower, usedPower);

  return scale8(targetBrightness, MAX_POWER_MILLIAMPS * 255 / usedPower);
}

std::string timeToString()
{
  char myString[20];
  unsigned long nowMillis = millis();
  unsigned int seconds = nowMillis / 1000;
  unsigned int remainder = nowMillis % 1000;
  int days = seconds / 86400;
  seconds %= 86400;
  byte hours = seconds / 3600;
  seconds %= 3600;
  byte minutes = seconds / 60;
  seconds %= 60;
  snprintf(myString, 20, "%02d:%02d:%02d:%02d.%03d", days, hours, minutes, seconds, remainder);
  return std::string(myString);
}

float fmap(float x, float a, float b, float c, float d)
{
  float f = x / (b - a) * (d - c) + c;
  return f;
}