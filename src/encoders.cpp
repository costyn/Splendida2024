#include "encoders.h"

/*
    encoderSetup
    Initialize the I2C encoders with the custom pins

*/

static i2cEncoderLibV2 RGBEncoder = i2cEncoderLibV2(ENCODER_ADDRESS);
extern EncoderState g_encoderState;

EncoderState g_encoderState = BRIGHTNESS; // default state

void encoderSetup()
{
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); // Initialize I2C with custom pins

    pinMode(I2C_INT_PIN, INPUT);

    RGBEncoder.reset();

    RGBEncoder.begin(
        i2cEncoderLibV2::INT_DATA |
        i2cEncoderLibV2::WRAP_DISABLE |
        i2cEncoderLibV2::DIRE_RIGHT |
        i2cEncoderLibV2::IPUP_ENABLE |
        i2cEncoderLibV2::RMOD_X1 |
        i2cEncoderLibV2::RGB_ENCODER);
    RGBEncoder.id = 0;                      // do we need this?
    RGBEncoder.writeAntibouncingPeriod(10); // 250ms of debouncing
    RGBEncoder.writeDoublePushPeriod(50);   // Set the double push period to 500ms

    setEncoderState(&RGBEncoder, BRIGHTNESS); // Initial state

    /* Configure the callback events */
    RGBEncoder.onChange = encoder_onChange;
    RGBEncoder.onButtonPush = encoder_onClick;
    RGBEncoder.onButtonDoublePush = encoder_doubleClick;

    /* Enable the I2C Encoder V2 interrupts according to the previus attached callback */
    RGBEncoder.autoconfigInterrupt();

    RGBEncoder.writeCounter((int32_t)ENCODER_DEFAULT);
    RGBEncoder.writeMax((int32_t)ENCODER_MAX);
    RGBEncoder.writeMin((int32_t)ENCODER_MIN);
    RGBEncoder.writeStep((int32_t)ENCODER_STEP);
}

void readEncoders()
{
    if (digitalRead(I2C_INT_PIN) == LOW)
    {
        RGBEncoder.updateStatus();
    }
}

void encoder_onChange(i2cEncoderLibV2 *obj)
{
    constexpr const char *SGN = "encoder_onChange()";
    bool increasing = obj->readStatus(i2cEncoderLibV2::RINC);

    switch (g_encoderState)
    {
    case BRIGHTNESS:
        if (increasing)
        {
            g_targetBrightness = min(MAX_BRIGHTNESS, g_targetBrightness + BRIGHNESS_STEP);
        }
        else
        {
            g_targetBrightness = max(MIN_BRIGHTNESS, g_targetBrightness - BRIGHNESS_STEP);
        }
        Serial.printf("%s: Brightness target: %d\n", SGN, g_targetBrightness);
        _taskChangeToBrightness.enableIfNot();
        break;

    case SPEED:
        if (increasing)
        {
            g_animationSpeed = min(0.4f, g_animationSpeed + 0.01f);
        }
        else
        {
            g_animationSpeed = max(-0.4f, g_animationSpeed - 0.01f);
        }
        Serial.printf("%s: Animation Speed: %.2f\n", SGN, g_animationSpeed);
        break;

    default:
        break;
    }
}

// Toggle between BRIGHTNESS and SPEED
void encoder_onClick(i2cEncoderLibV2 *obj)
{
    setEncoderState(obj, g_encoderState == BRIGHTNESS ? SPEED : BRIGHTNESS);
}

void encoder_doubleClick(i2cEncoderLibV2 *obj)
{
    obj->writeRGBCode(0xFFFFFF); // White
    constexpr const char *SGN = "encoder_doubleClick()";
    Serial.printf("%s: %s: Initiating change pattern\n", timeToString().c_str(), SGN);
    changePattern();
}

void setEncoderState(i2cEncoderLibV2 *obj, EncoderState state)
{
    constexpr const char *SGN = "setEncoderState()";

    switch (state)
    {
    case BRIGHTNESS:
        obj->writeRGBCode(0xFF0000); // Red
        break;
    case SPEED:
        obj->writeRGBCode(0x00FFFF); // Cyan
        break;
    }
    g_encoderState = state;
    Serial.printf("%s: %s: EncoderState: %s\n", timeToString().c_str(), SGN, state == BRIGHTNESS ? "BRIGHTNESS" : "SPEED");
}