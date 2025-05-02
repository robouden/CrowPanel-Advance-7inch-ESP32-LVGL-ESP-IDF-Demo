#include "sdkconfig.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/ledc.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"

#include "lvgl.h"
#include "esp32_s3.h"
#include "pca9557.h"
#include "esp_lcd_touch.h"

#include "elecrow_advanced_7inch_800x480.h"

#include "esp_timer.h"

#define CONFIG_DOUBLE_FB 1

#if CONFIG_DOUBLE_FB
#define LCD_NUM_FB             2
#else
#define LCD_NUM_FB             1
#endif

static const char* TAG = "DISPLAY";

static esp_lcd_touch_handle_t touch_handle = NULL;  
static esp_lcd_panel_handle_t lcd_handle = NULL;    
static pca9557_handle_t expander_handle = NULL;   
static bm8563_handle_t rtc_handle = NULL;   

// Forward declare the touch interrupt callback with correct type
static void touch_interrupt_callback(esp_lcd_touch_handle_t tp);

static void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
static bool on_vsync_event(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_data);
static void lvgl_port_task(void *arg);

SemaphoreHandle_t lvgl_mux;
SemaphoreHandle_t sem_vsync_end;
SemaphoreHandle_t sem_gui_ready;

// Add at the top with other semaphores
static SemaphoreHandle_t touch_mux = NULL;

/**
 * @brief Initialize Display
 *
 * This function initializes the LCD display, including the backlight, touch panel, LCD panel, and LVGL library.
 */
void init_display(void)
{
    ESP_LOGI(TAG, "init_display started");
    i2c_master_bus_handle_t i2c_handle = NULL;

    // Create touch mutex
    ESP_LOGI(TAG, "Creating touch mutex...");
    touch_mux = xSemaphoreCreateMutex();
    if (touch_mux == NULL) {
        ESP_LOGE(TAG, "Failed to create touch mutex");
        return;
    }
    ESP_LOGI(TAG, "Touch mutex created.");

    ESP_LOGI(TAG, "Initializing buzzer...");
    init_buzzer();
    ESP_LOGI(TAG, "Buzzer initialized.");

    ESP_LOGI(TAG, "Initializing I2C...");
    init_i2c(&i2c_handle);
    ESP_LOGI(TAG, "I2C initialized.");

    ESP_LOGI(TAG, "Initializing I2C expander...");
    init_i2c_expander(i2c_handle, &expander_handle);
    ESP_LOGI(TAG, "I2C expander initialized.");

    ESP_LOGI(TAG, "Initializing touch...");
    init_touch(i2c_handle, expander_handle, &touch_handle);
    ESP_LOGI(TAG, "Touch initialized.");

    ESP_LOGI(TAG, "Initializing RTC...");
    init_rtc(i2c_handle, &rtc_handle);
    ESP_LOGI(TAG, "RTC initialized.");

    ESP_LOGI(TAG, "Initializing LCD...");
    init_lcd(&lcd_handle);
    ESP_LOGI(TAG, "LCD initialized.");

    ESP_LOGI(TAG, "Initializing LVGL...");
    init_lvgl(lcd_handle, touch_handle);
    ESP_LOGI(TAG, "LVGL initialized.");

    ESP_LOGI(TAG, "Initializing backlight...");
    init_backlight(expander_handle);
    ESP_LOGI(TAG, "Backlight initialized.");

    ESP_LOGI(TAG, "init_display finished");
}

/**
 * @brief Initialize LCD Backlight
 *
 * This function initializes the PWM timer and channel configurations for controlling the LCD backlight.
 */
void init_buzzer(void) {

    esp_err_t ret = buzzer_init(BUZZER_GPIO, BUZZER_LEDC_TIMER, BUZZER_LEDC_CHANNEL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Buzzer konnte nicht initialisiert werden!");
    }
}

void init_i2c(i2c_master_bus_handle_t *i2c_bus_handle) {
    // 1. I2C Master Config erstellen
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM,
        .scl_io_num = I2C_SCL,
        .sda_io_num = I2C_SDA,
        .flags.enable_internal_pullup = true,
        .flags.allow_pd = false,
        .trans_queue_depth = 0,
    };

    esp_err_t ret = i2c_new_master_bus(&i2c_bus_config, i2c_bus_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C Bus konnte nicht erstellt werden: %s", esp_err_to_name(ret));
        return;
    }

    ret = i2c_master_bus_reset(*i2c_bus_handle); // I2C Bus zurücksetzen
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C Bus konnte nicht zurückgesetzt werden: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "I2C Bus erfolgreich initialisiert!");
}


/**
 * @brief Initialize LCD Backlight
 *
 * This function initializes the PWM timer and channel configurations for controlling the LCD backlight.
 */
void init_i2c_expander(i2c_master_bus_handle_t i2c_bus_handle, pca9557_handle_t *expander_handle) { 
    // Initialisierung des PCA9557 Treibers
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = PCA9557_I2C_ADDRESS,
        .scl_speed_hz = I2C_CLK_SPEED_HZ, // Standard I2C Geschwindigkeit
    };

    esp_err_t ret = pca9557_init(i2c_bus_handle, expander_handle, &dev_config); // PCA9557 Treiber initialisieren [5]
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fehler beim Initialisieren des PCA9557");
        //i2c_master_bus_delete(i2c_bus); // Bei Fehler I2C Bus wieder freigeben
        return;
    }

 
    ESP_LOGI(TAG, "PCA9557 erfolgreich initialisiert");
}

void init_rtc(i2c_master_bus_handle_t i2c_bus_handle, bm8563_handle_t *rtc_handle) { 
    // Initialisierung des PCA9557 Treibers
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BM8563_I2C_ADDR,
        .scl_speed_hz = I2C_CLK_SPEED_HZ, 
        .flags.disable_ack_check = true, // Disable ACK check for the RTC
    };

    esp_err_t ret = bm8563_init(i2c_bus_handle, rtc_handle, &dev_config, GPIO_NUM_NC); // PCA9557 Treiber initialisieren [5]
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fehler beim Initialisieren des PCA9557");
        //i2c_master_bus_delete(i2c_bus); // Bei Fehler I2C Bus wieder freigeben
        return;
    }

 
    ESP_LOGI(TAG, "bm8563 erfolgreich initialisiert");
}


/**
 * @brief Initialize Touch Driver
 *
 * This function installs the touch driver, configures the I2C interface for touch communication,
 * initializes the touch controller, and creates a touch handle for touch input.
 *
 * @param[out] touch_handle Pointer to the handle for the initialized touch controller.
 */
void init_touch(i2c_master_bus_handle_t i2c_bus_handle, pca9557_handle_t expander_handle, esp_lcd_touch_handle_t *touch_handle) {
    ESP_LOGI(TAG, "Initialize GT911 touch controller");

    // Configure touch I2C
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = TOUCH_I2C_SDA,
        .scl_io_num = TOUCH_I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = TOUCH_I2C_FREQ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));

    // GT911 touch controller configuration
    esp_lcd_touch_config_t tp_cfg = {
        .x_max = LCD_H_RES,
        .y_max = LCD_V_RES,
        .rst_gpio_num = GPIO_NUM_NC, // No hardware reset pin
        .int_gpio_num = GPIO_NUM_NC, // No interrupt pin
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };

    // Initialize GT911
    esp_lcd_panel_io_i2c_config_t tp_io_cfg = {
        .dev_addr = TOUCH_I2C_ADDR,
        .control_phase_bytes = 1,
        .dc_bit_offset = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .flags = {
            .dc_low_on_data = 0,
            .disable_control_phase = 1,
        }
    };
    
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)i2c_bus_handle, &tp_io_cfg, &tp_io_handle));
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_gt911(tp_io_handle, &tp_cfg, touch_handle));

    ESP_LOGI(TAG, "Touch controller initialized successfully");
}

// Implement the touch interrupt callback
static void touch_interrupt_callback(esp_lcd_touch_handle_t tp)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    // Read touch data immediately when interrupt occurs
    esp_lcd_touch_read_data(tp);
    
    // Notify the LVGL task to process the touch data
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}


/**
 * @brief Initialize RGB LCD Panel
 *
 * This function installs the RGB LCD panel driver, creates semaphores for synchronization,
 * configures the RGB LCD panel with the provided parameters, registers event callbacks,
 * resets and initializes the RGB LCD panel.
 *
 * @param[out] panel_handle Pointer to the handle for the initialized RGB LCD panel.
 */
void init_lcd(esp_lcd_panel_handle_t *panel_handle) {
    ESP_LOGI(TAG, "Initialize RGB LCD panel");
    
    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .timings = {
            .pclk_hz = LCD_PIXEL_CLOCK_HZ,
            .h_res = LCD_H_RES,
            .v_res = LCD_V_RES,
            .hsync_pulse_width = HSYNC_PULSE_WIDTH,
            .hsync_back_porch = HSYNC_BACK_PORCH,
            .hsync_front_porch = HSYNC_FRONT_PORCH,
            .vsync_pulse_width = VSYNC_PULSE_WIDTH,
            .vsync_back_porch = VSYNC_BACK_PORCH,
            .vsync_front_porch = VSYNC_FRONT_PORCH,
            .flags.pclk_active_neg = true,
        },
        .data_width = 16, // RGB565
        .psram_trans_align = 64,
        .hsync_gpio_num = PIN_NUM_HSYNC,
        .vsync_gpio_num = PIN_NUM_VSYNC,
        .de_gpio_num = PIN_NUM_DE,
        .pclk_gpio_num = PIN_NUM_PCLK,
        .data_gpio_nums = {
            PIN_NUM_DATA0, PIN_NUM_DATA1, PIN_NUM_DATA2, PIN_NUM_DATA3, PIN_NUM_DATA4,
            PIN_NUM_DATA5, PIN_NUM_DATA6, PIN_NUM_DATA7, PIN_NUM_DATA8, PIN_NUM_DATA9,
            PIN_NUM_DATA10, PIN_NUM_DATA11, PIN_NUM_DATA12, PIN_NUM_DATA13, PIN_NUM_DATA14,
            PIN_NUM_DATA15
        },
        .disp_gpio_num = PIN_NUM_DISP_EN,
        .on_frame_trans_done = NULL,
        .flags = {
            .fb_in_psram = true,
            .double_fb = true,
            .no_fb = false,
            .refresh_on_demand = false,
        },
    };

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(*panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(*panel_handle));

    // Initialize VSYNC semaphore
    sem_vsync_end = xSemaphoreCreateBinary();
    assert(sem_vsync_end);
    esp_lcd_rgb_panel_event_callbacks_t cbs = {
        .on_vsync = on_vsync_event,
    };
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(*panel_handle, &cbs, NULL));

    ESP_LOGI(TAG, "LCD panel initialized successfully");
}


void init_backlight(pca9557_handle_t expander_handle) {
    ESP_LOGI(TAG, "Switch on the backlight");

    pca9557_set_direction(expander_handle, 1, PCA9557_OUTPUT); 
    pca9557_write_pin(expander_handle, 1, 1);

    //xTaskCreate(brightness_task, "BRIGHTNESS", LVGL_TASK_STACK_SIZE, NULL, LVGL_TASK_PRIORITY, NULL);
}



/**
 * @brief Initialize LVGL Library
 *
 * This function initializes the LVGL library, allocates separate draw buffers from PSRAM,
 * registers the display driver and input device driver to LVGL, creates a semaphore for
 * LVGL synchronization, and starts the LVGL port task.
 *
 * @param[in] panel_handle Handle to the LCD panel associated with LVGL.
 * @param[in] touch_handle Handle to the touchpad device associated with LVGL.
 */
void init_lvgl(esp_lcd_panel_handle_t panel_handle, esp_lcd_touch_handle_t touch_handle) {
    ESP_LOGI(TAG, "Initialize LVGL library");

    // Initialize LVGL library
    lv_init();

    // Allocate LVGL display buffer
    void *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1);
    void *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2);
    
    // Initialize LVGL display buffer
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);

    // Register display driver to LVGL
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_H_RES;
    disp_drv.ver_res = LCD_V_RES;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
    disp_drv.full_refresh = true;
    lv_disp_drv_register(&disp_drv);

    // Register touch input device
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    indev_drv.user_data = touch_handle;
    lv_indev_drv_register(&indev_drv);

    // Create LVGL task
    lvgl_mux = xSemaphoreCreateMutex();
    assert(lvgl_mux);
    xTaskCreatePinnedToCore(lvgl_port_task, "lvgl", LVGL_TASK_STACK_SIZE, NULL, LVGL_TASK_PRIORITY, NULL, 1);

    ESP_LOGI(TAG, "LVGL library initialized successfully");
}

/**
 * @brief Touchpad Read Function
 *
 * This function reads touchpad input and updates the LVGL input device data accordingly.
 *
 * @param[in] indev_driver Pointer to the LVGL input device driver structure.
 * @param[out] data Pointer to the LVGL input device data structure to be updated.
 */
static void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    static uint16_t x, y;
    static bool touched;
    
    if (xSemaphoreTake(touch_mux, pdMS_TO_TICKS(100)) == pdTRUE) {
        esp_lcd_touch_read_data(*touch_handle);
        
        if (esp_lcd_touch_get_coordinates(*touch_handle, &x, &y, 1, &touched) == ESP_OK) {
            if (touched) {
                data->point.x = x;
                data->point.y = y;
                data->state = LV_INDEV_STATE_PR;
            } else {
                data->state = LV_INDEV_STATE_REL;
            }
        }
        xSemaphoreGive(touch_mux);
    }
}

/**
 * @brief LVGL Flush Callback
 *
 * This callback function is called by LVGL to flush a portion of the display buffer to the physical display.
 * It passes the draw buffer to the LCD panel driver, indicating the area that needs to be updated.
 *
 * @param[in] drv Pointer to the display driver structure.
 * @param[in] area Pointer to the area that needs to be flushed.
 * @param[in] color_map Pointer to the color map containing pixel data to be flushed.
 */
static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    
    // Pass the flush command to the panel
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
    
    // Notify LVGL that the flush is done
    lv_disp_flush_ready(drv);
}

/**
 * @brief Handles VSYNC events for an ESP32 LCD panel.
 *
 * This function waits until LVGL has finished its operations, indicated by the
 * `sem_gui_ready` semaphore being taken. Once LVGL is ready, it signals that
 * the VSYNC event has ended by giving the `sem_vsync_end` semaphore.
 *
 * @param[in] panel Handle to the LCD panel associated with the VSYNC event.
 * @param[in] event_data Pointer to a structure containing data related to the VSYNC event.
 * @param[in] user_data User data pointer passed when registering the VSYNC event handler.
 * @return
 *     - `true` if the task was woken up due to handling the VSYNC event, indicating that it's safe
 *       to proceed with flushing the buffer.
 *     - `false` otherwise.
 */
static bool on_vsync_event(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_data) {
    BaseType_t high_task_awoken = pdFALSE;
    xSemaphoreGiveFromISR(sem_vsync_end, &high_task_awoken);
    return high_task_awoken == pdTRUE;
}

/**
 * @brief LVGL Port Task
 *
 * This task handles LVGL operations in the background. It periodically calls
 * the LVGL timer handler to update the GUI.
 *
 * @param[in] arg Pointer to task arguments (not used).
 */
static void lvgl_port_task(void *arg) {
    ESP_LOGI(TAG, "Starting LVGL task");
    while (1) {
        if (pdTRUE == xSemaphoreTake(lvgl_mux, portMAX_DELAY)) {
            lv_task_handler();
            xSemaphoreGive(lvgl_mux);
        }
        vTaskDelay(pdMS_TO_TICKS(LVGL_TASK_DELAY_MS));
    }
}

void set_time(uint8_t hours, uint8_t minutes, uint8_t seconds) {

   
    struct tm time_to_set = {
        .tm_sec = seconds,
        .tm_min = minutes,
        .tm_hour = hours,
        .tm_mday = 15,
        .tm_mon = 3,    // April (0-basiert)
        .tm_year = 124, // 2024 (Jahre seit 1900)
        .tm_wday = 1    // Montag
    };

    ESP_LOGI(TAG, "Setze Uhrzeit auf: %02d.%02d.%04d %02d:%02d:%02d",
             time_to_set.tm_mday,
             time_to_set.tm_mon + 1,
             time_to_set.tm_year + 1900,
             time_to_set.tm_hour,
             time_to_set.tm_min,
             time_to_set.tm_sec);

    ESP_ERROR_CHECK(bm8563_set_time(rtc_handle, &time_to_set));
    ESP_LOGI(TAG, "Uhrzeit erfolgreich gesetzt!");
}

esp_err_t get_time(uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
    struct tm current_time;

    esp_err_t ret = bm8563_get_time(rtc_handle, &current_time);
    if (ret != ESP_OK) {
        return ret;
    }

    *hours = current_time.tm_hour;
    *minutes = current_time.tm_min;
    *seconds = current_time.tm_sec;

    return ESP_OK;
}

void debug_touch_info(void) {
    if (touch_handle == NULL) {
        ESP_LOGE(TAG, "Touch handle is NULL!");
        return;
    }

    if (xSemaphoreTake(touch_mux, pdMS_TO_TICKS(10)) != pdTRUE) {
        ESP_LOGW(TAG, "Failed to get touch mutex for debug");
        return;
    }

    uint16_t x[CONFIG_ESP_LCD_TOUCH_MAX_POINTS] = {0};
    uint16_t y[CONFIG_ESP_LCD_TOUCH_MAX_POINTS] = {0};
    uint16_t strength[CONFIG_ESP_LCD_TOUCH_MAX_POINTS] = {0};
    uint8_t count = 0;

    esp_err_t ret = esp_lcd_touch_read_data(touch_handle);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to read touch data in debug: %d", ret);
        xSemaphoreGive(touch_mux);
        return;
    }
    
    if (esp_lcd_touch_get_coordinates(touch_handle, x, y, strength, &count, CONFIG_ESP_LCD_TOUCH_MAX_POINTS)) {
        if (count > 0 && count <= CONFIG_ESP_LCD_TOUCH_MAX_POINTS) {
            printf("Touch Debug Info:\n");
            printf("Number of touch points: %d\n", count);
            for (int i = 0; i < count; i++) {
                printf("Point %d: x=%d, y=%d, strength=%d\n", i, x[i], y[i], strength[i]);
            }
        }
    }

    xSemaphoreGive(touch_mux);
}