#ifndef GYRO_H
#define GYRO_H

#include <Arduino.h>
#include <Wire.h>

extern bool is_mpu_on;
extern int mpu_transistor_pin;
extern const int mpu_i2c_address;

// Function declarations
void switch_mpu_on();
void switch_mpu_off();
void reset_mpu_thru_register();
void read_mpu_error();
void read_mpu_gyroscope();
void read_mpu_accelerometer();
void read_mpu_temp();
void run_mpu_selftest();
void run_mpu_ecomode();
void print_mpu_output();
void set_mpu_fullscale_range();

// Declare global variables used by the functions
extern float acc_x, acc_y, acc_z, acc_err_x, acc_err_y;
extern int16_t acc_x_raw, acc_y_raw, acc_z_raw;

extern float gyro_x, gyro_y, gyro_z, gyro_err_x, gyro_err_y, gyro_err_z;
extern int16_t gyro_x_raw, gyro_y_raw, gyro_z_raw;

extern float roll, pitch, yaw, gyroAngleX, gyroAngleY;
extern unsigned long currentTime, previousTime;
extern float elapsedTime;
extern int c;

#endif // MPU6050_H
