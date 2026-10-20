// M5Stack Atom S3 Lite (ESP32-S3): the current Uzumaru controller.
// Selected with -D UZUMARU_BOARD_ATOM_S3 (env m5stack_atomlite_s3), included from config.h.
#ifndef BOARD_ATOM_S3_H
#define BOARD_ATOM_S3_H

#define BOARD_NAME "M5Stack Atom S3 Lite"
#define DATA_PIN 8 // LED data out
#define I2C_SDA_PIN 5
#define I2C_SCL_PIN 6
#define I2C_INT_PIN 7
#define MAX_POWER_MILLIAMPS 1500 // power budget for the LEDs

#endif
