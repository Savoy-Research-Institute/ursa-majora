#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

// Initialize all sensors
void sensors_init();

// Read data from MPU6050
void read_mpu6050(float* accel, float* gyro);

// Read ultrasonic sensor distance
float read_ultrasonic();

// Read barometer altitude
float read_barometer();

// Read GPS data
void read_gps(float* lat, float* lon, float* alt);

// Read magnetometer heading
float read_magnetometer();

#endif // SENSORS_H
