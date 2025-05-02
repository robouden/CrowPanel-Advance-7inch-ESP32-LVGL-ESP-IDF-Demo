#ifdef USING_ARDUINO_FRAMEWORK

#include <Arduino.h>
#include <lvgl.h>

static lv_obj_t *label;
static lv_obj_t *counter_label;
static lv_obj_t *btn1;
static int counter = 0;

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        counter++;
        char buf[32];
        snprintf(buf, sizeof(buf), "Counter: %d", counter);
        lv_label_set_text(counter_label, buf);
        Serial.printf("Button clicked, counter: %d\n", counter);
    }
}

void ui_init(void)
{
    // Create a simple GUI
    
    // Create a button
    btn1 = lv_btn_create(lv_scr_act());
    lv_obj_set_pos(btn1, 50, 50);
    lv_obj_set_size(btn1, 120, 50);
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    
    // Create a label on the button
    label = lv_label_create(btn1);
    lv_label_set_text(label, "Click Me");
    lv_obj_center(label);
    
    // Create a counter label
    counter_label = lv_label_create(lv_scr_act());
    lv_obj_set_pos(counter_label, 50, 120);
    lv_label_set_text(counter_label, "Counter: 0");
    
    // Create a title label
    lv_obj_t *title = lv_label_create(lv_scr_act());
    lv_obj_set_pos(title, 10, 10);
    lv_label_set_text(title, "CrowPanel LVGL Demo");
    
    // Create some decorative elements
    lv_obj_t *panel = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(panel, 200, 50);
    lv_obj_set_size(panel, 100, 100);
    
    Serial.println("UI created successfully");
}

void ui_tick(void)
{
    // This is called periodically to update the UI
    // Add any time-dependent operations here
    
    // Just update the counter label color occasionally for a visual cue
    static uint32_t last_color_change = 0;
    if (millis() - last_color_change > 1000) { // Every second
        static lv_color_t colors[] = {
            lv_palette_main(LV_PALETTE_RED),
            lv_palette_main(LV_PALETTE_BLUE),
            lv_palette_main(LV_PALETTE_GREEN),
            lv_palette_main(LV_PALETTE_YELLOW)
        };
        static int color_idx = 0;
        
        lv_obj_set_style_text_color(counter_label, colors[color_idx], LV_PART_MAIN);
        color_idx = (color_idx + 1) % 4;
        
        last_color_change = millis();
    }
}

#endif // USING_ARDUINO_FRAMEWORK 