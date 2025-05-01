#ifndef UI_INIT_H
#define UI_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

// Initialize the UI components
void ui_init(void);

// Update UI elements - should be called periodically
void ui_update(void);

#ifdef __cplusplus
}
#endif

#endif // UI_INIT_H 