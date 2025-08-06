//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

//BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

/*Magnetometer measures the earth's magnetic field, method of constructing a digital compass
greedy heading


*/
#include "HMC5883L.h"

HMC5883L::HMC5883L() : declination(0), x_offset(0), y_offset(0), z_offset(0) {}

void HMC5883L::begin() {
  Wire.begin();
  
  // Configuration Register A
  writeRegister(0x00, 0x70);  // 8-average, 15 Hz default, normal measurement
  // Configuration Register B
  writeRegister(0x01, 0x20);  // Gain=1090 LSB/Gauss
  // Mode Register
  writeRegister(0x02, 0x00);  // Continuous measurement mode
}

void HMC5883L::readRawData(int16_t& x, int16_t& y, int16_t& z) {
  uint8_t data[6];
  readRegister(0x03, 6, data);

  x = twosComplement((data[0] << 8) | data[1]);
  z = twosComplement((data[2] << 8) | data[3]);
  y = twosComplement((data[4] << 8) | data[5]);

  x -= x_offset;
  y -= y_offset;
  z -= z_offset;
}

float HMC5883L::getHeading() {
  int16_t x, y, z;
  readRawData(x, y, z);
  
  float heading = atan2(y, x);
  heading += declination;
  
  if (heading < 0)
    heading += 2 * PI;
  if (heading > 2 * PI)
    heading -= 2 * PI;
  
  return heading * 180 / PI;
}

void HMC5883L::calibrate(uint16_t samples, uint16_t delay_ms) {
  int16_t x, y, z;
  int16_t x_min = 32767, y_min = 32767, z_min = 32767;
  int16_t x_max = -32768, y_max = -32768, z_max = -32768;
  
  Serial.println("Rotate the sensor in all directions for calibration...");
  for (uint16_t i = 0; i < samples; i++) {
    readRawData(x, y, z);
    x_min = min(x_min, x);
    x_max = max(x_max, x);
    y_min = min(y_min, y);
    y_max = max(y_max, y);
    z_min = min(z_min, z);
    z_max = max(z_max, z);
    delay(delay_ms);
  }
  
  x_offset = (x_min + x_max) / 2;
  y_offset = (y_min + y_max) / 2;
  z_offset = (z_min + z_max) / 2;
  
  Serial.println("Calibration complete.");
  Serial.print("X offset: "); Serial.println(x_offset);
  Serial.print("Y offset: "); Serial.println(y_offset);
  Serial.print("Z offset: "); Serial.println(z_offset);
}

void HMC5883L::setDeclination(float degrees, float minutes) {
  declination = (degrees + minutes / 60.0) * PI / 180.0;
}

void HMC5883L::writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(HMC5883L_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void HMC5883L::readRegister(uint8_t reg, uint8_t count, uint8_t* data) {
  Wire.beginTransmission(HMC5883L_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(HMC5883L_ADDRESS, count);
  uint8_t i = 0;
  while (Wire.available()) {
    data[i++] = Wire.read();
  }
}

int16_t HMC5883L::twosComplement(int16_t val) {
  if (val >= 0x8000) {
    return val - 0x10000;
  }
  return val;
}


void read_magnetometer()
{ // gives out greedy heading,
}
void switch_magnetometer(bool var)
{
    if (var == 1)
    {
        // Turn the transistor on
        digitalWrite(datalogger_transistor_pin, HIGH);
    }
    else if (var == 0)
    {
        // Function to switch the module off
        // Turn the transistor off
        digitalWrite(datalogger_transistor_pin, LOW);
    }
} 
/*Magnetometer measures the earth's magnetic field, method of constructing a digital compass
greedy heading
*/
void read_magnetometer()
{ // gives out greedy heading,
}

