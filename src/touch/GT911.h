#ifndef GT911_H
#define GT911_H

#include <Arduino.h>
#include <Wire.h>
#include "../display/pins.h"

// GT911 I2C address options
#define GT911_ADDR1 0x5D
#define GT911_ADDR2 0x14

// GT911 registers
#define GT911_REG_CONFIG    0x8047
#define GT911_REG_COORD     0x814E
#define GT911_REG_TOUCH     0x814E
#define GT911_REG_VERSION   0x8140

#define GT911_MAX_TOUCH_POINTS 5

struct TouchPoint {
    uint16_t x;
    uint16_t y;
    uint16_t size;
    uint8_t trackId;
};

// Rotation enum
enum TouchRotation {
    TOUCH_ROTATION_NORMAL = 0,
    TOUCH_ROTATION_90 = 1,
    TOUCH_ROTATION_180 = 2,
    TOUCH_ROTATION_270 = 3
};

class GT911 {
public:
    GT911(uint8_t addr = GT911_ADDR1) : _addr(addr), _points(0), _rotation(TOUCH_ROTATION_NORMAL) {}
    
    bool begin(void) {
        Wire.beginTransmission(_addr);
        bool success = (Wire.endTransmission() == 0);
        if (success) {
            // Read version to confirm communication
            uint8_t buf[4];
            if (readBytes(GT911_REG_VERSION, buf, 4)) {
                return true;
            }
        }
        return false;
    }
    
    void setRotation(TouchRotation rotation) {
        _rotation = rotation;
    }
    
    void setResolution(uint16_t width, uint16_t height) {
        _width = width;
        _height = height;
    }
    
    bool available(void) {
        uint8_t points = 0;
        if (readBytes(GT911_REG_TOUCH, &points, 1)) {
            points &= 0x0F;
            if (points > 5) points = 5; // Maximum 5 touch points
            _points = points;
            if (points > 0) {
                uint8_t buf[8 * 5]; // 8 bytes per touch point, max 5 points
                if (readBytes(GT911_REG_COORD, buf, points * 8)) {
                    for (uint8_t i = 0; i < points; i++) {
                        _touchPoints[i].x = buf[i * 8 + 1] << 8 | buf[i * 8];
                        _touchPoints[i].y = buf[i * 8 + 3] << 8 | buf[i * 8 + 2];
                        _touchPoints[i].size = buf[i * 8 + 4];
                        _touchPoints[i].trackId = buf[i * 8 + 5];
                        
                        // Apply rotation
                        uint16_t x = _touchPoints[i].x;
                        uint16_t y = _touchPoints[i].y;
                        
                        switch (_rotation) {
                            case TOUCH_ROTATION_90:
                                _touchPoints[i].x = y;
                                _touchPoints[i].y = _width - x;
                                break;
                            case TOUCH_ROTATION_180:
                                _touchPoints[i].x = _width - x;
                                _touchPoints[i].y = _height - y;
                                break;
                            case TOUCH_ROTATION_270:
                                _touchPoints[i].x = _height - y;
                                _touchPoints[i].y = x;
                                break;
                            default: // TOUCH_ROTATION_NORMAL
                                break;
                        }
                    }
                    return true;
                }
            }
        }
        return false;
    }
    
    uint8_t getTouchPoints(void) {
        return _points;
    }
    
    TouchPoint getPoint(uint8_t index) {
        if (index < _points) {
            return _touchPoints[index];
        }
        return TouchPoint{0, 0, 0, 0};
    }

private:
    bool readBytes(uint16_t reg, uint8_t *data, uint8_t len) {
        Wire.beginTransmission(_addr);
        Wire.write(reg >> 8);
        Wire.write(reg & 0xFF);
        if (Wire.endTransmission(false) != 0) {
            return false;
        }
        Wire.requestFrom(_addr, len);
        if (Wire.available() != len) {
            return false;
        }
        for (uint8_t i = 0; i < len; i++) {
            data[i] = Wire.read();
        }
        return true;
    }

    uint8_t _addr;
    uint8_t _points;
    TouchPoint _touchPoints[5];
    TouchRotation _rotation;
    uint16_t _width;
    uint16_t _height;
};

#endif // GT911_H
