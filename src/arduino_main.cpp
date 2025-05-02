#ifdef USING_ARDUINO_FRAMEWORK

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <lvgl.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "display/display_arduino.h"

// Constants
#define TOUCH_SDA 15
#define TOUCH_SCL 16
#define TOUCH_INT 7
#define TOUCH_ADDR 0x5D  // GT911 address

// Event bits (defined in display_arduino.h)
#define DISPLAY_READY_BIT   (1 << 0)
#define UI_READY_BIT        (1 << 1)
#define DISPLAY_WORKING_BIT (1 << 2)

// External declarations
extern SemaphoreHandle_t lvgl_mux;
extern EventGroupHandle_t app_events;

// Forward declarations
void ui_init(void);
void ui_tick(void);

// LVGL timer handler task
void lvglTask(void *parameter)
{
    Serial.println("LVGL update task started");
    
    // Wait for both display and UI to be ready
    if (app_events != NULL) {
        xEventGroupWaitBits(app_events, 
                          DISPLAY_READY_BIT | UI_READY_BIT,
                          pdFALSE, pdTRUE, portMAX_DELAY);
    }
    
    Serial.println("LVGL update task created successfully");
    uint32_t updateCount = 0;
    
    while (1) {
        // Take mutex to access LVGL
        if (lvgl_mux != NULL && xSemaphoreTake(lvgl_mux, pdMS_TO_TICKS(100)) == pdTRUE) {
            // Update LVGL timer
            lv_timer_handler();
            
            // Call UI tick function
            ui_tick();
            
            // Release mutex
            xSemaphoreGive(lvgl_mux);
            
            // Set display working bit
            notify_display_working();
            
            // Occasionally log status
            if (++updateCount % 1000 == 0) {
                Serial.printf("LVGL update #%u\n", (unsigned int)updateCount);
            }
        }
        
        // Small delay between updates
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

// Initialize touch controller
void init_touch(void)
{
    Serial.println("Initializing touch controller...");
    
    // Initialize I2C for touch
    Wire.begin(TOUCH_SDA, TOUCH_SCL);
    Wire.setClock(100000);
    
    // Basic test to confirm touch controller is responding
    Wire.beginTransmission(TOUCH_ADDR);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
        Serial.println("GT911 touch controller initialized successfully");
    } else {
        Serial.println("Touch controller not found, check wiring and address");
    }
}

// Initialize UI
void init_ui(void)
{
    Serial.println("Initializing UI...");
    
    // Take the mutex for LVGL operations during UI init
    if (lvgl_mux != NULL && xSemaphoreTake(lvgl_mux, pdMS_TO_TICKS(1000)) == pdTRUE) {
        // Call LVGL UI init function
        ui_init();
        
        // Release mutex
        xSemaphoreGive(lvgl_mux);
        
        Serial.println("UI initialization complete");
        
        // Signal that UI is ready
        if (app_events != NULL) {
            xEventGroupSetBits(app_events, UI_READY_BIT);
        }
    } else {
        Serial.println("Failed to take mutex for UI initialization");
    }
}

// Arduino setup function
void setup() {
    // Initialize serial
    Serial.begin(115200);
    delay(500); // Give some time for serial to connect
    
    Serial.println("CrowPanel Advance 7\" ESP32-S3 LVGL Demo - Arduino Version");
    
    // Initialize hardware
    init_touch();
    init_display();  // Defined in esp32_s3_arduino.cpp
    
    Serial.println("Initializing GUI with Arduino framework");
    init_ui();
    
    Serial.println("Display and UI initialized");
    
    // Create LVGL timer task
    xTaskCreatePinnedToCore(
        lvglTask,          // Task function
        "lvglTask",        // Name
        8192,             // Stack size (bytes)
        NULL,             // Parameter
        1,                // Priority
        NULL,             // Task handle
        1                 // Core (1 = Arduino loop core)
    );
}

// Arduino loop function
void loop() {
    // The main LVGL work is done in the lvglTask
    // Keep this loop empty to avoid conflicts
    delay(1000);
}

#endif // USING_ARDUINO_FRAMEWORK 