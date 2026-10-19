// SPDX-License-Identifier: MIT
//
// This header is deliberately MIT-licensed (the rest of beatsense is EUPL-1.2) so that I2C hosts under any license can include it
// without inheriting the copyleft. The author owns the code and can dual-license it this way.
//
// beatsense I2C protocol, version 2 (version 1: 17 bytes, no `groove`; a v2 host rejects a v1 frame: version byte, CRC position).
//
// The board is an I2C slave (default 7-bit address 0x42).
//   Read : any read returns the 18-byte status frame below, starting at offset 0 (no register pointer needed).
//   Write: [register, value] sets a config register or taps (see Reg). Unknown registers are ignored.
//
// The frame describes *state*, not events: beatPhase is the position within the current beat at the moment the frame was produced,
// so the host's poll rate only affects smoothness, and a missed poll loses nothing. Multi-byte fields are little-endian.
//
//   off  field       type      meaning
//    0   version     u8        kVersion
//    1   status      u8        bit0 signal present, bit1 beat locked, bit2 clipping, bit3 breakdown
//    2   bpm         u16 Q8.8  tempo
//    4   beatPhase   u16       0-65535 = position within the current beat (0 = the beat itself)
//    6   beatCount   u8        wraps; increments each beat (lets the host detect missed beats)
//    7   beatInBar   u8        0-3, best guess
//    8   confidence  u8        0-255
//    9   level[3]    u8        low/mid/high, AGC-normalised
//   12   onset[3]    u8        max onset strength since the previous read (peak-hold, cleared on read)
//   15   energy      u8        slow (~8 bar) energy relative to the long-term average; 128 = average
//   16   groove      u8        0-255: how much kick-driven groove is present right now (a state, not peak-held). Raw and fast (about
//                               1 s): 0 in a breakdown or without a steady kick, 255 in a full groove. Hosts do their own easing.
//   17   crc         u8        CRC-8 (poly 0x07, init 0) over bytes 0-16
#ifndef BEATSENSE_PROTOCOL_H
#define BEATSENSE_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

namespace beatsense {
namespace proto {

constexpr uint8_t kVersion = 2;
constexpr uint8_t kDefaultI2cAddress = 0x42; // 7-bit; the firmware's address is configurable in src/config.h
constexpr size_t kFrameSize = 18;

// status bits
constexpr uint8_t kStatusSignal = 1 << 0;  // input is above the noise gate
constexpr uint8_t kStatusLocked = 1 << 1;  // tempo/beat tracking has locked (confidence passed the hysteresis threshold)
constexpr uint8_t kStatusClipping = 1 << 2; // input reached full scale recently
// The kick is gone (the low band is well below its recent average) while there is signal; the tempo and beat clock are held, free-running.
// Added without a version bump: the frame layout is unchanged and hosts test status bits with masks (a host that predates bit 3 ignores it).
constexpr uint8_t kStatusBreakdown = 1 << 3;

// Write registers: [reg, value]
enum Reg : uint8_t {
  kRegAgcMode = 0x10,   // value: AgcMode
  kRegNoiseGate = 0x11, // value: gate threshold as -dBFS (RMS); 66 = -66 dBFS. 0 = default
  kRegLatencyMs = 0x12, // value: int8 ms, shifts beatPhase forward (positive) or back to compensate for the host's own display latency
  kRegTap = 0x13,       // value ignored: "a beat is happening now" (seeds the tracker; tap tempo)
};

enum AgcMode : uint8_t { kAgcNormal = 0, kAgcVivid = 1, kAgcLazy = 2, kAgcOff = 3 };

struct Frame {
  uint8_t version = kVersion;
  uint8_t status = 0;
  uint16_t bpmQ88 = 0;
  uint16_t beatPhase = 0;
  uint8_t beatCount = 0;
  uint8_t beatInBar = 0;
  uint8_t confidence = 0;
  uint8_t level[3] = {0, 0, 0};
  uint8_t onset[3] = {0, 0, 0};
  uint8_t energy = 0;
  uint8_t groove = 0;

  float bpm() const { return bpmQ88 / 256.0f; }
};

struct WriteCommand {
  Reg reg;
  uint8_t value;
};

// CRC-8, polynomial x^8 + x^2 + x + 1 (0x07), init 0, no reflection or final xor
inline uint8_t crc8(const uint8_t *data, size_t len) {
  uint8_t crc = 0;
  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (int bit = 0; bit < 8; bit++) {
      crc = (uint8_t)((crc & 0x80) ? ((crc << 1) ^ 0x07) : (crc << 1));
    }
  }
  return crc;
}

inline void encodeFrame(const Frame &f, uint8_t out[kFrameSize]) {
  out[0] = f.version;
  out[1] = f.status;
  out[2] = (uint8_t)(f.bpmQ88 & 0xFF);
  out[3] = (uint8_t)(f.bpmQ88 >> 8);
  out[4] = (uint8_t)(f.beatPhase & 0xFF);
  out[5] = (uint8_t)(f.beatPhase >> 8);
  out[6] = f.beatCount;
  out[7] = f.beatInBar;
  out[8] = f.confidence;
  for (int i = 0; i < 3; i++) {
    out[9 + i] = f.level[i];
    out[12 + i] = f.onset[i];
  }
  out[15] = f.energy;
  out[16] = f.groove;
  out[17] = crc8(out, 17);
}

// Returns false (and leaves `f` untouched) if the CRC or the version doesn't match, e.g. when the bus floats because no board is fitted
inline bool decodeFrame(const uint8_t in[kFrameSize], Frame &f) {
  if (crc8(in, 17) != in[17] || in[0] != kVersion) {
    return false;
  }
  f.version = in[0];
  f.status = in[1];
  f.bpmQ88 = (uint16_t)(in[2] | (in[3] << 8));
  f.beatPhase = (uint16_t)(in[4] | (in[5] << 8));
  f.beatCount = in[6];
  f.beatInBar = in[7];
  f.confidence = in[8];
  for (int i = 0; i < 3; i++) {
    f.level[i] = in[9 + i];
    f.onset[i] = in[12 + i];
  }
  f.energy = in[15];
  f.groove = in[16];
  return true;
}

inline void encodeWrite(Reg reg, uint8_t value, uint8_t out[2]) {
  out[0] = reg;
  out[1] = value;
}

// A tap needs no value byte, so len 1 is accepted for kRegTap. Returns false for unknown registers or short writes.
inline bool decodeWrite(const uint8_t *data, size_t len, WriteCommand &cmd) {
  if (len < 1) {
    return false;
  }
  switch (data[0]) {
  case kRegTap:
    cmd.reg = kRegTap;
    cmd.value = len > 1 ? data[1] : 0;
    return true;
  case kRegAgcMode:
  case kRegNoiseGate:
  case kRegLatencyMs:
    if (len < 2) {
      return false;
    }
    cmd.reg = (Reg)data[0];
    cmd.value = data[1];
    return true;
  default:
    return false;
  }
}

} // namespace proto
} // namespace beatsense

#endif // BEATSENSE_PROTOCOL_H
