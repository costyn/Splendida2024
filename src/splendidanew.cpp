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

// Setup function
void setup()
{
  initializeGPIO();
  initializeSerial();
  initializeLEDs();
  initializeButton();

  _runner.init();
  _runner.addTask(_taskChangeToBrightness);
  _runner.addTask(_taskRunPattern);
  _runner.addTask(_taskChangePalette);
  _runner.addTask(_taskChangePattern);
  _runner.addTask(_taskHandleButton);
  _runner.addTask(_taskReadPotentiometers);
  _runner.addTask(_taskBlendPalette);
  _runner.addTask(_taskFade);

  _taskChangeToBrightness.enable();
  _taskRunPattern.enable();
  _taskChangePalette.enable();
  _taskChangePattern.enable();
  _taskHandleButton.enable();
  _taskReadPotentiometers.enable();
  _taskBlendPalette.enable();

  // Seed random number generator with noise from analog pin
  randomSeed(analogRead(BRIGHTNESS_POT_PIN));
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
  Serial.print("Pattern: ");
  Serial.print(patternNames[gCurrentPatternNumber]);
  Serial.print("\tPalette: ");
  Serial.println(paletteNames[gCurrentPaletteNumber]);
}

void initializeGPIO()
{
  // Configure ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_12);
  adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_12);
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

// Initialize Button
void initializeButton()
{
  g_patternButton.attachClick(oneClick);
  // g_patternButton.attachDoubleClick(doubleClick);
  g_patternButton.attachDuringLongPress(longPress);
  // g_patternButton.setDebounceTicks(80);
}

// Handle Button Events
void handleButton()
{
  g_patternButton.tick();
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

static void oneClick()
{
  Serial.println("Clicked! Next pattern. automode OFF");
  printPatternAndPalette();
  _taskChangePattern.disable();
  changePattern(); // Change immediately
  g_statusLed[0].setHue(0);
}

static void longPress()
{
  Serial.println("Long press! Automode ON");
  _taskChangePattern.enable();
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
      Serial.printf("%s: Fade out complete\n", SGN);
      _taskFade.disable();

      // Change pattern here
      gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
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
      Serial.printf("%s: Fade in complete\n", SGN);
      _taskFade.disable();
      g_fadeState = FADE_NONE;

      // Re-enable tasks
      _taskReadPotentiometers.enable();
      _taskChangeToBrightness.enable();
    }
  }
}

void startFadeOut()
{
  g_fadeState = FADING_OUT;
  g_fadeStartBrightness = g_currentBrightness;
  g_fadeTargetBrightness = 0;
  g_fadeCurrentBrightness = g_fadeStartBrightness;

  // Disable interfering tasks
  _taskReadPotentiometers.disable();
  _taskChangeToBrightness.disable();

  _taskFade.enable();
}

void startFadeIn()
{
  g_fadeState = FADING_IN;
  g_fadeStartBrightness = 0;
  g_fadeTargetBrightness = g_currentBrightness;
  g_fadeCurrentBrightness = g_fadeStartBrightness;

  _taskFade.enable();
}

int readPotentiometer(uint8_t pin)
{
  if (pin == BRIGHTNESS_POT_PIN)
  {
    return adc1_get_raw(ADC1_CHANNEL_4);
  }
  else if (pin == SPEED_POT_PIN)
  {
    return adc1_get_raw(ADC1_CHANNEL_5);
  }
  return 0;
}
void readPotentiometers()
{
  constexpr const char *SGN = "readPotentiometers()";

  static uint16_t lastMappedBrightness = 0;
  static float lastMappedSpeed = 0.0f;

  // Read and smooth both potentiometers
  uint16_t brightnessPot = readPotentiometer(BRIGHTNESS_POT_PIN);
  uint16_t speedPot = readPotentiometer(SPEED_POT_PIN);
  // Serial.printf("%s: %s: Raw pot %u\n", timeToString().c_str(), SGN, speedPot);

  g_smoothedBrightnessPot += brightnessPot;
  g_smoothedSpeedPot += speedPot;

  // Handle brightness - invert map function
  int mappedBrightness = map(g_smoothedBrightnessPot.get_avg(), 0, 4096, MAX_BRIGHTNESS, 0);
  if (lastMappedBrightness != mappedBrightness)
  {
    g_targetBrightness = mappedBrightness;
    _taskChangeToBrightness.enable();
    lastMappedBrightness = mappedBrightness;
  }

  // Handle speed - invert map function
  float mappedSpeed = fmap(g_smoothedSpeedPot.get_avg(), 0, 4095, MAX_ANIMATION_SPEED, MIN_ANIMATION_SPEED);
  if (abs(lastMappedSpeed - mappedSpeed) > 0.001f)
  {
    Serial.printf("%s: %s: Adjusting speed mapped %f\n", timeToString().c_str(), SGN, mappedSpeed);
    g_animationSpeed = mappedSpeed;
    lastMappedSpeed = mappedSpeed;
  }
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
    _taskReadPotentiometers.enable();
  }

  // this doesn't work reliably yet
  // uint8_t scaledBrightness = calculatePowerScaledBrightness(g_currentBrightness);
  // Serial.println("Brightness: " + String(g_currentBrightness) + " Scaled: " + String(scaledBrightness));
  FastLED.setBrightness(g_currentBrightness);
}

// Calculate the scaled brightness based on the current power usage
// In theory this should keep brightness levels between animations consistent
uint8_t calculatePowerScaledBrightness(uint8_t targetBrightness)
{
  uint32_t maxPower = calculate_max_brightness_for_power_mW(leds, NUM_LEDS, g_currentBrightness, 800);
  Serial.println("Max power: " + String(maxPower));
  uint32_t usedPower = calculate_unscaled_power_mW(leds, NUM_LEDS);
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