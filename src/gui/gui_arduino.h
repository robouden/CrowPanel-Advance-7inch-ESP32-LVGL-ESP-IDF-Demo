#ifndef GUI_ARDUINO_H
#define GUI_ARDUINO_H

#include <Arduino.h>
#include <lvgl.h>

// Arduino implementation of the ESP-IDF GUI functions
// This replaces the ESP-IDF specific gui.c/h files

// Function to initialize the GUI
void gui_init();

// Function to update the GUI
void gui_update();

// Function to handle GUI events
void gui_event_handler(lv_event_t *e);

#endif // GUI_ARDUINO_H
