#if defined(EEZ_FOR_LVGL)
#include <eez/core/vars.h>
#endif

#include <Arduino.h>
#include "ui.h"
#include "screens.h"
#include "images.h"
#include "actions.h"
#include "vars.h"

#if defined(EEZ_FOR_LVGL)

void ui_init() {
    eez_flow_init(assets, sizeof(assets), (lv_obj_t **)&objects, sizeof(objects), images, sizeof(images), actions);
}

void ui_tick() {
    static int debug_counter = 0;
    if (debug_counter++ % 100 == 0) { // Log every 100 ticks to avoid flooding
        Serial.println("ui_tick called");
    }
    
    eez_flow_tick();
#if defined(EEZ_FOR_LVGL)
    Serial.printf("Using g_currentScreen: %d\n", g_currentScreen);
    tick_screen(g_currentScreen);
#else
    Serial.printf("Using currentScreen: %d\n", currentScreen);
    tick_screen(currentScreen);
#endif
}

#else

#include <string.h>

static int16_t currentScreen = -1;

static lv_obj_t *getLvglObjectFromIndex(int32_t index) {
    if (index == -1) {
        return 0;
    }
    return ((lv_obj_t **)&objects)[index];
}

void loadScreen(enum ScreensEnum screenId) {
    currentScreen = screenId - 1;
    lv_obj_t *screen = getLvglObjectFromIndex(currentScreen);
    lv_scr_load_anim(screen, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
}

void ui_init() {
    create_screens();
    loadScreen(SCREEN_ID_MAIN);
}

void ui_tick() {
    tick_screen(currentScreen);
}

#endif
