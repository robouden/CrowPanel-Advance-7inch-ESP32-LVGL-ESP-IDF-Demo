#include <Arduino.h>
#include <SPI.h>
#include <lvgl.h>
#include <Wire.h>
#include "pins.h"
#include "elecrow_advanced_7inch_800x480.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp32_s3_arduino.h"

static const char* TAG = "DISPLAY";

// Display buffer in PSRAM
static lv_disp_draw_buf_t disp_buf;
static lv_color_t *buf1 = NULL;
static lv_color_t *buf2 = NULL;

// Display driver instance
static lv_disp_drv_t disp_drv;

// RGB panel configuration
static esp_lcd_panel_handle_t panel_handle = NULL;

// Define the LVGL mutex
SemaphoreHandle_t lvgl_mux = NULL;

// LVGL update task
static void lvgl_task(void *pvParameters) {
    Serial.println("LVGL task started");
    
    while (1) {
        // Take mutex to access LVGL
        if (xSemaphoreTake(lvgl_mux, pdMS_TO_TICKS(100)) == pdTRUE) {
            // Update LVGL timer
            lv_timer_handler();
            
            // Release mutex
            xSemaphoreGive(lvgl_mux);
        }
        
        // Small delay between updates (20ms = 50Hz refresh rate)
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// Display I2C address (this is a placeholder - you need to check the actual address)
#define DISPLAY_I2C_ADDR 0x38

// Display controller registers (these are placeholders - you need to check the actual registers)
#define DISPLAY_REG_POWER      0x01
#define DISPLAY_REG_BRIGHTNESS 0x02
#define DISPLAY_REG_WINDOW_X   0x03
#define DISPLAY_REG_WINDOW_Y   0x04
#define DISPLAY_REG_WINDOW_W   0x05
#define DISPLAY_REG_WINDOW_H   0x06
#define DISPLAY_REG_DATA       0x10

// Function to write a command to the display
static bool display_write_cmd(uint8_t reg, uint8_t* data, size_t len) {
    Wire.beginTransmission(DISPLAY_I2C_ADDR);
    Wire.write(reg);
    for (size_t i = 0; i < len; i++) {
        Wire.write(data[i]);
    }
    return (Wire.endTransmission() == 0);
}

// Function to read data from the display
static bool display_read_data(uint8_t reg, uint8_t* data, size_t len) {
    Wire.beginTransmission(DISPLAY_I2C_ADDR);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) {
        return false;
    }
    
    Wire.requestFrom(DISPLAY_I2C_ADDR, len);
    if (Wire.available() != len) {
        return false;
    }
    
    for (size_t i = 0; i < len; i++) {
        data[i] = Wire.read();
    }
    
    return true;
}

// Function to flush the display buffer
static void display_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
    esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_map);
    lv_disp_flush_ready(drv);
}

extern "C" {

// Function to set display brightness
void set_display_brightness(uint8_t brightness) {
    Serial.printf("Setting display brightness to %d%%\n", brightness);
    // Note: This display doesn't have brightness control
}

// Function to initialize the display
void init_display(void) {
    Serial.println("Initializing display...");

    // Step 1: Allocate display buffers from PSRAM
    Serial.println("Allocating display buffers from PSRAM...");
    buf1 = (lv_color_t *)ps_malloc(LCD_H_RES * 100 * sizeof(lv_color_t));
    buf2 = (lv_color_t *)ps_malloc(LCD_H_RES * 100 * sizeof(lv_color_t));
    
    if (!buf1 || !buf2) {
        Serial.println("Failed to allocate display buffers!");
        return;
    }
    Serial.println("Display buffers allocated successfully");

    // Step 2: Initialize LVGL
    Serial.println("Initializing LVGL...");
    lv_init();
    
    // Step 3: Initialize LVGL display buffer
    Serial.println("Initializing LVGL display buffer...");
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, LCD_H_RES * 100);

    // Step 4: Configure RGB LCD panel
    Serial.println("Configuring RGB LCD panel...");
    esp_lcd_rgb_panel_config_t panel_config;
    memset(&panel_config, 0, sizeof(panel_config));

    panel_config.clk_src = LCD_CLK_SRC_PLL160M;
    panel_config.timings.pclk_hz = LCD_PIXEL_CLOCK_HZ;
    panel_config.timings.h_res = LCD_H_RES;
    panel_config.timings.v_res = LCD_V_RES;
    panel_config.timings.hsync_pulse_width = HSYNC_PULSE_WIDTH;
    panel_config.timings.hsync_back_porch = HSYNC_BACK_PORCH;
    panel_config.timings.hsync_front_porch = HSYNC_FRONT_PORCH;
    panel_config.timings.vsync_pulse_width = VSYNC_PULSE_WIDTH;
    panel_config.timings.vsync_back_porch = VSYNC_BACK_PORCH;
    panel_config.timings.vsync_front_porch = VSYNC_FRONT_PORCH;
    panel_config.timings.flags.pclk_active_neg = true;
    
    panel_config.data_width = 16; // RGB565
    panel_config.psram_trans_align = 64;
    panel_config.hsync_gpio_num = PIN_NUM_HSYNC;
    panel_config.vsync_gpio_num = PIN_NUM_VSYNC;
    panel_config.de_gpio_num = PIN_NUM_DE;
    panel_config.pclk_gpio_num = PIN_NUM_PCLK;
    panel_config.data_gpio_nums[0] = PIN_NUM_DATA0;
    panel_config.data_gpio_nums[1] = PIN_NUM_DATA1;
    panel_config.data_gpio_nums[2] = PIN_NUM_DATA2;
    panel_config.data_gpio_nums[3] = PIN_NUM_DATA3;
    panel_config.data_gpio_nums[4] = PIN_NUM_DATA4;
    panel_config.data_gpio_nums[5] = PIN_NUM_DATA5;
    panel_config.data_gpio_nums[6] = PIN_NUM_DATA6;
    panel_config.data_gpio_nums[7] = PIN_NUM_DATA7;
    panel_config.data_gpio_nums[8] = PIN_NUM_DATA8;
    panel_config.data_gpio_nums[9] = PIN_NUM_DATA9;
    panel_config.data_gpio_nums[10] = PIN_NUM_DATA10;
    panel_config.data_gpio_nums[11] = PIN_NUM_DATA11;
    panel_config.data_gpio_nums[12] = PIN_NUM_DATA12;
    panel_config.data_gpio_nums[13] = PIN_NUM_DATA13;
    panel_config.data_gpio_nums[14] = PIN_NUM_DATA14;
    panel_config.data_gpio_nums[15] = PIN_NUM_DATA15;

    // Step 5: Initialize RGB LCD panel
    Serial.println("Initializing RGB LCD panel...");
    esp_err_t ret = esp_lcd_new_rgb_panel(&panel_config, &panel_handle);
    if (ret != ESP_OK) {
        Serial.printf("Failed to initialize RGB LCD panel: %d\n", ret);
        return;
    }

    // Step 6: Reset panel
    Serial.println("Resetting LCD panel...");
    ret = esp_lcd_panel_reset(panel_handle);
    if (ret != ESP_OK) {
        Serial.printf("Failed to reset panel: %d\n", ret);
        return;
    }
    delay(100); // Wait for reset to complete

    // Step 7: Initialize panel
    Serial.println("Initializing LCD panel...");
    ret = esp_lcd_panel_init(panel_handle);
    if (ret != ESP_OK) {
        Serial.printf("Failed to initialize panel: %d\n", ret);
        return;
    }
    delay(100); // Wait for initialization to complete

    // Step 8: Turn on display
    Serial.println("Turning on display...");
    ret = esp_lcd_panel_disp_on_off(panel_handle, true);
    if (ret != ESP_OK) {
        Serial.printf("Failed to turn on display: %d\n", ret);
        return;
    }

    // Step 9: Register display driver
    Serial.println("Registering display driver...");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_H_RES;
    disp_drv.ver_res = LCD_V_RES;
    disp_drv.flush_cb = display_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = NULL;
    lv_disp_drv_register(&disp_drv);

    // Create LVGL mutex
    lvgl_mux = xSemaphoreCreateMutex();
    if (!lvgl_mux) {
        Serial.println("Failed to create LVGL mutex!");
        return;
    }

    // Create LVGL task
    Serial.println("Creating LVGL task...");
    xTaskCreatePinnedToCore(
        lvgl_task,
        "lvgl",
        8192,
        NULL,
        2,
        NULL,
        1
    );

    Serial.println("Display initialized successfully");
}

// Function to initialize the touch controller
void touch_init() {
    Serial.println("Initializing touch controller using Arduino framework");
    // Touch controller is initialized separately in main.cpp using GT911 class
}

// Function to turn display on/off
void set_display_power(bool on) {
    Serial.printf("Setting display power %s\n", on ? "ON" : "OFF");
    if (panel_handle) {
        esp_lcd_panel_disp_on_off(panel_handle, on);
    }
}

// Function to get display power state
bool get_display_power() {
    return true; // Always return true as we can't read the power state
}

// Function to set the display window for drawing
bool set_display_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    if (panel_handle) {
        esp_err_t ret = esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + width, y + height, NULL);
        return (ret == ESP_OK);
    }
    return false;
}

// Function to write pixel data to the display
bool write_display_data(const uint8_t* data, size_t len) {
    if (panel_handle) {
        // Note: The actual data writing is handled by LVGL's flush callback
        return true;
    }
    return false;
}

void display_task(void *pvParameters) {
    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

} // extern "C"
