/*
 * CrowPanel Advance 7" ESP32-S3 LVGL Demo
 * Arduino Framework Version
 */

#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>

// Include UI files
#include "ui/generated/src/ui/ui.h"
#include "ui/generated/src/ui/screens.h"
#include "ui/generated/src/ui/vars.h"
#include "ui/generated/src/ui/actions.h"
#include "touch/GT911.h"
#include "display/pins.h"
#include "display/esp32_s3_arduino.h"
#include "gui/gui_arduino.h"
#include "display/elecrow_advanced_7inch_800x480.h"

GT911 touch = GT911();
bool touch_debug_enabled = true;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(1000); // Give serial time to initialize
    
    Serial.println("\n\nCrowPanel Advance 7\" ESP32-S3 LVGL Demo - Arduino Version");
    Serial.println("Build: " __DATE__ " " __TIME__);
    
    // Initialize PSRAM
    if (psramInit()) {
        Serial.println("PSRAM initialized successfully");
        Serial.printf("Total PSRAM: %d bytes\n", ESP.getPsramSize());
        Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
    } else {
        Serial.println("PSRAM initialization failed!");
        while(1) delay(100); // Stop here if PSRAM fails
    }

    // Initialize I2C for touch controller
    Serial.println("Initializing I2C...");
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(TOUCH_I2C_FREQ);
    delay(100); // Give I2C time to stabilize
    
    Serial.println("Initializing touch controller...");
    if (touch.begin()) {
        Serial.println("GT911 touch controller initialized successfully");
        
        // Configure touch controller
        touch.setRotation(ROTATION_NORMAL);
        touch.setResolution(LCD_H_RES, LCD_V_RES);
        Serial.println("Touch configuration complete");
    } else {
        Serial.println("Failed to initialize GT911 touch controller!");
        // Continue anyway as display might still work
    }

    // Initialize display
    Serial.println("Initializing display...");
    init_display();
    delay(100); // Give display time to initialize
    
    // Initialize LVGL
    Serial.println("Initializing LVGL...");
    lv_init();
    
    // Create LVGL update task
    Serial.println("Creating LVGL task...");
    xTaskCreatePinnedToCore(
        display_task,
        "lvgl",
        8192, // Increased stack size
        NULL,
        2,
        NULL,
        1
    );
    
    // Initialize the UI
    Serial.println("Initializing UI...");
    ui_init(); // This will call create_screens() internally
    Serial.println("UI initialization complete");

    // Final status
    Serial.println("Setup complete!");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
}

void loop() {
    static uint32_t last_print = 0;
    static uint32_t frame_count = 0;
    frame_count++;
    
    // Update touch data
    if (touch.available()) {
        if (touch_debug_enabled) {
            // Print touch debug information every second
            if (millis() - last_print > 1000) {
                Serial.printf("Touch points: %d\n", touch.getTouchPoints());
                for (uint8_t i = 0; i < touch.getTouchPoints(); i++) {
                    TouchPoint point = touch.getPoint(i);
                    Serial.printf("Point %d - X: %d, Y: %d, Size: %d\n", 
                        i + 1,
                        point.x,
                        point.y,
                        point.size
                    );
                }
                // Print frame statistics
                Serial.printf("Frames last second: %d\n", frame_count);
                frame_count = 0;
                last_print = millis();
            }
        }
    }

    // Small delay to prevent watchdog reset
    delay(5);
}
