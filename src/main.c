#include <stdio.h>

#include "esp_err.h"
#include "esp_log.h"

#include "display/esp32_s3.h"
#include "lvgl.h"

static const char* TAG = "MAIN";

extern SemaphoreHandle_t lvgl_mux;

void app_main(void)
{
    // Initialize display
    init_display();
    
    // Create a black screen
    lv_obj_t * screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_disp_load_scr(screen);
    
    // Create a label with white text
    lv_obj_t * label = lv_label_create(screen);
    lv_label_set_text(label, "Hello World!");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_42, 0);  // Using largest available font
    lv_obj_center(label);
    
    ESP_LOGI(TAG, "Display initialized");
}