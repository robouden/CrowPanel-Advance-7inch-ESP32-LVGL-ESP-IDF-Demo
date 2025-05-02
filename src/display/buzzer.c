#include "buzzer.h"
#include "esp_log.h"

static const char* TAG = "BUZZER";

esp_err_t buzzer_init(gpio_num_t gpio_num, ledc_timer_t timer_num, ledc_channel_t channel) {
    // Configure timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = timer_num,
        .freq_hz = BUZZER_FREQ_DEFAULT,
        .clk_cfg = LEDC_AUTO_CLK
    };
    
    esp_err_t ret = ledc_timer_config(&ledc_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC timer");
        return ret;
    }

    // Configure channel
    ledc_channel_config_t ledc_channel = {
        .gpio_num = gpio_num,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = channel,
        .timer_sel = timer_num,
        .duty = 0,
        .hpoint = 0
    };
    
    ret = ledc_channel_config(&ledc_channel);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC channel");
        return ret;
    }

    // Start with buzzer off
    buzzer_stop();
    
    ESP_LOGI(TAG, "Buzzer initialized successfully");
    return ESP_OK;
}

void buzzer_set_freq(uint32_t freq) {
    if (freq > 0) {
        ledc_set_freq(LEDC_LOW_SPEED_MODE, BUZZER_LEDC_TIMER, freq);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, BUZZER_LEDC_CHANNEL, 512); // 50% duty cycle
        ledc_update_duty(LEDC_LOW_SPEED_MODE, BUZZER_LEDC_CHANNEL);
    } else {
        buzzer_stop();
    }
}

void buzzer_set_duty(uint32_t duty) {
    if (duty > 1023) duty = 1023; // Limit to 10-bit resolution
    ledc_set_duty(LEDC_LOW_SPEED_MODE, BUZZER_LEDC_CHANNEL, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, BUZZER_LEDC_CHANNEL);
}

void buzzer_stop(void) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, BUZZER_LEDC_CHANNEL, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, BUZZER_LEDC_CHANNEL);
} 