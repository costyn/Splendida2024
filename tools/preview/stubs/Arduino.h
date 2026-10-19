// Minimal Arduino shim so the pattern code compiles on a desktop machine.
// Only what the pattern/palette sources actually use is provided.
#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <string>
#include <algorithm>

#include "platforms/stub/led_sysdefs_stub.h"

typedef uint8_t byte;
typedef bool boolean;

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif
#ifndef PROGMEM
#define PROGMEM
#endif
#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const uint8_t *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const uint16_t *)(addr))
#endif

#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
using std::max;
using std::min;

inline long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
inline long random(long howbig) { return howbig ? ::rand() % howbig : 0; }
inline long random(long lo, long hi) { return lo + random(hi - lo); }
inline long random() { return ::rand(); }
inline void randomSeed(unsigned long s) { ::srand(s); }

struct HostSerial
{
    bool quiet = true;
    void begin(unsigned long) {}
    int printf(const char *fmt, ...)
    {
        if (quiet) return 0;
        va_list ap;
        va_start(ap, fmt);
        int n = vfprintf(stderr, fmt, ap);
        va_end(ap);
        return n;
    }
    template <typename T> void print(const T &) {}
    template <typename T> void println(const T &) {}
    void println() {}
};
extern HostSerial Serial;
