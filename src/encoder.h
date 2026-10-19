// DuPPa I2C RGB rotary encoder: the single control knob of Uzumaru.
#ifndef ENCODER_H
#define ENCODER_H

#include <i2cEncoderLibV2.h>
#include <Wire.h>

#include "globals.h"

typedef enum
{
    BRIGHTNESS,
    SPEED,
    PLAYLIST,
} EncoderState;

#define ENCODER_MIN 0
#define ENCODER_MAX 254
#define ENCODER_STEP 1
#define ENCODER_DEFAULT 150

extern EncoderState g_encoderState;

void encoderSetup();
void readEncoder();
void updateEncoderIdleAnimation();

#endif
