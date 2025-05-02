#ifndef PINS_H
#define PINS_H

// I2C Pins
#define I2C_SDA 15
#define I2C_SCL 16

// Touch Configuration
#define TOUCH_I2C_SDA I2C_SDA
#define TOUCH_I2C_SCL I2C_SCL
#define TOUCH_I2C_FREQ 400000
#define TOUCH_INT_PIN 7
#define TOUCH_RST_PIN -1  // No dedicated reset pin, handled by PCA9557

// Display Configuration
#define LCD_PIXEL_CLOCK_HZ     (18 * 1000 * 1000)
#define LCD_H_RES              800
#define LCD_V_RES              480

// Display Rotation
#define ROTATION_NORMAL 0
#define ROTATION_90     1
#define ROTATION_180    2
#define ROTATION_270    3

#endif // PINS_H
