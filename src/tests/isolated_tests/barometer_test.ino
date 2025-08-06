#include "BMP280.h"

BMP280 bmp;

void setup() {
  Serial.begin(9600);
  
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
}

void loop() {
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure();
  float altitude = bmp.readAltitude(1013.25); // 1013.25 is the standard atmospheric pressure at sea level

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" Pa");

  Serial.print("Approx. Altitude: ");
  Serial.print(altitude);
  Serial.println(" m");

  Serial.println();
  delay(2000);
}