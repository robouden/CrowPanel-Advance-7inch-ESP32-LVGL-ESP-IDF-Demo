#include "esp_log.h"

#include "gui.h"
#include <esp_err.h>


static const char* TAG = "GUI";

uint8_t hours = 0;
uint8_t minutes = 0;
uint8_t seconds = 0;



void event_screen_init(lv_event_t * e) {
    ESP_LOGI(TAG, "Init screen");
}
