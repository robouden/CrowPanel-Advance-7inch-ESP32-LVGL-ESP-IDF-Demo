#include "actions.h"
#include "vars.h"
#include "ui.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "UI_ACTIONS";

// Forward declaration for the increment function
extern void increment_label_count();

// Action handler for the click counter button
void action_button_click_action(lv_event_t *e) {
    // Increment counter and update the label
    increment_label_count();
    ESP_LOGI(TAG, "Button clicked, counter incremented");
}

// Action handler for the "Put to sleep" button
void action_button_click_action_1(lv_event_t *e) {
    ESP_LOGI(TAG, "Put to sleep button clicked");
    // Here you would implement sleep functionality if needed
}

// Action handler for the slider
void action_button_click_action_3(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    int32_t value = lv_slider_get_value(slider);
    ESP_LOGI(TAG, "Slider value changed: %ld", value);
} 