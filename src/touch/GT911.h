#ifndef GT911_H
#define GT911_H

#include <Arduino.h>
#include <Wire.h>

// GT911 I2C address
#define GT911_ADDR1 0x5D
#define GT911_ADDR2 0x14

// GT911 registers
#define GT911_REG_STATUS    0x814E
#define GT911_REG_POINT1    0x8150
#define GT911_REG_POINT2    0x8158
#define GT911_REG_POINT3    0x8160
#define GT911_REG_POINT4    0x8168
#define GT911_REG_POINT5    0x8170
#define GT911_REG_ID        0x8140
#define GT911_REG_RESET     0x8040

// GT911 touch point structure
typedef struct {
    uint8_t status;     // 0x80: touch down, 0x00: touch up
    uint16_t x;         // X coordinate
    uint16_t y;         // Y coordinate
    uint16_t size;      // Touch size
    uint8_t reserved;   // Reserved
} gt911_touch_point_t;

class GT911 {
public:
    GT911(uint8_t addr = GT911_ADDR1, TwoWire *wire = &Wire) : _addr(addr), _wire(wire) {}

    bool begin() {
        _wire->beginTransmission(_addr);
        bool result = (_wire->endTransmission() == 0);
        if (!result) {
            // Try alternate address
            _addr = (_addr == GT911_ADDR1) ? GT911_ADDR2 : GT911_ADDR1;
            _wire->beginTransmission(_addr);
            result = (_wire->endTransmission() == 0);
        }
        return result;
    }

    bool readTouchPoints(gt911_touch_point_t *points, uint8_t *count) {
        uint8_t status = 0;
        
        // Read touch status
        if (!readRegister(GT911_REG_STATUS, &status, 1)) {
            return false;
        }
        
        // Number of touch points (0-5)
        *count = status & 0x0F;
        if (*count == 0) {
            return true;
        }
        
        // Read touch points
        for (uint8_t i = 0; i < *count && i < 5; i++) {
            uint8_t data[8];
            uint16_t reg = GT911_REG_POINT1 + (i * 8);
            
            if (!readRegister(reg, data, 8)) {
                return false;
            }
            
            points[i].status = data[0];
            points[i].x = (data[2] << 8) | data[1];
            points[i].y = (data[4] << 8) | data[3];
            points[i].size = (data[6] << 8) | data[5];
            points[i].reserved = data[7];
        }
        
        // Clear status register
        uint8_t clear = 0;
        writeRegister(GT911_REG_STATUS, &clear, 1);
        
        return true;
    }

private:
    uint8_t _addr;
    TwoWire *_wire;

    bool readRegister(uint16_t reg, uint8_t *data, uint8_t len) {
        _wire->beginTransmission(_addr);
        _wire->write(reg >> 8);        // High byte
        _wire->write(reg & 0xFF);      // Low byte
        if (_wire->endTransmission(false) != 0) {
            return false;
        }
        
        _wire->requestFrom(_addr, len);
        if (_wire->available() != len) {
            return false;
        }
        
        for (uint8_t i = 0; i < len; i++) {
            data[i] = _wire->read();
        }
        
        return true;
    }

    bool writeRegister(uint16_t reg, uint8_t *data, uint8_t len) {
        _wire->beginTransmission(_addr);
        _wire->write(reg >> 8);        // High byte
        _wire->write(reg & 0xFF);      // Low byte
        
        for (uint8_t i = 0; i < len; i++) {
            _wire->write(data[i]);
        }
        
        return (_wire->endTransmission() == 0);
    }
};

#endif // GT911_H
