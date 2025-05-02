#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include "driver/ledc.h"

#ifdef __cplusplus
extern "C" {
#endif

// Buzzer configuration
#define BUZZER_GPIO         4
#define BUZZER_LEDC_TIMER  LEDC_TIMER_0
#define BUZZER_LEDC_CHANNEL LEDC_CHANNEL_0
#define BUZZER_FREQ_DEFAULT 0  // Start with buzzer off

// Function declarations
esp_err_t buzzer_init(gpio_num_t gpio_num, ledc_timer_t timer_num, ledc_channel_t channel);
void buzzer_set_freq(uint32_t freq);
void buzzer_set_duty(uint32_t duty);
void buzzer_stop(void);

#ifdef __cplusplus
}
#endif

#endif // BUZZER_H 