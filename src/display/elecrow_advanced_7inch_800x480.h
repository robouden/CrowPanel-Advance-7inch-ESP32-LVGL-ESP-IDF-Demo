#ifndef ELECROW_ADVANCED_7INCH_800X480_H
#define ELECROW_ADVANCED_7INCH_800X480_H

#include "pins.h"

// Buzzer Configuration
#define BUZZER_GPIO GPIO_NUM_8 
#define BUZZER_LEDC_TIMER LEDC_TIMER_1
#define BUZZER_LEDC_CHANNEL LEDC_CHANNEL_1
#define BUZZER_FREQ 0 // Set to 0 to disable buzzer initially

// I2C
#define I2C_RST          GPIO_NUM_NC
#define I2C_CLK_SPEED_HZ 400000
#define I2C_NUM          I2C_NUM_0

// LCD
#define LCD_PIXEL_CLOCK_HZ     (18 * 1000 * 1000)

// Display timing parameters
#define HSYNC_PULSE_WIDTH    4
#define HSYNC_BACK_PORCH    8
#define HSYNC_FRONT_PORCH   8
#define VSYNC_PULSE_WIDTH   4
#define VSYNC_BACK_PORCH    8
#define VSYNC_FRONT_PORCH   8

// RGB interface pins
#define PIN_NUM_PCLK        GPIO_NUM_47
#define PIN_NUM_VSYNC       GPIO_NUM_41
#define PIN_NUM_HSYNC       GPIO_NUM_39
#define PIN_NUM_DE          GPIO_NUM_45

#define PIN_NUM_DATA0       GPIO_NUM_21
#define PIN_NUM_DATA1       GPIO_NUM_14
#define PIN_NUM_DATA2       GPIO_NUM_13
#define PIN_NUM_DATA3       GPIO_NUM_12
#define PIN_NUM_DATA4       GPIO_NUM_11
#define PIN_NUM_DATA5       GPIO_NUM_10
#define PIN_NUM_DATA6       GPIO_NUM_9
#define PIN_NUM_DATA7       GPIO_NUM_8
#define PIN_NUM_DATA8       GPIO_NUM_18
#define PIN_NUM_DATA9       GPIO_NUM_17
#define PIN_NUM_DATA10      GPIO_NUM_16
#define PIN_NUM_DATA11      GPIO_NUM_15
#define PIN_NUM_DATA12      GPIO_NUM_7
#define PIN_NUM_DATA13      GPIO_NUM_6
#define PIN_NUM_DATA14      GPIO_NUM_5
#define PIN_NUM_DATA15      GPIO_NUM_4

#define LCD_H_RES         800
#define LCD_V_RES         480

// LVGL
#define LVGL_TASK_DELAY_MS   10
#define LVGL_TASK_STACK_SIZE (4 * 1024)
#define LVGL_TASK_PRIORITY   0

#endif // ELECROW_ADVANCED_7INCH_800X480_H