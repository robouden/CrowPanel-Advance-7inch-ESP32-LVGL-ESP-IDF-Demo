#include <stdio.h>

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "display/esp32_s3.h"
#include "lvgl.h"
#include "ui/ui_init.h"

static const char *TAG = "MAIN";
static const char *LVGL_TASK_TAG = "LVGL_TASK"; // Tag for task logs

extern SemaphoreHandle_t lvgl_mux;
static QueueHandle_t debug_queue = NULL;

// Function to toggle touch debugging
void toggle_touch_debug(bool enable) {
    if (debug_queue != NULL) {
        uint8_t state = enable ? 1 : 0;
        xQueueSend(debug_queue, &state, 0);
        ESP_LOGI(TAG, "Touch debug %s", enable ? "enabled" : "disabled");
    }
}

// Task for touch debugging
static void touch_debug_task(void *pvParameters) {
    // Create queue for touch debug control
    debug_queue = xQueueCreate(1, sizeof(uint8_t));
    if (debug_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create debug queue");
        vTaskDelete(NULL);
        return;
    }

    uint8_t debug_enabled = 0;  // Disabled by default
    uint32_t consecutive_reads = 0;
    const uint32_t MAX_CONSECUTIVE_READS = 50; // Limit consecutive touch reads

    while (1) {
        // Check if we should toggle debug output
        uint8_t new_state;
        if (xQueueReceive(debug_queue, &new_state, 0) == pdTRUE) {
            debug_enabled = new_state;
            consecutive_reads = 0;
        }

        if (debug_enabled) {
            debug_touch_info();
            consecutive_reads++;
            
            // If we've had too many consecutive reads, pause debugging temporarily
            if (consecutive_reads >= MAX_CONSECUTIVE_READS) {
                ESP_LOGW(TAG, "Too many consecutive touch reads, pausing debug for 1 second");
                debug_enabled = 0;
                consecutive_reads = 0;
                vTaskDelay(pdMS_TO_TICKS(1000));
                debug_enabled = 1;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200)); // Reduced frequency of checks
    }
}

// LVGL update task
static void lvgl_update_task(void *pvParameters) {
    ESP_LOGI(LVGL_TASK_TAG, "Task started"); 
    while (1) {
        // Try to take the mutex with a timeout
        if (xSemaphoreTake(lvgl_mux, pdMS_TO_TICKS(50)) == pdTRUE) { 
            // Call UI update function
            ui_app_update();
            xSemaphoreGive(lvgl_mux);
        } else {
            // Silently continue if we can't get the mutex
        }
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

void app_main(void)
{
    // Initialize display
    init_display();

    // Initialize the UI
    ui_app_init();

    ESP_LOGI(TAG, "Display and UI initialized");

    // Enable touch debugging by default
    toggle_touch_debug(true);
    ESP_LOGI(TAG, "Touch debugging enabled");

    // Create touch debug task with higher stack size and priority
    xTaskCreatePinnedToCore(touch_debug_task, "touch_debug", 8192, NULL, 2, NULL, 0);
    
    // Create LVGL update task with higher priority than lvgl_port_task
    BaseType_t lvgl_task_created = xTaskCreatePinnedToCore(lvgl_update_task, "lvgl_update", 8192, NULL, 2, NULL, 1); 
    if (lvgl_task_created == pdPASS) {
        ESP_LOGI(TAG, "LVGL update task created successfully");
    } else {
        ESP_LOGE(TAG, "Failed to create LVGL update task");
    }
}