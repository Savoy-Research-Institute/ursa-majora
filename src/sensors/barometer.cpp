https://www.youtube.com/watch?v=rabWc5W84ug
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

//BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

/************************Barometer functions**********************************************
Read absolute altitude from air pressure, read surface chip temperature, absolute altitude lock */
#include "BMP280.h"

#define BMP280_REGISTER_DIG_T1 0x88
#define BMP280_REGISTER_DIG_T2 0x8A
#define BMP280_REGISTER_DIG_T3 0x8C
#define BMP280_REGISTER_DIG_P1 0x8E
#define BMP280_REGISTER_DIG_P2 0x90
#define BMP280_REGISTER_DIG_P3 0x92
#define BMP280_REGISTER_DIG_P4 0x94
#define BMP280_REGISTER_DIG_P5 0x96
#define BMP280_REGISTER_DIG_P6 0x98
#define BMP280_REGISTER_DIG_P7 0x9A
#define BMP280_REGISTER_DIG_P8 0x9C
#define BMP280_REGISTER_DIG_P9 0x9E
#define BMP280_REGISTER_CHIPID 0xD0
#define BMP280_REGISTER_RESET 0xE0
#define BMP280_REGISTER_STATUS 0xF3
#define BMP280_REGISTER_CONTROL 0xF4
#define BMP280_REGISTER_CONFIG 0xF5
#define BMP280_REGISTER_PRESSUREDATA 0xF7
#define BMP280_REGISTER_TEMPDATA 0xFA

BMP280::BMP280() : _address(0x76), _t_fine(0) {}

bool BMP280::begin(uint8_t address) {
    _address = address;
    Wire.begin();

    if (read8(BMP280_REGISTER_CHIPID) != 0x58)
        return false;

    readCalibrationData();

    write8(BMP280_REGISTER_CONTROL, 0x3F); // Normal mode, temperature and pressure over sampling rate = 16

    return true;
}

float BMP280::readTemperature() {
    int32_t adc_T = read24(BMP280_REGISTER_TEMPDATA);
    adc_T >>= 4;

    int32_t var1 = ((((adc_T >> 3) - ((int32_t)_dig_T1 << 1))) * ((int32_t)_dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)_dig_T1)) * ((adc_T >> 4) - ((int32_t)_dig_T1))) >> 12) * ((int32_t)_dig_T3)) >> 14;

    _t_fine = var1 + var2;
    float T = (_t_fine * 5 + 128) >> 8;
    return T / 100.0;
}

float BMP280::readPressure() {
    int32_t adc_P = read24(BMP280_REGISTER_PRESSUREDATA);
    adc_P >>= 4;

    int64_t var1, var2, p;
    var1 = ((int64_t)_t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_dig_P6;
    var2 = var2 + ((var1 * (int64_t)_dig_P5) << 17);
    var2 = var2 + (((int64_t)_dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)_dig_P3) >> 8) + ((var1 * (int64_t)_dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_dig_P1) >> 33;

    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)_dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)_dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)_dig_P7) << 4);
    return (float)p / 256.0;
}

float BMP280::readAltitude(float seaLevelPressure) {
    float pressure = readPressure();
    return 44330 * (1.0 - pow(pressure / seaLevelPressure, 0.1903));
}

void BMP280::readCalibrationData() {
    _dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
    _dig_T2 = readS16_LE(BMP280_REGISTER_DIG_T2);
    _dig_T3 = readS16_LE(BMP280_REGISTER_DIG_T3);

    _dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
    _dig_P2 = readS16_LE(BMP280_REGISTER_DIG_P2);
    _dig_P3 = readS16_LE(BMP280_REGISTER_DIG_P3);
    _dig_P4 = readS16_LE(BMP280_REGISTER_DIG_P4);
    _dig_P5 = readS16_LE(BMP280_REGISTER_DIG_P5);
    _dig_P6 = readS16_LE(BMP280_REGISTER_DIG_P6);
    _dig_P7 = readS16_LE(BMP280_REGISTER_DIG_P7);
    _dig_P8 = readS16_LE(BMP280_REGISTER_DIG_P8);
    _dig_P9 = readS16_LE(BMP280_REGISTER_DIG_P9);
}

uint8_t BMP280::read8(uint8_t reg) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t)1);
    return Wire.read();
}

uint16_t BMP280::read16(uint8_t reg) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t)2);
    return (Wire.read() << 8) | Wire.read();
}

uint16_t BMP280::read16_LE(uint8_t reg) {
    uint16_t temp = read16(reg);
    return (temp >> 8) | (temp << 8);
}

int16_t BMP280::readS16(uint8_t reg) {
    return (int16_t)read16(reg);
}

int16_t BMP280::readS16_LE(uint8_t reg) {
    return (int16_t)read16_LE(reg);
}

uint32_t BMP280::read24(uint8_t reg) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t)3);
    
    uint32_t value;
    value = Wire.read();
    value <<= 8;
    value |= Wire.read();
    value <<= 8;
    value |= Wire.read();

    return value;
}

void BMP280::write8(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

void read_barometer()
{
}
void read_barometer_temperature()
{
}
