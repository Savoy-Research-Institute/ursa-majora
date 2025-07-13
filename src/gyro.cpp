#include "gyro.h"

bool is_mpu_on = false;
int mpu_transistor_pin = 7; // Replace with your actual pin
const int mpu_i2c_address = 0x68;

float acc_x, acc_y, acc_z, acc_err_x, acc_err_y;
int16_t acc_x_raw, acc_y_raw, acc_z_raw;

float gyro_x, gyro_y, gyro_z, gyro_err_x, gyro_err_y, gyro_err_z;
int16_t gyro_x_raw, gyro_y_raw, gyro_z_raw;

float roll, pitch, yaw, gyroAngleX, gyroAngleY;
unsigned long currentTime, previousTime;
float elapsedTime;
int c;

void switch_mpu_on()
{
    if (!is_mpu_on)
    {
        digitalWrite(mpu_transistor_pin, HIGH);
        is_mpu_on = true;
    }
}

void switch_mpu_off()
{
    if (is_mpu_on)
    {
        digitalWrite(mpu_transistor_pin, LOW);
        is_mpu_on = false;
    }
}

void reset_mpu_thru_register()
{
    Wire.beginTransmission(mpu_i2c_address);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission(true);
}

void read_mpu_error()
{
    while (is_mpu_on)
    {
        c = 0;
        while (c < 2000)
        {
            Wire.beginTransmission(mpu_i2c_address);
            Wire.write(0x3B);
            Wire.endTransmission(false);
            Wire.requestFrom(mpu_i2c_address, 6, true);
            acc_x_raw = (Wire.read() << 8 | Wire.read());
            acc_y_raw = (Wire.read() << 8 | Wire.read());
            acc_z_raw = (Wire.read() << 8 | Wire.read());
            acc_x = acc_x_raw / 4096.0;
            acc_y = acc_y_raw / 4096.0;
            acc_z = acc_z_raw / 4096.0;
            acc_err_x += (atan((acc_y) / sqrt(pow((acc_x), 2) + pow((acc_z), 2))) * 180 / PI);
            acc_err_y += (atan(-1 * (acc_x) / sqrt(pow((acc_y), 2) + pow((acc_z), 2))) * 180 / PI);
            c++;

            Wire.beginTransmission(mpu_i2c_address);
            Wire.write(0x43);
            Wire.endTransmission(false);
            Wire.requestFrom(mpu_i2c_address, 6, true);
            gyro_x_raw = Wire.read() << 8 | Wire.read();
            gyro_y_raw = Wire.read() << 8 | Wire.read();
            gyro_z_raw = Wire.read() << 8 | Wire.read();
            gyro_err_x += (gyro_x_raw / 65.5);
            gyro_err_y += (gyro_y_raw / 65.5);
            gyro_err_z += (gyro_z_raw / 65.5);
            c++;
        }
        acc_err_x /= 2000;
        acc_err_y /= 2000;
        gyro_err_x /= 2000;
        gyro_err_y /= 2000;
        gyro_err_z /= 2000;
        c = 0;
    }
}

void read_mpu_gyroscope()
{
    while (is_mpu_on)
    {
        previousTime = currentTime;
        currentTime = millis();
        elapsedTime = (currentTime - previousTime) / 1000.0;
        Wire.beginTransmission(mpu_i2c_address);
        Wire.write(0x43);
        Wire.endTransmission(false);
        Wire.requestFrom(mpu_i2c_address, 6, true);
        gyro_x = (Wire.read() << 8 | Wire.read());
        gyro_y = (Wire.read() << 8 | Wire.read());
        gyro_z = (Wire.read() << 8 | Wire.read());
        gyro_x = (gyro_x - gyro_err_x) / 131.0;
        gyro_y = (gyro_y - gyro_err_y) / 131.0;
        gyro_z = (gyro_z - gyro_err_z) / 131.0;
        gyroAngleX = gyro_x;
        gyroAngleY = gyro_y;
        yaw = gyro_z;
        roll = 0.96 * gyroAngleX;
        pitch = 0.96 * gyroAngleY;
    }
}

void read_mpu_accelerometer()
{
    while (is_mpu_on)
    {
        Wire.beginTransmission(mpu_i2c_address);
        Wire.write(0x3B);
        Wire.endTransmission(false);
        Wire.requestFrom(mpu_i2c_address, 6, true);
        acc_x_raw = (Wire.read() << 8 | Wire.read());
        acc_y_raw = (Wire.read() << 8 | Wire.read());
        acc_z_raw = (Wire.read() << 8 | Wire.read());
        acc_x = acc_x_raw / 4096.0;
        acc_y = acc_y_raw / 4096.0;
        acc_z = acc_z_raw / 4096.0;
        accAngleX = (atan(acc_y / sqrt(pow(acc_x, 2) + pow(acc_z, 2))) * 180 / PI) + acc_err_x;
        accAngleY = (atan(-1 * acc_x / sqrt(pow(acc_y, 2) + pow(acc_z, 2))) * 180 / PI) + acc_err_y;
    }
}

void read_mpu_temp()
{
    // Implement temperature reading here if needed later
}

void run_mpu_selftest()
{
    // Implement MPU self-test logic here if needed later
}

void run_mpu_ecomode()
{
    // Implement ecomode settings here if needed later
}

void print_mpu_output()
{
    Serial.print("   pitch");
    if (gyro_y >= 0) Serial.print("+");
    Serial.print(gyro_y);
    if (gyro_y - 2 > 0)
        Serial.print("NoU");
    else if (gyro_y + 2 < 0)
        Serial.print("NoD");
    else
        Serial.print("-----------");

    Serial.print("   roll");
    if (gyro_x >= 0) Serial.print("+");
    Serial.print(gyro_x);
    if (gyro_x - 2 > 0)
        Serial.print("RwD");
    else if (gyro_x + 2 < 0)
        Serial.print("RwU");
    else
        Serial.print("-----------");

    Serial.print("   Yaw");
    if (gyro_z >= 0) Serial.print("+");
    Serial.print(gyro_z);
    if (gyro_z - 2 > 0)
        Serial.println("NoR");
    else if (gyro_z + 2 < 0)
        Serial.println("NoL");
    else
        Serial.println("-----------");
}

void set_mpu_fullscale_range()
{
    Wire.beginTransmission(mpu_i2c_address);
    Wire.write(0x1C);
    Wire.write(0x10);
    Wire.endTransmission(true);

    Wire.beginTransmission(mpu_i2c_address);
    Wire.write(0x1B);
    Wire.write(0x08);
    Wire.endTransmission(true);
}
