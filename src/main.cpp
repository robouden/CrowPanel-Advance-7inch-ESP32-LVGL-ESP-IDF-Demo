/*
 * CrowPanel Advance 7" ESP32-S3 LVGL Demo
 * Arduino Framework Version
 */

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <lvgl.h>
#include "display/pins.h"
#include "display/esp32_s3_arduino.h"
#include "display/buzzer.h"
#include "touch/GT911.h"

// Debug flag
#define DEBUG_ENABLED 1

// I2C Addresses
#define PCA9557_ADDR 0x18
#define BM8563_ADDR 0x51
#define GT911_ADDR 0x5D

// Function declarations
static void init_i2c(void);
static bool check_i2c_device(uint8_t addr);
static void init_peripherals(void);
static void lvgl_task(void *pvParameters);

// Global objects
GT911 touch = GT911();

void setup() {
    // Initialize serial and wait for connection
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\nCrowPanel Advance 7\" ESP32-S3 LVGL Demo - Arduino Version");
    Serial.println("Build: " __DATE__ " " __TIME__);

    // Initialize PSRAM first
    if (psramInit()) {
        Serial.println("PSRAM initialized successfully");
        Serial.printf("Total PSRAM: %d bytes\n", ESP.getPsramSize());
        Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
    } else {
        Serial.println("PSRAM initialization failed!");
        while(1) delay(100);
    }

    // Initialize buzzer first and turn it off
    Serial.println("Initializing buzzer...");
    esp_err_t ret = buzzer_init((gpio_num_t)BUZZER_GPIO, BUZZER_LEDC_TIMER, BUZZER_LEDC_CHANNEL);
    if (ret != ESP_OK) {
        Serial.println("Failed to initialize buzzer!");
    } else {
        buzzer_stop();
        Serial.println("Buzzer initialized and stopped");
    }

    // Initialize I2C with proper delays
    init_i2c();
    
    // Initialize peripherals
    init_peripherals();
    
    // Initialize display
    init_display();
    
    // Create LVGL task
    xTaskCreatePinnedToCore(
        lvgl_task,
        "lvgl",
        8192,
        NULL,
        2,
        NULL,
        1
    );
    
    Serial.println("Setup complete!");
}

void loop() {
    // Keep the loop empty as LVGL runs in its own task
    delay(1000);
}

static void init_i2c(void) {
    Serial.println("Initializing I2C...");
    
    // Reset I2C pins to input mode first
    pinMode(I2C_SDA, INPUT);
    pinMode(I2C_SCL, INPUT);
    delay(100);
    
    // Start I2C with internal pullups
    Wire.begin(I2C_SDA, I2C_SCL, 100000);
    delay(100);
    
    // Check for I2C devices
    Serial.println("Scanning I2C devices...");
    if (check_i2c_device(PCA9557_ADDR)) {
        Serial.println("PCA9557 I/O expander found");
    } else {
        Serial.println("PCA9557 I/O expander not found!");
    }
    
    if (check_i2c_device(BM8563_ADDR)) {
        Serial.println("BM8563 RTC found");
    } else {
        Serial.println("BM8563 RTC not found!");
    }
    
    if (check_i2c_device(GT911_ADDR)) {
        Serial.println("GT911 touch controller found");
    } else {
        Serial.println("GT911 touch controller not found!");
    }
}

static bool check_i2c_device(uint8_t addr) {
    Wire.beginTransmission(addr);
    return (Wire.endTransmission() == 0);
}

static void init_peripherals(void) {
    Serial.println("Initializing peripherals...");
    
    // Initialize PCA9557 first (I/O expander)
    Wire.beginTransmission(PCA9557_ADDR);
    Wire.write(0x03); // Configuration register
    Wire.write(0x00); // Set all pins as outputs
    if (Wire.endTransmission() != 0) {
        Serial.println("Failed to configure PCA9557!");
    }
    delay(100);
    
    // Initialize touch controller
    if (touch.begin()) {
        Serial.println("GT911 touch controller initialized successfully");
        touch.setRotation(TOUCH_ROTATION_NORMAL);
        touch.setResolution(LCD_H_RES, LCD_V_RES);
    } else {
        Serial.println("Failed to initialize GT911 touch controller!");
    }
    delay(100);
}

static void lvgl_task(void *pvParameters) {
    while (1) {
        // Handle LVGL tasks
        lv_timer_handler();
        delay(5);
    }
}
