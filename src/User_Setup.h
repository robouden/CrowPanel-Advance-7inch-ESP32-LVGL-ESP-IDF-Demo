// User_Setup.h for CrowPanel Advance 7" ESP32-S3
// Configuration for TFT_eSPI based on elecrow_advanced_7inch_800x480.h

#define USER_SETUP_INFO "User_Setup for CrowPanel Advance 7inch"

// Define the display driver chip
#define ST7701_DRIVER

// Define the display resolution
#define TFT_WIDTH  800
#define TFT_HEIGHT 480

// RGB Interface pins from elecrow_advanced_7inch_800x480.h
#define TFT_HSYNC 40  // PIN_NUM_HSYNC
#define TFT_VSYNC 41  // PIN_NUM_VSYNC
#define TFT_DE    42  // PIN_NUM_DE
#define TFT_PCLK  39  // PIN_NUM_PCLK

// Data pins - RGB interface
// Red pins
#define TFT_R0    7   // PIN_NUM_DATA11
#define TFT_R1    17  // PIN_NUM_DATA12
#define TFT_R2    18  // PIN_NUM_DATA13
#define TFT_R3    3   // PIN_NUM_DATA14
#define TFT_R4    46  // PIN_NUM_DATA15

// Green pins
#define TFT_G0    9   // PIN_NUM_DATA5
#define TFT_G1    10  // PIN_NUM_DATA6
#define TFT_G2    11  // PIN_NUM_DATA7
#define TFT_G3    12  // PIN_NUM_DATA8
#define TFT_G4    13  // PIN_NUM_DATA9
#define TFT_G5    14  // PIN_NUM_DATA10

// Blue pins
#define TFT_B0    21  // PIN_NUM_DATA0
#define TFT_B1    47  // PIN_NUM_DATA1
#define TFT_B2    48  // PIN_NUM_DATA2
#define TFT_B3    45  // PIN_NUM_DATA3
#define TFT_B4    38  // PIN_NUM_DATA4

// I2C pins for touch and other peripherals
#define I2C_SDA   15
#define I2C_SCL   16

// Use ESP32 DMA for display updates
#define ESP32_DMA
#define DMA_QUEUE_SIZE 8

// Enable PSRAM support
#define SUPPORT_TRANSACTIONS
#define SUPPORT_PSRAM

// Timing parameters
#define TFT_HSYNC_BACK_PORCH    8
#define TFT_HSYNC_FRONT_PORCH   8
#define TFT_HSYNC_PULSE_WIDTH   4
#define TFT_VSYNC_BACK_PORCH    8
#define TFT_VSYNC_FRONT_PORCH   8
#define TFT_VSYNC_PULSE_WIDTH   4

// LVGL task settings
#define LVGL_TASK_DELAY_MS      10
#define LVGL_TASK_STACK_SIZE    (4 * 1024)
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

// Smooth fonts
#define SMOOTH_FONT

// Don't rotate the screen by default
#define CGRAM_OFFSET
