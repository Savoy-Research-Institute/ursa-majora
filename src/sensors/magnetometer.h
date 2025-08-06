#ifndef HMC5883L_h
#define HMC5883L_h

#include <Arduino.h>
#include <Wire.h>

class HMC5883L {
  public:
    HMC5883L();
    void begin();
    void readRawData(int16_t& x, int16_t& y, int16_t& z);
    float getHeading();
    void calibrate(uint16_t samples = 1000, uint16_t delay_ms = 10);
    void setDeclination(float degrees, float minutes = 0);

  private:
    static const uint8_t HMC5883L_ADDRESS = 0x1E;
    float declination;
    int16_t x_offset, y_offset, z_offset;

    void writeRegister(uint8_t reg, uint8_t value);
    void readRegister(uint8_t reg, uint8_t count, uint8_t* data);
    int16_t twosComplement(int16_t val);
};

#endif