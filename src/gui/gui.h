#ifndef GUI_H
#define GUI_H

#include <lvgl.h>
#include "../display/pins.h"
#include "../touch/GT911.h"

#ifdef __cplusplus
extern "C" {
#endif

// void disp_time();

void event_screen_init(lv_event_t *e);

void gui_init(void);

#ifdef __cplusplus
}
#endif

#endif /* GUI_H */