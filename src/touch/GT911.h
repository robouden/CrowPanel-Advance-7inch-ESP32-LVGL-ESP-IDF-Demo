#ifndef GT911_H
#define GT911_H

#include <Arduino.h>
#include <Wire.h>
#include "../display/pins.h"

#define GT911_ADDR 0x5D
#define GT911_MAX_TOUCH_POINTS 5

struct TouchPoint {
    uint16_t x;
    uint16_t y;
    uint16_t size;
    uint8_t trackId;
};

class GT911 {
public:
    GT911(uint8_t addr = GT911_ADDR) : _addr(addr) {}
    
    bool begin() {
        Wire.beginTransmission(_addr);
        bool success = (Wire.endTransmission() == 0);
        if (success) {
            // Reset touch configuration
            setResolution(LCD_H_RES, LCD_V_RES);
            _rotation = ROTATION_NORMAL;
        }
        return success;
    }
    
    bool available() {
        Wire.beginTransmission(_addr);
        Wire.write(0x81);  // Status register
        Wire.endTransmission(false);
        Wire.requestFrom(_addr, (uint8_t)1);
        if (Wire.available()) {
            uint8_t status = Wire.read();
            return (status & 0x80) != 0;  // Buffer status bit
        }
        return false;
    }
    
    uint8_t getTouchPoints() {
        Wire.beginTransmission(_addr);
        Wire.write(0x81);  // Status register
        Wire.endTransmission(false);
        Wire.requestFrom(_addr, (uint8_t)1);
        if (Wire.available()) {
            uint8_t touches = Wire.read() & 0x0F;  // Number of touch points
            return touches > GT911_MAX_TOUCH_POINTS ? 0 : touches;
        }
        return 0;
    }
    
    TouchPoint getPoint(uint8_t index) {
        TouchPoint point = {0, 0, 0, 0};
        if (index >= GT911_MAX_TOUCH_POINTS) return point;
        
        Wire.beginTransmission(_addr);
        Wire.write(0x84 + (index * 8));  // Touch point data registers
        Wire.endTransmission(false);
        Wire.requestFrom(_addr, (uint8_t)7);
        
        if (Wire.available() >= 7) {
            uint16_t x = Wire.read() | (Wire.read() << 8);
            uint16_t y = Wire.read() | (Wire.read() << 8);
            uint16_t size = Wire.read() | (Wire.read() << 8);
            uint8_t id = Wire.read();
            
            // Apply rotation if needed
            switch (_rotation) {
                case ROTATION_90:
                    point.x = y;
                    point.y = LCD_H_RES - x;
                    break;
                case ROTATION_180:
                    point.x = LCD_H_RES - x;
                    point.y = LCD_V_RES - y;
                    break;
                case ROTATION_270:
                    point.x = LCD_V_RES - y;
                    point.y = x;
                    break;
                default:  // ROTATION_NORMAL
                    point.x = x;
                    point.y = y;
                    break;
            }
            point.size = size;
            point.trackId = id;
        }
        return point;
    }
    
    void setRotation(uint8_t rotation) {
        _rotation = rotation % 4;
    }
    
    void setResolution(uint16_t width, uint16_t height) {
        Wire.beginTransmission(_addr);
        Wire.write(0x8048);  // Resolution registers
        Wire.write(width & 0xFF);
        Wire.write((width >> 8) & 0xFF);
        Wire.write(height & 0xFF);
        Wire.write((height >> 8) & 0xFF);
        Wire.endTransmission();
    }

private:
    uint8_t _addr;
    uint8_t _rotation;
};

#endif // GT911_H
