// Uzumaru hardware configuration and tuning constants.
#ifndef CONFIG_H
#define CONFIG_H

// --- Hardware: M5Stack Atom Matrix (ESP32-PICO) driving a Splendida 256 PCB ---
#define DATA_PIN 22      // LED data out
#define LED_TYPE WS2812B // leds type
#define COLOR_ORDER GRB  // color order of leds
#define NUM_LEDS 256

// DuPPa I2C RGB rotary encoder (https://www.duppa.net/)
#define I2C_SDA_PIN 33
#define I2C_SCL_PIN 23
#define I2C_INT_PIN 19
#define ENCODER_ADDRESS 0x28

// --- Power & brightness ---
#define MAX_POWER_MILLIAMPS 800 // write here your power in milliamps. default i set 800 mA for safety
#define MIN_BRIGHTNESS 1        // min brightness of leds
#define MAX_BRIGHTNESS 250      // max brightness of leds - will be limited by max power
#define DEFAULT_BRIGHTNESS 50   // default brightness

// --- Lookup table resolutions (see tables.h) ---
#define NUM_COLS_PLANAR 20 // resolution of planar lookup table
#define NUM_ROWS_PLANAR 20 // resolution of planar lookup table
#define NUM_LEDS_PLANAR NUM_COLS_PLANAR *NUM_ROWS_PLANAR

#define NUM_COLS_CYLINDER 45 // resolution of cylindrical lookup table
#define NUM_ROWS_CYLINDER 11 // resolution of cinindrical lookup table

// --- Animation ---
#define MAX_ANIMATION_SPEED 0.4f
#define MIN_ANIMATION_SPEED -0.4f
#define DEFAULT_ANIMATION_SPEED 0.08f
#define ANIMATION_SPEED_STEP 0.01f

#define SECONDS_PER_PALETTE 19
#define SECONDS_PER_PATTERN 123
#define BLEND_SPEED 16
#define BLEND_INTERVAL_MS 40
#define CROSSFADE_TIME 4000 // milliseconds
#define CROSSFADE_STEPS 255 // 255 = max (8 bit)

// Now you can see how long the crossfade is still active
#define ENCODER_ANIMATION_IDLE_TIMEOUT CROSSFADE_TIME

#endif // CONFIG_H
