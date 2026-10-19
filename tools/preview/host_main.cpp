// Desktop renderer for Uzumaru patterns.
//
// Runs the real pattern code (compiled against the real FastLED, using its
// stub platform) with a simulated clock and writes raw RGB frames to stdout.
// See tools/preview/README.md.
//
//   preview --list
//   preview <pattern> [seconds] [palette-index] [frame-ms] [stride] [speed] [uptime-hours]

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "palettes.h"
#include "audio.h"

HostSerial Serial;

static uint32_t s_nowMs = 0;
extern "C" uint32_t millis(void) { return s_nowMs; }
extern "C" uint32_t micros(void) { return s_nowMs * 1000; }
extern "C" void pinMode(uint8_t, uint8_t) {}
extern "C" void delay(int ms) { s_nowMs += ms; }
extern "C" void yield(void) {}

#include HOST_GLOBALS

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s --list | <pattern> [seconds] [palette] [frame-ms] [stride] [speed]\n", argv[0]);
        return 2;
    }
    if (!strcmp(argv[1], "--list"))
    {
        for (int i = 0; i < hostPatternCount(); i++)
            printf("%s\n", hostPatternName(i));
        printf("#palettes %d\n", (int)gGradientPaletteCount);
        return 0;
    }

    int pattern = -1;
    for (int i = 0; i < hostPatternCount(); i++)
        if (!strcmp(argv[1], hostPatternName(i)))
            pattern = i;
    if (pattern < 0)
    {
        fprintf(stderr, "unknown pattern %s\n", argv[1]);
        return 2;
    }
    float seconds = argc > 2 ? atof(argv[2]) : 8;
    int palette = argc > 3 ? atoi(argv[3]) : 0;
    int frameMs = argc > 4 ? atoi(argv[4]) : 12;
    int stride = argc > 5 ? atoi(argv[5]) : 3;
    if (argc > 6)
        g_animationSpeed = atof(argv[6]);
    // Start the animation clock as if the lamp had been on for a while
    double uptimeHours = argc > 7 ? atof(argv[7]) : 0;
    g_timeAccumulator = uptimeHours * 3600e3 * g_animationSpeed;

    srand(1234);
    random16_set_seed(1234);
    gCurrentPalette = gGradientPalettes[palette % gGradientPaletteCount];
    gTargetPalette = gCurrentPalette;
    g_currentBrightness = 255;
    g_targetBrightness = 255;
    s_nowMs = 1000; // EVERY_N_MILLISECONDS timers start at 0
    hostResetPatternState();

    int frames = (int)(seconds * 1000 / frameMs);
    uint32_t lastUpdate = s_nowMs;
    for (int f = 0; f < frames; f++)
    {
        s_nowMs += frameMs;
        // same accumulator update as runPattern()
        g_timeAccumulator += (double)(s_nowMs - lastUpdate) * g_animationSpeed;
        lastUpdate = s_nowMs;
        audioUpdate();
        hostRender(pattern, buffer1);
        if (f % stride == 0)
            fwrite(buffer1, 3, NUM_LEDS, stdout);
    }
    return 0;
}
