#ifndef VELMA_MPU6050_H
#define VELMA_MPU6050_H

#include <Wire.h>

#define MPU6050_ADDR 0x68

class VelmaMPU6050 {
public:
    VelmaMPU6050(int transistorPin);

    void begin();
    void wake();
    void sleep();
    void reset();
    void calibrate();

    void update(); // Reads new data and calculates angles

    float getAccelX();
    float getAccelY();
    float getAccelZ();

    float getGyroX();
    float getGyroY();
    float getGyroZ();

    float getRoll();
    float getPitch();
    float getYaw();

    float getTemperature();

    void printOutput();

private:
    int transistorPin;
    bool isOn;

    // Error calibration variables
    float acc_err_x, acc_err_y;
    float gyro_err_x, gyro_err_y, gyro_err_z;

    // Raw sensor values
    int16_t acc_x_raw, acc_y_raw, acc_z_raw;
    int16_t gyro_x_raw, gyro_y_raw, gyro_z_raw;

    // Processed data
    float acc_x, acc_y, acc_z;
    float gyro_x, gyro_y, gyro_z;
    float roll, pitch, yaw;

    unsigned long previousTime;

    int16_t read16bitRegister(uint8_t reg);
};

#endif
