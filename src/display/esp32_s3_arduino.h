#ifndef ESP32_S3_ARDUINO_H
#define ESP32_S3_ARDUINO_H

#include <Arduino.h>

// Arduino implementation of the ESP-IDF display driver functions
// This replaces the ESP-IDF specific esp32_s3.h file

// Function to initialize the display
void display_init();

// Function to initialize the touch controller
void touch_init();

// Function to set display brightness
void set_display_brightness(uint8_t brightness);

// Function to turn display on/off
void set_display_power(bool on);

// Function to get display power state
bool get_display_power();

#endif // ESP32_S3_ARDUINO_H
