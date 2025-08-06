#ifndef BMP280_h
#define BMP280_h

#include <Arduino.h>
#include <Wire.h>

class BMP280 {
  public:
    BMP280();
    bool begin(uint8_t address = 0x76);
    float readTemperature();
    float readPressure();
    float readAltitude(float seaLevelPressure = 1013.25);
    
  private:
    uint8_t _address;
    uint16_t _dig_T1;
    int16_t _dig_T2, _dig_T3;
    uint16_t _dig_P1;
    int16_t _dig_P2, _dig_P3, _dig_P4, _dig_P5, _dig_P6, _dig_P7, _dig_P8, _dig_P9;
    int32_t _t_fine;

    void readCalibrationData();
    uint8_t read8(uint8_t reg);
    uint16_t read16(uint8_t reg);
    uint16_t read16_LE(uint8_t reg);
    int16_t readS16(uint8_t reg);
    int16_t readS16_LE(uint8_t reg);
    uint32_t read24(uint8_t reg);
    void write8(uint8_t reg, uint8_t value);
};

#endif