#include <stdio.h>

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "display/esp32_s3.h"
#include "lvgl.h"

static const char *TAG = "MAIN";

extern SemaphoreHandle_t lvgl_mux;
static QueueHandle_t debug_queue = NULL;

// Task for touch debugging
static void touch_debug_task(void *pvParameters) {
    // Create queue for touch debug control
    debug_queue = xQueueCreate(1, sizeof(uint8_t));
    if (debug_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create debug queue");
        vTaskDelete(NULL);
        return;
    }

    uint8_t debug_enabled = 1;
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

void app_main(void)
{
    // Initialize display
    init_display();

    // Create a black screen
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_disp_load_scr(screen);

      lv_obj_t * label = lv_label_create(screen);
    lv_label_set_text(label, "Hello World!");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_42, 0);  // Using largest available font
    lv_obj_center(label);

    ESP_LOGI(TAG, "Display initialized");

    // Create touch debug task with higher stack size and priority
    xTaskCreatePinnedToCore(touch_debug_task, "touch_debug", 8192, NULL, 2, NULL, 0);
}