#ifndef ESP32_S3_ARDUINO_H
#define ESP32_S3_ARDUINO_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

// LVGL mutex
extern SemaphoreHandle_t lvgl_mux;

// Display control functions
void display_init(void);
void touch_init(void);
void set_display_brightness(uint8_t brightness);
void set_display_power(bool on);
bool get_display_power(void);
bool set_display_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
bool write_display_data(const uint8_t* data, size_t len);

// Internal helper functions
bool display_write_cmd(uint8_t reg, uint8_t* data, size_t len);
bool display_read_data(uint8_t reg, uint8_t* data, size_t len);

#ifdef __cplusplus
}
#endif

#endif // ESP32_S3_ARDUINO_H
