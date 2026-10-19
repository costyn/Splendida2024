// Music reactivity.
//
// g_audio has the shape of a beatsense frame (https://github.com/costyn/beatsense):
// tempo, beat phase, band levels, onsets, energy. Patterns read it every frame.
//
// Without a beatsense board a synthetic metronome fills it in: a steady beat
// whose tempo follows the speed knob, so beat-driven patterns work standalone.
// Build with -D UZUMARU_BEATSENSE to poll a real board on the encoder's I2C
// bus (address 0x42). That path is written against the protocol but has not
// been run on hardware yet.
#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>
#include <beatsense_protocol.h>

extern beatsense::proto::Frame g_audio;

// Call once per rendered frame, before the patterns run.
void audioUpdate();

// True during the one frame in which a new beat started.
bool audioBeatStarted();

// True when the frame comes from a beatsense board that hears music.
inline bool audioIsLive()
{
    return g_audio.status & beatsense::proto::kStatusSignal;
}

#endif
