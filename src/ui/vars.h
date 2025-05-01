#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

// enum declarations

// Flow global variables
enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_NONE
};

// Native global variables
const char *get_var_label_count_value();
void set_var_label_count_value(const char *value);
void increment_label_count();

#endif /*EEZ_LVGL_UI_VARS_H*/