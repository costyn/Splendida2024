#ifndef ENCODERS_H
#define ENCODERS_H

#include <i2cEncoderLibV2.h>
#include <Wire.h>

#include "globals.h"

typedef enum
{
    ROTATE,
    LIMIT,
    CLICK
} EncoderEvent;

typedef enum
{
    BRIGHTNESS,
    SPEED,
    ROTATION,
    PRESET,
} EncoderState;

#define I2C_SDA_PIN 23
#define I2C_SCL_PIN 19
#define I2C_INT_PIN 22

#define ENCODER_ADDRESS 0x28
#define ENCODER_MIN 0
#define ENCODER_MAX 254
#define ENCODER_STEP 1
#define ENCODER_DEFAULT 150

// Forward Declarations
extern EncoderState g_encoderState;
extern uint8_t g_targetBrightness;
extern float g_animationSpeed;
extern Task _taskChangeToBrightness; // Declare the external task

// Function Prototypes
void encoder_onChange(i2cEncoderLibV2 *obj);
void encoder_onClick(i2cEncoderLibV2 *obj);
void encoder_onMinMax(i2cEncoderLibV2 *obj);
void encoder_onFadeEnd(i2cEncoderLibV2 *obj);
void encoder_doubleClick(i2cEncoderLibV2 *obj);
void encoderColorFeedback(i2cEncoderLibV2 *obj, EncoderEvent event);

void setPreset(int presetIndex);
void setBrightness(int brightness);

void changePattern();
std::string timeToString();

#endif