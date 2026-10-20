// M5Stack Atom Lite / Atom Matrix (ESP32-PICO-D4): the original Uzumaru controller.
// Selected with -D UZUMARU_BOARD_ATOM (env m5stack_atomlite), included from config.h.
#ifndef BOARD_ATOM_H
#define BOARD_ATOM_H

#define BOARD_NAME "M5Stack Atom Lite"
#define DATA_PIN 22 // LED data out
#define I2C_SDA_PIN 33
#define I2C_SCL_PIN 23
#define I2C_INT_PIN 19
#define MAX_POWER_MILLIAMPS 800 // power budget for the LEDs

#endif
