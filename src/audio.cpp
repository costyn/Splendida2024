#include "audio.h"
#include "globals.h"

#ifdef UZUMARU_BEATSENSE
#include <Wire.h>
#endif

using namespace beatsense::proto;

#define AUDIO_IDLE_BPM 72.0f // synthetic beat at the default animation speed

Frame g_audio;
static uint8_t s_lastBeatCount = 0;
static bool s_beatStarted = false;

// A steady metronome in beatsense's frame format. Its tempo scales with the
// speed knob (72 BPM at the default speed, 18-360 BPM over the knob's range).
static void syntheticBeat()
{
    static uint32_t lastMs = millis();
    static uint32_t phase32 = 0; // beat phase in 1/2^32 of a beat
    uint32_t now = millis();
    uint32_t dt = now - lastMs;
    lastMs = now;

    float bpm = AUDIO_IDLE_BPM * fabsf(g_animationSpeed) / DEFAULT_ANIMATION_SPEED;
    bpm = constrain(bpm, 18.0f, 360.0f);
    uint64_t step = (uint64_t)(bpm / 60000.0f * dt * 4294967296.0f);
    uint64_t next = (uint64_t)phase32 + step;
    if (next >> 32)
    {
        g_audio.beatCount++;
        g_audio.beatInBar = (g_audio.beatInBar + 1) & 3;
    }
    phase32 = (uint32_t)next;

    g_audio.status = 0; // no signal: patterns can tell this is not music
    g_audio.bpmQ88 = (uint16_t)(bpm * 256);
    g_audio.beatPhase = phase32 >> 16;
    g_audio.confidence = 0;
    // a gentle kick-like envelope so level-driven patterns have something to show
    uint8_t env = 255 - (g_audio.beatPhase >> 8);
    g_audio.level[0] = scale8(env, env);
    g_audio.level[1] = 96;
    g_audio.level[2] = 64;
    g_audio.onset[0] = g_audio.onset[1] = g_audio.onset[2] = 0;
    g_audio.energy = 128;
    g_audio.groove = 0;
}

#ifdef UZUMARU_BEATSENSE
static bool readBeatsense()
{
    uint8_t raw[kFrameSize];
    if (Wire.requestFrom(kDefaultI2cAddress, (uint8_t)kFrameSize) != kFrameSize)
        return false;
    for (size_t i = 0; i < kFrameSize; i++)
        raw[i] = Wire.read();
    Frame frame;
    if (!decodeFrame(raw, frame))
        return false;
    g_audio = frame;
    return true;
}
#endif

void audioUpdate()
{
    bool live = false;
#ifdef UZUMARU_BEATSENSE
    live = readBeatsense() && audioIsLive();
#endif
    if (!live)
        syntheticBeat();

    s_beatStarted = g_audio.beatCount != s_lastBeatCount;
    s_lastBeatCount = g_audio.beatCount;
}

bool audioBeatStarted()
{
    return s_beatStarted;
}
