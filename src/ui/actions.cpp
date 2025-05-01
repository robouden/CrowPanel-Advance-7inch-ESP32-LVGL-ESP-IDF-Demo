#include "actions.h"
#include "vars.h"
#include "ui.h"
#include <Arduino.h>

// Forward declarations
extern void increment_label_count();
extern const char* get_var_label_count_value();

// Action handler for the click counter button
void action_button_click_action(lv_event_t *e) {
    // Increment counter and update the label
    increment_label_count();
    // Get the current counter value and log it
    Serial.printf("Button 1 (Counter) clicked - New count: %s\n", get_var_label_count_value());
}

// Action handler for the "Put to sleep" button
void action_button_click_action_1(lv_event_t *e) {
    static int sleep_button_count = 0;
    sleep_button_count++;
    Serial.printf("Button 2 (Sleep) clicked - Press count: %d\n", sleep_button_count);
    // Here you would implement sleep functionality if needed
}

// Action handler for Button 3
void action_button_click_action_2(lv_event_t *e) {
    static int button3_count = 0;
    button3_count++;
    Serial.printf("Button 3 clicked - Press count: %d\n", button3_count);
    // Add any Button 3 functionality here
}

// Action handler for the slider
void action_button_click_action_3(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    int32_t value = lv_slider_get_value(slider);
    static int32_t prev_value = -1;
    
    // Only log when the value actually changes
    if (value != prev_value) {
        Serial.printf("Slider moved - New value: %ld (range: 0-100)\n", value);
        prev_value = value;
    }
}
