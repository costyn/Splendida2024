#ifndef ENCODERS_H
#define ENCODERS_H

#include <i2cEncoderLibV2.h>
#include <Wire.h>

#include "globals.h"

typedef enum
{
    BRIGHTNESS,
    SPEED,
    PLAYLIST,
} EncoderState;

#define I2C_SDA_PIN 33
#define I2C_SCL_PIN 23
#define I2C_INT_PIN 19

#define ENCODER_ADDRESS 0x28
#define ENCODER_MIN 0
#define ENCODER_MAX 254
#define ENCODER_STEP 1
#define ENCODER_DEFAULT 150

// Forward Declarations
extern EncoderState g_encoderState;
extern uint8_t g_targetBrightness;
extern float g_animationSpeed;
extern Task _taskChangeToBrightness;
extern Task _taskEncoderAnimation;
extern Task _taskChangePattern;
extern CRGBPalette16 gCurrentPalette;

// Function Prototypes
void encoder_onChange(i2cEncoderLibV2 *obj);
void encoder_onClick(i2cEncoderLibV2 *obj);
void encoder_onFadeEnd(i2cEncoderLibV2 *obj);
void encoder_doubleClick(i2cEncoderLibV2 *obj);

void setEncoderState(i2cEncoderLibV2 *obj, EncoderState state);
void setEncoderColor(i2cEncoderLibV2 *obj, EncoderState state);

void changePattern();
std::string timeToString();

#endif