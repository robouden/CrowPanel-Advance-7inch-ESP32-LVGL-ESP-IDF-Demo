#ifndef DISPLAY_ARDUINO_H
#define DISPLAY_ARDUINO_H

#ifdef __cplusplus
extern "C" {
#endif

// Display functions
void init_display(void);
void force_lcd_refresh(void);
void debug_touch_info(void);
void notify_display_working(void);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_ARDUINO_H 