#include <Wire.h>
#include <lvgl.h>
#include "GT911.h"
#include "ui.h" // EEZ Studio generated header

// Pin definitions (match your hardware)
#define LCD_HSYNC 40
#define LCD_VSYNC 41
#define LCD_DE    42
#define LCD_PCLK  39
#define LCD_DATA0 21
#define LCD_DATA1 47
#define LCD_DATA2 48
#define LCD_DATA3 45
#define LCD_DATA4 38
#define LCD_DATA5 9
#define LCD_DATA6 10
#define LCD_DATA7 11
#define LCD_DATA8 12
#define LCD_DATA9 13
#define LCD_DATA10 14
#define LCD_DATA11 7
#define LCD_DATA12 17
#define LCD_DATA13 18
#define LCD_DATA14 3
#define LCD_DATA15 46
#define TOUCH_INT 4
#define TOUCH_RST 5
#define I2C_SCL   16
#define I2C_SDA   15
#define BACKLIGHT_PIN 2 // Set to your actual backlight pin

GT911 ts = GT911(TOUCH_INT, TOUCH_RST, 800, 480);

static const uint16_t screenWidth  = 800;
static const uint16_t screenHeight = 480;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 40];

void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    // TODO: Implement your display flush code here for your panel
    lv_disp_flush_ready(disp_drv);
}

void my_touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data) {
    uint16_t x, y;
    if (ts.touched()) {
        ts.read();
        x = ts.getX();
        y = ts.getY();
        data->state = LV_INDEV_STATE_PR;
        data->point.x = x;
        data->point.y = y;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void setupTouch() {
    pinMode(TOUCH_RST, OUTPUT);
    pinMode(TOUCH_INT, OUTPUT);
    digitalWrite(TOUCH_RST, LOW);
    digitalWrite(TOUCH_INT, LOW); // or HIGH for alternate I2C address
    delay(10);
    digitalWrite(TOUCH_RST, HIGH);
    delay(50);
    pinMode(TOUCH_INT, INPUT);

    Wire.begin(I2C_SDA, I2C_SCL, 400000);
    ts.begin(Wire);
}

void setup() {
    Serial.begin(115200);

    // Backlight
    pinMode(BACKLIGHT_PIN, OUTPUT);
    digitalWrite(BACKLIGHT_PIN, HIGH);

    // Touch
    setupTouch();

    // Display
    // TODO: Add your display initialization here (e.g., RGBPanel, LovyanGFX, etc.)

    // LVGL
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 40);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    // EEZ Studio UI initialization
    ui_init();
}

void loop() {
    lv_timer_handler();
    // EEZ Studio UI tick/update
    ui_tick();
    delay(5);
} 