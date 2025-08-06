// isolated tests for accelerometer, MPU6050...
/*
   Arduino and MPU6050 Accelerometer and Gyroscope Sensor Tutorial
   by Dejan, https://howtomechatronics.com
*/
#include <Wire.h>
#include "mpu.h"
int c = 0;
void setup()
{ 
  Serial.begin(19200);
  Wire.begin(); // Initialize comunication
  reset_mpu_thru_register();
  set_mpu_fullscale_range();
  /// i need a function to get the full scale range and divide the neccessary LSB

  // Call this function if you need to get the IMU error values for your module
  read_mpu_error();
  delay(20);
}
void loop()
{ 
  read_mpu_gyroscope();
  printOutput();
  delay(20);
}


void printOutput()
{
  // Print the values on the serial monitor
  Serial.print("   pitch");
  if (gyro_y >= 0)
    Serial.print("+");
  Serial.print(gyro_y);
  if (gyro_y - 2 > 0)
    Serial.print("NoU");
  else if (gyro_y + 2 < 0)
    Serial.print("NoD");
  else
    Serial.print("-----------");
  Serial.print("   roll");
  if (gyro_x >= 0)
    Serial.print("+");
  Serial.print(gyro_x);
  if (gyro_x - 2 > 0)
    Serial.print("RoU");
  else if (gyro_x + 2 < 0)
    Serial.print("RoD");
  else
    Serial.print("-----------");
  Serial.print("   Yaw");
  if (gyro_z >= 0)
    Serial.print("+");
  Serial.print(gyro_z);
  if (gyro_z - 2 > 0)
    Serial.println("NoU");
  else if (gyro_z + 2 < 0)
    Serial.println("NoD");
  else
    Serial.println("-----------");
}