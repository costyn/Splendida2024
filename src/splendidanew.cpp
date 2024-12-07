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
  _runner.addTask(_taskFadeOut);
  _runner.addTask(_taskFadeIn);

  _taskChangeToBrightness.enable();
  _taskRunPattern.enable();
  _taskChangePalette.enable();
  _taskChangePattern.enable();
  _taskHandleButton.enable();
  _taskReadPotentiometers.enable();
  _taskBlendPalette.enable();
}

// Loop function
void loop()
{
  _runner.execute();
}

void runPattern()
{
  gPatterns[gCurrentPatternNumber](); // play current pattern
  statled[0].fadeToBlackBy(1);

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
  // button.attachDoubleClick(doubleClick);
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
  changePattern();
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

// Usage example in pattern transition:
void changePattern()
{
  _taskFadeOut.enable();
}

void fadeOut()
{
  constexpr const char *SGN = "fadeOut()";

  static uint8_t currentBrightness = _currentBrightness;

  _taskReadPotentiometers.disable();
  _taskChangeToBrightness.disable();
  currentBrightness--;
  Serial.printf("%s: %s: Fading step %u from %u\n", timeToString().c_str(), SGN, currentBrightness, _currentBrightness);

  if (currentBrightness == 0)
  {
    Serial.printf("%s: %s: End Fade out %u\n", timeToString().c_str(), SGN, _currentBrightness);

    _taskFadeOut.disable();
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
    InitNeeded = 1;
    printPatternAndPalette();
    _taskFadeIn.enable();
  }
  FastLED.setBrightness(currentBrightness);
}

void fadeIn()
{
  constexpr const char *SGN = "fadeIn()";

  static uint8_t currentBrightness = 0;
  currentBrightness++;
  Serial.printf("%s: %s: Fading step %u from %u\n", timeToString().c_str(), SGN, currentBrightness, _currentBrightness);
  if (currentBrightness == _currentBrightness)
  {
    Serial.printf("%s: %s: End Fade in %u\n", timeToString().c_str(), SGN, _currentBrightness);

    _taskReadPotentiometers.enable();
    _taskChangeToBrightness.enable();
    _taskFadeIn.disable();
  }
  FastLED.setBrightness(currentBrightness);
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
  static int lastMappedBrightness = 0;

  int brightnessPot = readPotentiometer(BRIGHTNESS_POT_PIN);

  smoothedBrightnessPot += brightnessPot;

  int mappedBrightness = map(smoothedBrightnessPot.get_avg(), 0, 4096, 0, 150);
  if (lastMappedBrightness != mappedBrightness)
  {
    // _taskReadPotentiometers.disable(); // Disable the task if we're already changing brightness
    _targetBrightness = mappedBrightness;
    _taskChangeToBrightness.enable();
    lastMappedBrightness = mappedBrightness;
  }

  // Serial.println(smoothedBrightnessPot.get_avg());
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
  Serial.printf("%s: %s: Adjusting Brightness: %u -> %u\n", timeToString().c_str(), SGN, _currentBrightness, _targetBrightness);

  if (changeToTarget(_targetBrightness, _currentBrightness))
  {
    _taskChangeToBrightness.disable();
    Serial.printf("%s: %s: Brightness adjusted to %u\n", timeToString().c_str(), SGN, _currentBrightness);
    _taskReadPotentiometers.enable();
  }

  FastLED.setBrightness(_currentBrightness);
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