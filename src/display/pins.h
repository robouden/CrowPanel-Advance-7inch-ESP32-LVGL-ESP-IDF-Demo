#ifndef DISPLAY_PINS_H
#define DISPLAY_PINS_H

// Display resolution
#define LCD_H_RES           800
#define LCD_V_RES           480
#define LCD_PIXEL_CLOCK_HZ  (18 * 1000 * 1000)

// RGB Interface pins
#define PIN_NUM_HSYNC       40  // HSYNC control signal
#define PIN_NUM_VSYNC       41  // VSYNC control signal
#define PIN_NUM_DE          42  // Data Enable control signal
#define PIN_NUM_PCLK        39  // Pixel clock

// Data pins - RGB interface
// Red pins
#define PIN_NUM_DATA11      7   // R0
#define PIN_NUM_DATA12      17  // R1
#define PIN_NUM_DATA13      18  // R2
#define PIN_NUM_DATA14      3   // R3
#define PIN_NUM_DATA15      46  // R4

// Green pins
#define PIN_NUM_DATA5       9   // G0
#define PIN_NUM_DATA6       10  // G1
#define PIN_NUM_DATA7       11  // G2
#define PIN_NUM_DATA8       12  // G3
#define PIN_NUM_DATA9       13  // G4
#define PIN_NUM_DATA10      14  // G5

// Blue pins
#define PIN_NUM_DATA0       21  // B0
#define PIN_NUM_DATA1       47  // B1
#define PIN_NUM_DATA2       48  // B2
#define PIN_NUM_DATA3       45  // B3
#define PIN_NUM_DATA4       38  // B4

// I2C pins
#define I2C_SDA            15
#define I2C_SCL            16
#define I2C_CLK_SPEED_HZ   100000

// Touch controller
#define TOUCH_I2C_ADDR     0x5D
#define TOUCH_I2C_FREQ     100000
#define TOUCH_SDA          I2C_SDA
#define TOUCH_SCL          I2C_SCL
#define TOUCH_RST          -1
#define TOUCH_INT          -1

// Display timing
#define HSYNC_PULSE_WIDTH   4
#define HSYNC_BACK_PORCH    8
#define HSYNC_FRONT_PORCH   8
#define VSYNC_PULSE_WIDTH   4
#define VSYNC_BACK_PORCH    8
#define VSYNC_FRONT_PORCH   8

// Display buffer size (in pixels)
#define DISP_BUF_SIZE      (LCD_H_RES * 100)

// LVGL task settings
#define LVGL_TASK_STACK_SIZE 8192
#define LVGL_TASK_PRIORITY   2

// Rotation settings
#define ROTATION_NORMAL     0
#define ROTATION_90        90
#define ROTATION_180      180
#define ROTATION_270      270

#endif // DISPLAY_PINS_H
