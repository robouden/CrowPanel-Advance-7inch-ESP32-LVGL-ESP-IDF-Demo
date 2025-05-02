/*
 * CrowPanel Advance 7" ESP32-S3 LVGL Demo
 * Arduino Framework Version
 */

#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>

// Include UI files
#include "ui/ui.h"
#include "ui/ui_init.h"
#include "ui/vars.h"
#include "ui/actions.h"
#include "touch/GT911.h"
#include "display/pins.h"
#include "display/esp32_s3_arduino.h"
#include "gui/gui_arduino.h"

// Display definitions
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 480

// I2C addresses for display and touch
#define DISPLAY_I2C_ADDR 0x38

// TFT instance
// TFT_eSPI tft = TFT_eSPI();

// LVGL display buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

// Touch controller
GT911 touch;

// Touch variables
static lv_indev_drv_t indev_drv;
static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);

// Mutex for LVGL (defined in esp32_s3_arduino.cpp)
extern SemaphoreHandle_t lvgl_mux;

// Display flush callback for I2C display
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
    // For I2C display, we would need to send the pixel data via I2C
    // This is a simplified implementation and would need to be adapted to your specific display
    
    // Signal to LVGL that the flushing is done
    lv_disp_flush_ready(disp);
}

// Read touch points using GT911 controller
static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
    gt911_touch_point_t points[5];
    uint8_t count = 0;
    
    if (touch.readTouchPoints(points, &count) && count > 0) {
        // Use the first touch point
        data->state = LV_INDEV_STATE_PR;
        data->point.x = points[0].x;
        data->point.y = points[0].y;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("CrowPanel Advance 7\" ESP32-S3 LVGL Demo - Arduino Version");
    
    // Initialize I2C with pins from elecrow_advanced_7inch_800x480.h
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(400000); // Set I2C clock to 400kHz for faster communication
    
    Serial.println("Initializing touch controller...");
    if (touch.begin()) {
        Serial.println("GT911 touch controller initialized successfully");
    } else {
        Serial.println("Failed to initialize GT911 touch controller!");
        // Continue anyway, as we might still want to use the display without touch
    }
    
    Serial.println("Initializing display and touch...");
    // Initialize display and touch using our Arduino implementation
    display_init();
    
    // Initialize touch input device
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    lv_indev_drv_register(&indev_drv);
    
    // Initialize GUI using our Arduino implementation
    gui_init();
    
    Serial.println("Display and UI initialized");
}

void loop() {
    // The main LVGL work is done in the lvgl_task
    // Keep this loop empty to avoid conflicts
    delay(1000);
}
