#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#include "driver/gpio.h"

// Display Configuration
#define LCD_PIXEL_CLOCK_HZ     (18 * 1000 * 1000)
#define LCD_H_RES              800
#define LCD_V_RES              480

// RGB Interface Pins (from working configuration)
#define PIN_NUM_HSYNC          GPIO_NUM_40
#define PIN_NUM_VSYNC          GPIO_NUM_41
#define PIN_NUM_DE             GPIO_NUM_42
#define PIN_NUM_PCLK           GPIO_NUM_39

// RGB Data Pins
// Blue Channel
#define PIN_NUM_B0             GPIO_NUM_21
#define PIN_NUM_B1             GPIO_NUM_47
#define PIN_NUM_B2             GPIO_NUM_48
#define PIN_NUM_B3             GPIO_NUM_45
#define PIN_NUM_B4             GPIO_NUM_38

// Green Channel
#define PIN_NUM_G0             GPIO_NUM_9
#define PIN_NUM_G1             GPIO_NUM_10
#define PIN_NUM_G2             GPIO_NUM_11
#define PIN_NUM_G3             GPIO_NUM_12
#define PIN_NUM_G4             GPIO_NUM_13
#define PIN_NUM_G5             GPIO_NUM_14

// Red Channel
#define PIN_NUM_R0             GPIO_NUM_7
#define PIN_NUM_R1             GPIO_NUM_17
#define PIN_NUM_R2             GPIO_NUM_18
#define PIN_NUM_R3             GPIO_NUM_3
#define PIN_NUM_R4             GPIO_NUM_46

// Display Timing (from working configuration)
#define HSYNC_BACK_PORCH       8
#define HSYNC_FRONT_PORCH      8
#define HSYNC_PULSE_WIDTH      4
#define VSYNC_BACK_PORCH       8
#define VSYNC_FRONT_PORCH      8
#define VSYNC_PULSE_WIDTH      4

// Touch Configuration
#define TOUCH_I2C_SCL          GPIO_NUM_16
#define TOUCH_I2C_SDA          GPIO_NUM_15
#define TOUCH_INT_GPIO         GPIO_NUM_7
#define TOUCH_RST_GPIO         GPIO_NUM_NC  // No dedicated reset pin, handled by PCA9557
#define TOUCH_I2C_FREQ         400000
#define TOUCH_I2C_ADDR         0x5D    // GT911 address
#define TOUCH_READ_INTERVAL_MS 20      // Touch read interval

// Touch Calibration (if needed)
#define TOUCH_CALIB_X_MIN      0
#define TOUCH_CALIB_X_MAX      LCD_H_RES
#define TOUCH_CALIB_Y_MIN      0
#define TOUCH_CALIB_Y_MAX      LCD_V_RES

// Display Buffer Configuration
#define DISP_BUF_SIZE          (LCD_H_RES * 40)
#define LVGL_TICK_PERIOD_MS    2

// LVGL Task Configuration
#define LVGL_TASK_DELAY_MS     10
#define LVGL_TASK_STACK_SIZE   (4 * 1024)
#define LVGL_TASK_PRIORITY     0

#endif // DISPLAY_CONFIG_H 