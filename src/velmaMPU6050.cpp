#include "VelmaMPU6050.h"

VelmaMPU6050::VelmaMPU6050(int pin) : transistorPin(pin), isOn(false),
    acc_err_x(0), acc_err_y(0), gyro_err_x(0), gyro_err_y(0), gyro_err_z(0),
    roll(0), pitch(0), yaw(0), previousTime(0) {}

void VelmaMPU6050::begin() {
    Wire.begin();
    pinMode(transistorPin, OUTPUT);
    wake();
    reset();
    calibrate();
    previousTime = millis();
}

void VelmaMPU6050::wake() {
    digitalWrite(transistorPin, HIGH);
    isOn = true;

    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission();
}

void VelmaMPU6050::sleep() {
    digitalWrite(transistorPin, LOW);
    isOn = false;

    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x6B);
    Wire.write(0x40);
    Wire.endTransmission();
}

void VelmaMPU6050::reset() {
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission(true);
}

void VelmaMPU6050::calibrate() {
    const int samples = 2000;
    for (int i = 0; i < samples; i++) {
        Wire.beginTransmission(MPU6050_ADDR);
        Wire.write(0x3B);
        Wire.endTransmission(false);
        Wire.requestFrom(MPU6050_ADDR, 6, true);
        acc_x_raw = (Wire.read() << 8 | Wire.read());
        acc_y_raw = (Wire.read() << 8 | Wire.read());
        acc_z_raw = (Wire.read() << 8 | Wire.read());
        acc_err_x += (atan((acc_y_raw / 4096.0) / sqrt(pow((acc_x_raw / 4096.0), 2) + pow((acc_z_raw / 4096.0), 2))) * 180 / PI);
        acc_err_y += (atan(-1 * (acc_x_raw / 4096.0) / sqrt(pow((acc_y_raw / 4096.0), 2) + pow((acc_z_raw / 4096.0), 2))) * 180 / PI);

        Wire.beginTransmission(MPU6050_ADDR);
        Wire.write(0x43);
        Wire.endTransmission(false);
        Wire.requestFrom(MPU6050_ADDR, 6, true);
        gyro_x_raw = Wire.read() << 8 | Wire.read();
        gyro_y_raw = Wire.read() << 8 | Wire.read();
        gyro_z_raw = Wire.read() << 8 | Wire.read();
        gyro_err_x += (gyro_x_raw / 65.5);
        gyro_err_y += (gyro_y_raw / 65.5);
        gyro_err_z += (gyro_z_raw / 65.5);
    }
    acc_err_x /= samples;
    acc_err_y /= samples;
    gyro_err_x /= samples;
    gyro_err_y /= samples;
    gyro_err_z /= samples;
}

void VelmaMPU6050::update() {
    unsigned long currentTime = millis();
    float elapsedTime = (currentTime - previousTime) / 1000.0;
    previousTime = currentTime;

    // Gyro data
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDR, 6, true);
    gyro_x_raw = (Wire.read() << 8 | Wire.read());
    gyro_y_raw = (Wire.read() << 8 | Wire.read());
    gyro_z_raw = (Wire.read() << 8 | Wire.read());
    gyro_x = (gyro_x_raw - gyro_err_x) / 131.0;
    gyro_y = (gyro_y_raw - gyro_err_y) / 131.0;
    gyro_z = (gyro_z_raw - gyro_err_z) / 131.0;

    // Accel data
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDR, 6, true);
    acc_x_raw = (Wire.read() << 8 | Wire.read());
    acc_y_raw = (Wire.read() << 8 | Wire.read());
    acc_z_raw = (Wire.read() << 8 | Wire.read());
    acc_x = acc_x_raw / 4096.0;
    acc_y = acc_y_raw / 4096.0;
    acc_z = acc_z_raw / 4096.0;

    // Calculate roll, pitch, yaw
    roll = 0.96 * (gyro_x * elapsedTime) + 0.04 * (atan(acc_y / sqrt(pow(acc_x, 2) + pow(acc_z, 2))) * 180 / PI);
    pitch = 0.96 * (gyro_y * elapsedTime) + 0.04 * (atan(-1 * acc_x / sqrt(pow(acc_y, 2) + pow(acc_z, 2))) * 180 / PI);
    yaw += gyro_z * elapsedTime;
}

float VelmaMPU6050::getAccelX() { return acc_x; }
float VelmaMPU6050::getAccelY() { return acc_y; }
float VelmaMPU6050::getAccelZ() { return acc_z; }

float VelmaMPU6050::getGyroX() { return gyro_x; }
float VelmaMPU6050::getGyroY() { return gyro_y; }
float VelmaMPU6050::getGyroZ() { return gyro_z; }

float VelmaMPU6050::getRoll() { return roll; }
float VelmaMPU6050::getPitch() { return pitch; }
float VelmaMPU6050::getYaw() { return yaw; }

float VelmaMPU6050::getTemperature() {
    int16_t tempRaw = read16bitRegister(0x41);
    return (tempRaw / 340.0) + 36.53;
}

int16_t VelmaMPU6050::read16bitRegister(uint8_t reg) {
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDR, 2, true);
    return (Wire.read() << 8 | Wire.read());
}

void VelmaMPU6050::printOutput() {
    Serial.print("Roll: "); Serial.print(roll);
    Serial.print(" Pitch: "); Serial.print(pitch);
    Serial.print(" Yaw: "); Serial.println(yaw);
}
