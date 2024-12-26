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

// TODO
// - Check if if crossfading is already happening before starting a new one

#include "splendidanew.h"
#include "encoders.h"

uint8_t g_targetBrightness = DEFAULT_BRIGHTNESS;
uint8_t g_currentBrightness = 0;
float g_animationSpeed = DEFAULT_ANIMATION_SPEED;
float g_timeAccumulator = 0.0f;
uint8_t gBuffer1PatternNumber = 0;
uint8_t gBuffer2PatternNumber = 0;
CRGB g_statusLed[1];
byte g_buffer1InitNeeded = 1;
byte g_buffer2InitNeeded = 1;
CRGB leds[NUM_LEDS];
CRGB buffer1[NUM_LEDS + 1];
CRGB buffer2[NUM_LEDS + 1];
uint16_t g_lastSafeIndex = 256;                                                   // FIXME change to a #define
CRGBPalette16 gTargetPalette = gGradientPalettes[random8(gGradientPaletteCount)]; // Choose random palette on start
RenderBuffer _renderBuffer = BUFFER1;

uint8_t _bufferBlendAmount = 0; // blending between buffers

Scheduler _runner;
// This task smoothly changes the brightness to the target brightness
Task _taskChangeToBrightness(10 * TASK_MILLISECOND, TASK_FOREVER, &changeToBrightness);
// Calls the pattern rendering functions
Task _taskRunPattern(1 * TASK_MILLISECOND, TASK_FOREVER, &runPattern);
// Blends the two animation buffers to crossfade between patterns
Task _taskBufferCrossfade(round(CROSSFADE_TIME / CROSSFADE_STEPS) * TASK_MILLISECOND, CROSSFADE_STEPS, &bufferCrossfade);
// Changes the palette every SECONDS_PER_PALETTE seconds
Task _taskChangePalette(SECONDS_PER_PALETTE *TASK_SECOND, TASK_FOREVER, &changePalette);
// Smoothly blends during palette changes
Task _taskBlendPalette(BLEND_INTERVAL_MS *TASK_MILLISECOND, TASK_FOREVER, &blendPalette);
// Initiates patttern change every SECONDS_PER_PATTERN seconds
Task _taskChangePattern(SECONDS_PER_PATTERN *TASK_SECOND, TASK_FOREVER, &changePattern);
// Reads the rotary encoder
Task _taskReadEncoder(10 * TASK_MILLISECOND, TASK_FOREVER, &readEncoder);
// Animates the encoder LED, when the encoder is idle (not being turned)
Task _taskEncoderAnimation(25 * TASK_MILLISECOND, TASK_FOREVER, &updateEncoderIdleAnimation);

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
  _runner.addTask(_taskReadEncoder);
  _runner.addTask(_taskEncoderAnimation);

  _runner.addTask(_taskBufferCrossfade);
  _taskBufferCrossfade.setOnDisable(&bufferBlendDone);

  _taskChangeToBrightness.enable();
  _taskRunPattern.enable();
  _taskChangePalette.enable();
  _taskChangePattern.enableDelayed(SECONDS_PER_PATTERN * TASK_SECOND); // Don't do this immediately
  _taskBlendPalette.enable();
  _taskReadEncoder.enable();
  _taskEncoderAnimation.enable();

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
  // Yes, it's a bit inefficient to render both buffers all the time,
  // when most of the time only one is showing, but it's not a big deal
  gPatterns[gBuffer1PatternNumber](buffer1);
  gPatterns[gBuffer2PatternNumber](buffer2);

  blend(buffer1, buffer2, leds, NUM_LEDS, _bufferBlendAmount);

  g_statusLed[0].fadeToBlackBy(1);
  FastLED.show();
  // Pass the LED buffer to the pattern function
}

void bufferCrossfade()
{
  if (_taskBufferCrossfade.getRunCounter() == 1)
  {
    if (_renderBuffer == BUFFER1)
    {
      g_buffer2InitNeeded = 1;
      // Serial.println("g_buffer2InitNeeded TRUE");
    }
    else
    {
      g_buffer1InitNeeded = 1;
      // Serial.println("g_buffer1InitNeeded TRUE");
    }
  }

  if (_renderBuffer == BUFFER1)
  {
    _bufferBlendAmount = _taskBufferCrossfade.getRunCounter();
  }
  else
  {
    _bufferBlendAmount = CROSSFADE_STEPS - _taskBufferCrossfade.getRunCounter();
  }
  Serial.print(".");
}

void bufferBlendDone()
{
  constexpr const char *SGN = "bufferBlendDone()";
  _renderBuffer == BUFFER1 ? _renderBuffer = BUFFER2 : _renderBuffer = BUFFER1;
  _taskBufferCrossfade.setIterations(CROSSFADE_STEPS);

  Serial.println("Done");
  String buffer = _renderBuffer == BUFFER1 ? "1" : "2";
  Serial.printf("%s: %s: Rendering to buffer %s\n", timeToString().c_str(), SGN, buffer);
  printPatternAndPalette();
}

// Usage example in pattern transition:
void changePattern()
{
  constexpr const char *SGN = "changePattern()";

  Serial.printf("%s: %s: Crossfading", timeToString().c_str(), SGN);
  if (_renderBuffer == BUFFER1)
  {
    gBuffer2PatternNumber = (gBuffer1PatternNumber + 1) % gPatternCount;
  }
  else
  {
    gBuffer1PatternNumber = (gBuffer2PatternNumber + 1) % gPatternCount;
  }
  _taskBufferCrossfade.enableIfNot();
}

void printPatternAndPalette()
{
  constexpr const char *SGN = "printPatternAndPalette()";
  if (_renderBuffer == BUFFER1)
  {
    Serial.printf("%s: %s: Pattern: %s \tPalette: %s\n", timeToString().c_str(), SGN, patternNames[gBuffer1PatternNumber], paletteNames[gCurrentPaletteNumber]);
  }
  else
  {
    Serial.printf("%s: %s: Pattern: %s \tPalette: %s\n", timeToString().c_str(), SGN, patternNames[gBuffer2PatternNumber], paletteNames[gCurrentPaletteNumber]);
  }
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

// TODO: automode off and on
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

void changeToBrightness()
{
  constexpr const char *SGN = "ChangeToBrightness()";
  if (changeToTarget(g_targetBrightness, g_currentBrightness))
  {
    _taskChangeToBrightness.disable();
    Serial.printf("%s: %s: Brightness adjusted to %u\n", timeToString().c_str(), SGN, g_currentBrightness);
  }
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