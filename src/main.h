#ifndef MAIN_H
#define MAIN_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#ifdef __cplusplus
extern "C" {
#endif

// Event group bits
#define DISPLAY_READY_BIT   (1 << 0)
#define UI_READY_BIT        (1 << 1)
#define DISPLAY_WORKING_BIT (1 << 2)

// Function to toggle touch debugging
void toggle_touch_debug(bool enable);

// Function to force display to reset
void force_display_reset(void);

// Function to notify that display is working
void notify_display_working(void);

// UI initialization task
void ui_init_task(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */ 