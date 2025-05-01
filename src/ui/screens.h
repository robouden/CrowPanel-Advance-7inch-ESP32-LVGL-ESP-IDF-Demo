#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *click_count_up;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *label_count;
    lv_obj_t *put_to_sleep;
    lv_obj_t *obj2;
    lv_obj_t *obj3;
    lv_obj_t *slider_test;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

void create_screen_main();
void tick_screen_main();

void create_screens();
void tick_screen(int screen_index);

#endif /*EEZ_LVGL_UI_SCREENS_H*/