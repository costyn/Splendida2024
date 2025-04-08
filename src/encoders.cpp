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

void readEncoder()
{
    if (digitalRead(I2C_INT_PIN) == LOW)
    {
        _taskEncoderAnimation.delay(ENCODER_ANIMATION_IDLE_TIMEOUT);
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
        setEncoderColor(obj, BRIGHTNESS);
        uint8_t step;
        if (g_targetBrightness < 20)
        {
            step = 1; // Fine control at low brightness
        }
        else if (g_targetBrightness < 60)
        {
            step = 3; // Medium steps
        }
        else if (g_targetBrightness < 150)
        {
            step = 5; // Larger steps
        }
        else
        {
            step = 10; // Biggest steps at high brightness
        }

        if (increasing)
        {
            g_targetBrightness = min(MAX_BRIGHTNESS, g_targetBrightness + step);
        }
        else
        {
            g_targetBrightness = max(MIN_BRIGHTNESS, g_targetBrightness - step);
        }
        _taskChangeToBrightness.enableIfNot();
        break;

    case SPEED:
        setEncoderColor(obj, SPEED);
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

    case PLAYLIST:
        setEncoderColor(obj, PLAYLIST);
        if (increasing)
        {
            Serial.printf("%s: Next Pattern\n", SGN);
            changePattern();
        }
        else
        {
            Serial.printf("%s: Next Pattern\n", SGN);
            changePattern();
        }
        break;

    default:
        break;
    }
}

// Toggle between BRIGHTNESS and SPEED and PLAYLIST
void encoder_onClick(i2cEncoderLibV2 *obj)
{
    EncoderState newEncoderstate;
    if (g_encoderState == BRIGHTNESS)
    {
        newEncoderstate = SPEED;
    }
    else if (g_encoderState == SPEED)
    {
        newEncoderstate = PLAYLIST;
    }
    else if (g_encoderState == PLAYLIST)
    {
        newEncoderstate = BRIGHTNESS;
    }
    setEncoderState(obj, newEncoderstate);
}

void encoder_doubleClick(i2cEncoderLibV2 *obj)
{
    constexpr const char *SGN = "encoder_doubleClick()";
    if (g_encoderState == PLAYLIST)
    {
        if (_taskChangePattern.isEnabled())
        {
            obj->writeRGBCode(0x0000FF); // Dark Blue
            _taskChangePattern.disable();
            Serial.printf("%s: %s: Disabling change pattern\n", timeToString().c_str(), SGN);
        }
        else
        {
            obj->writeRGBCode(0xFFFFFF); // White
            _taskChangePattern.enableIfNot();
            Serial.printf("%s: %s: Initiating change pattern\n", timeToString().c_str(), SGN);
        }
    }
}

void setEncoderState(i2cEncoderLibV2 *obj, EncoderState state)
{
    setEncoderColor(obj, state);
    g_encoderState = state;
}

void setEncoderColor(i2cEncoderLibV2 *obj, EncoderState state)
{
    constexpr const char *SGN = "setEncoderColor()";

    switch (state)
    {
    case BRIGHTNESS:
        obj->writeRGBCode(0xFF0000); // Red
        Serial.printf("%s: %s: EncoderState: %s\n", timeToString().c_str(), SGN, "BRIGHTNESS");
        break;
    case SPEED:
        obj->writeRGBCode(0x00FFFF); // Cyan
        Serial.printf("%s: %s: EncoderState: %s\n", timeToString().c_str(), SGN, "SPEED");
        break;
    case PLAYLIST:
        obj->writeRGBCode(0xFFFF00); // Yellow
        Serial.printf("%s: %s: EncoderState: %s\n", timeToString().c_str(), SGN, "PLAYLIST");
        break;
    }
}
void updateEncoderIdleAnimation()
{
    // Should be the bottom most LED
    CRGB rgb = leds[12];

    // Ensure the max brightness is applied to the encoder LED
    uint8_t maxBrightness = 255;
    uint8_t r = map(rgb.r, 0, 255, 0, maxBrightness);
    uint8_t g = map(rgb.g, 0, 255, 0, maxBrightness);
    uint8_t b = map(rgb.b, 0, 255, 0, maxBrightness);

    // Update encoder LED
    RGBEncoder.writeLEDR(r);
    RGBEncoder.writeLEDG(g);
    RGBEncoder.writeLEDB(b);
}