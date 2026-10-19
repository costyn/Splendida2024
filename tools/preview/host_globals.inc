// Globals normally owned by main.cpp, for the desktop renderer.
uint8_t g_targetBrightness = DEFAULT_BRIGHTNESS;
uint8_t g_currentBrightness = 0;
float g_animationSpeed = DEFAULT_ANIMATION_SPEED;
double g_timeAccumulator = 0.0;
uint8_t gBuffer1PatternNumber = 0;
uint8_t gBuffer2PatternNumber = 1;
byte g_buffer1InitNeeded = 1;
byte g_buffer2InitNeeded = 1;
CRGB leds[NUM_LEDS];
CRGB buffer1[NUM_LEDS + 1];
CRGB buffer2[NUM_LEDS + 1];
uint16_t g_lastSafeIndex = NUM_LEDS;
CRGBPalette16 gTargetPalette;
RenderBuffer _renderBuffer = BUFFER1;
uint8_t _bufferBlendAmount = 0;
Task _taskChangeToBrightness, _taskChangePattern, _taskEncoderAnimation;
void changePattern(int8_t) {}
std::string timeToString() { return ""; }
float fmap(float x, float a, float b, float c, float d) { return x / (b - a) * (d - c) + c; }

static int hostPatternCount() { return gPatternCount; }
static const char *hostPatternName(int i) { return gPatterns[i].name; }
static void hostResetPatternState() { g_buffer1InitNeeded = 1; }
static void hostRender(int i, CRGB *buf) { gPatterns[i].render(buf); }
