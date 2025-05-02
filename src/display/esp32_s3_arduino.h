#ifndef ESP32_S3_ARDUINO_H
#define ESP32_S3_ARDUINO_H

#include <Arduino.h>
#include <lvgl.h>
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "pins.h"

#ifdef __cplusplus
extern "C" {
#endif

// Function declarations
void init_display(void);
void display_task(void *pvParameters);
void set_display_brightness(uint8_t brightness);
void set_display_power(bool on);
bool get_display_power(void);

#ifdef __cplusplus
}
#endif

#endif // ESP32_S3_ARDUINO_H
