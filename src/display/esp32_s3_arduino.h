#ifndef ESP32_S3_ARDUINO_H
#define ESP32_S3_ARDUINO_H

#include <Arduino.h>
#include <lvgl.h>
#include "esp_lcd_panel_rgb.h"

#ifdef __cplusplus
extern "C" {
#endif

// Function declarations
void init_display(void);
void display_task(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif // ESP32_S3_ARDUINO_H
