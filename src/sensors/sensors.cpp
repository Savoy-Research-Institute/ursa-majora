#include "sensors.h"

void sensors_init() {
    // Initialize all connected sensors here
}

void read_mpu6050(float* accel, float* gyro) {
    // Read accelerometer and gyroscope data
}

float read_ultrasonic() {
    // Read ultrasonic sensor distance
    return 0.0;
}

float read_barometer() {
    // Read barometer altitude
    return 0.0;
}

void read_gps(float* lat, float* lon, float* alt) {
    // Read GPS data into provided pointers
}

float read_magnetometer() {
    // Read magnetometer heading
    return 0.0;
}
