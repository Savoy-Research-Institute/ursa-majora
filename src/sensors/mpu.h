#ifndef MPU_H
#define MPU_H

#include <Arduino.h>
#include <Wire.h>

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

// Constants and global variables (you may want to make some of these extern)
extern const int mpu_i2c_address;
extern const int mpu_transistor_pin;
extern bool is_mpu_on;

extern float acc_x, acc_y, acc_z;
extern float gyro_x, gyro_y, gyro_z;
extern float acc_err_x, acc_err_y, gyro_err_x, gyro_err_y, gyro_err_z;
extern float gyroAngleX, gyroAngleY, roll, pitch, yaw;
extern float accAngleX, accAngleY;

extern unsigned long previousTime, currentTime, elapsedTime;

#endif // MPU_H