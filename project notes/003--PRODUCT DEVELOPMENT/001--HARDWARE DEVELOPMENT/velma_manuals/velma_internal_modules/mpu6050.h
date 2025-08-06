/*Mpu6050 gyroscope and accelerometer
#Power, 3.6v and level shifted I2C
Read angular motion and acceleration of the x y z axes, greedy orientation from upright via PPower
Acro and self leveling flight,  change sensitivity and full scale range, trigger self tests
Stabilization algorithms, complimentary and kalman filters
Read module temperature
*/
//~~~~~~~~~~~~~~~~~~~MPU6050 functions~~~~~~~~~~~~~~~~~~~~~~~
// Function to switch the MPU on, only switches on when previous status is off
void switch_mpu_on()
{
    // Check if the MPU is currently off
    if (!is_mpu_on)
    {
        // Turn the transistor on
        digitalWrite(mpu_transistor_pin, HIGH);
        // Update the variable to indicate that the MPU is on
        is_mpu_on = true;
    }
}
// Function to switch the MPU off
void switch_mpu_off()
{
    // Check if the MPU is currently on
    if (is_mpu_on)
    {
        // Turn the transistor off
        digitalWrite(mpu_transistor_pin, LOW);
        // Update the variable to indicate that the MPU is off
        is_mpu_on = false;
    }
}
void reset_mpu_thru_register()
{
    Wire.beginTransmission(mpu_i2c_address); // Start communication with MPU6050 // MPU=0x68
    Wire.write(0x6B);                        // Talk to the register 6B
    Wire.write(0x00);                        // Make reset to the mpu  - place a 0 into the 6B register
    Wire.endTransmission(true);              // end the transmission

}
void read_mpu_error()
{ // calculates MPU6050 drift error for both Acc and Gyro
    // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
    // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
    // Read accelerometer values 200 times
    while (is_mpu_on)
    {
        c = 0;
        while (c < 2000)
        {
            Wire.beginTransmission(mpu_i2c_address);
            Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
            Wire.endTransmission(false);
            Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
            // For a range of +-8g, we need to divide the raw values by 4096, according to the datasheet
            acc_x_raw = (Wire.read() << 8 | Wire.read()); // X-axis value
            acc_y_raw = (Wire.read() << 8 | Wire.read()); // Y-axis value
            acc_z_raw = (Wire.read() << 8 | Wire.read()); // Z-axis value
            acc_x = acc_x_raw / 4096.0;
            acc_y = acc_y_raw / 4096.0;
            acc_z = acc_z_raw / 4096.0;
            // Sum all readings
            acc_err_x = acc_err_x + ((atan((acc_y) / sqrt(pow((acc_x), 2) + pow((acc_z), 2))) * 180 / PI));
            acc_err_y = acc_err_y + ((atan(-1 * (acc_x) / sqrt(pow((acc_y), 2) + pow((acc_z), 2))) * 180 / PI));
            c++;

            // Read gyro values 200 times
            Wire.beginTransmission(mpu_i2c_address);
            Wire.write(0x43);
            Wire.endTransmission(false);
            Wire.requestFrom(mpu_i2c_address, 6, true);
            gyro_x_raw = Wire.read() << 8 | Wire.read(); // read values from the first byte, shift them left by 8 bits, then load the rest of the bytes
            gyro_y_raw = Wire.read() << 8 | Wire.read();
            gyro_z_raw = Wire.read() << 8 | Wire.read();
            // Sum all readings
            gyro_err_x = gyro_err_x + (gyro_x_raw / 65.5);
            gyro_err_y = gyro_err_y + (gyro_y_raw / 65.5);
            gyro_err_z = gyro_err_z + (gyro_z_raw / 65.5);
            c++;
        }
        // Divide the sum by 200 to get the error value
        acc_err_x = acc_err_x / 2000;
        acc_err_y = acc_err_y / 2000;
        c = 0;

        // Divide the sum by 200 to get the error value
        gyro_err_x = gyro_err_x / 2000;
        gyro_err_y = gyro_err_y / 2000;
        gyro_err_z = gyro_err_z / 2000;
        // Print the error values on the Serial Monitor
        // Serial.print("gyro_err_x: ");
        // Serial.println(gyro_err_x);
        // Serial.print("gyro_err_y: ");
        // Serial.println(gyro_err_y);
        // Serial.print("gyro_err_z: ");
        // Serial.println(gyro_err_z);
        // Serial.print("acc_err_x");
        // Serial.println(acc_err_x);
        // Serial.print("acc_err_y");
        // Serial.println(acc_err_y);
    }
}
void read_mpu_gyroscope()
{ // reads gyro and accelerometer data from MPU5060 in deg/s
    // i need a function to get the full scale range and divide the neccessary LSB
    // Gyroscope gives angular motion over time, and in deg/sec(travelled_pitch, travelled_yaw etc)
    //  === Read acceleromter data === //
    while (is_mpu_on)
    {
        // === Read gyroscope data === //previous gyro calculations
        previousTime = currentTime;                        // Previous time is stored before the actual time read
        currentTime = millis();                            // Current time actual time read
        elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
        Wire.beginTransmission(MPU);
        Wire.write(0x43); // Gyro data first register address 0x43
        Wire.endTransmission(false);
        Wire.requestFrom(MPU, 6, true);            // Read 4 registers total, each axis value is stored in 2 registers
        gyro_x = (Wire.read() << 8 | Wire.read()); // For a 250deg/s range we have to divide first the raw value by 131.0,
        gyro_y = (Wire.read() << 8 | Wire.read());
        gyro_z = (Wire.read() << 8 | Wire.read());
        // Correct the outputs with the calculated error values
        gyro_x = (gyro_x - gyro_err_x) / 131.0; // gyro_err_x ~(-3.15)
        gyro_y = (gyro_y - gyro_err_y) / 131.0; // gyro_err_y ~(2.96)
        gyro_z = (gyro_z - gyro_err_z) / 131.0; // gyro_err_z ~ (-1.11)
        // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
        gyroAngleX = gyro_x; //* elapsedTime; // deg/s * s = deg
        // premature increment on gyroangleX not in accelerometerangleX
        gyroAngleY = gyro_y; // * elapsedTime;
        yaw = gyro_z;        //* elapsedTime;
        // Complementary filter - combine acceleromter and gyro angle values
        roll = 0.96 * gyroAngleX;
        pitch = 0.96 * gyroAngleY;
        previousTime = currentTime;                        // Previous time is stored before the actual time read
        currentTime = millis();                            // Current time actual time read
        elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
        Wire.beginTransmission(mpu_i2c_address);
        Wire.write(0x43); // Gyro data first register address 0x43
        Wire.endTransmission(false);
        Wire.requestFrom(mpu_i2c_address, 6, true);    // Read 4 registers total, each axis value is stored in 2 registers
        gyro_x_raw = (Wire.read() << 8 | Wire.read()); // For a 1000deg/s range we have to divide first the raw value by 32.8,
        gyro_y_raw = (Wire.read() << 8 | Wire.read());
        gyro_z_raw = (Wire.read() << 8 | Wire.read());

        gyro_x = gyro_x_raw / 65.5;
        gyro_y = gyro_y_raw / 65.5;
        gyro_z = gyro_z_raw / 65.5;
        // Correct the outputs with the calculated error values
        gyro_x = (gyro_x - gyro_err_x) * 1;  // gyro_err_x ~(-0.56)
        gyro_y = (gyro_y - gyro_err_y) * -1; // gyro_err_y ~(2)
        gyro_z = (gyro_z - gyro_err_z) * -1; // gyro_err_z ~ (-0.8)
    }
}
void read_mpu_accelerometer()
{
    while (is_mpu_on)
    {
        Wire.beginTransmission(mpu_i2c_address);
        Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
        Wire.endTransmission(false);
        Wire.requestFrom(mpu_i2c_address, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
        // For a range of +-8g, we need to divide the raw values by 4096, according to the datasheet
        acc_x_raw = (Wire.read() << 8 | Wire.read()); // X-axis value
        acc_y_raw = (Wire.read() << 8 | Wire.read()); // Y-axis value
        acc_z_raw = (Wire.read() << 8 | Wire.read()); // Z-axis value
        acc_x = acc_x_raw / 4096.0;
        acc_y = acc_y_raw / 4096.0;
        acc_z = acc_z_raw / 4096.0;
        // Calculating Roll and Pitch from the accelerometer data
        accAngleX = (atan(acc_y / sqrt(pow(acc_x, 2) + pow(acc_z, 2))) * 180 / PI) + acc_err_x;      // acc_err_x ~(0.58) See more @funct calculate_IMU_error()
        accAngleY = (atan(-1 * acc_x / sqrt(pow(acc_y, 2) + pow(acc_z, 2))) * 180 / PI) + acc_err_y; // acc_err_y ~(-1.58)
    }
}
void read_mpu_temp()
{ // duplicate readmpugyro code to read registers
    /*Registers 65 and 66 in the MPU6050, labeled as TEMP_OUT_H and TEMP_OUT_L respectively, are read-only registers used to store the most recent temperature sensor measurement. Here's an overview of their features:

    Temperature Measurement: These registers hold the temperature sensor measurement data. The temperature measurements are written to these registers at the Sample Rate as defined in Register 25.
    Register Composition: Like other sensor data registers in the MPU6050, the temperature measurement registers consist of two sets: an internal register set and a user-facing read register set. The internal register set is continuously updated at the Sample Rate,
    while the user-facing read register set duplicates the internal register set's data values whenever the serial interface is idle.
    Data Synchronization: The duplication of data values ensures that a burst read of sensor registers will read measurements from the same sampling instant. If burst reads are not used, it's the user's responsibility to ensure that a set of single-byte reads corresponds to a single sampling instant by checking the Data Ready interrupt.
    Temperature Calculation: The scale factor and offset for the temperature sensor are provided in the Electrical Specifications table of the MPU-6000/MPU-6050 Product Specification document.
    The temperature in degrees Celsius for a given register value can be computed using the formula:Temperature in degrees Celsius = (TEMP_OUT Register Value as a signed quantity) / 340 + 36.53It's important to note that the math in the equation is in decimal.
    Parameter: TEMP_OUT is a 16-bit signed value that stores the most recent temperature sensor measurement.
    These temperature measurement registers provide essential data for monitoring the temperature of the MPU6050 sensor, allowing users to make informed decisions based on the temperature readings.*/
}
void run_mpu_selftest()
{
    /*Self-test allows for the testing of the mechanical and electrical portions of the sensors. The self-test for each
    measurement axis can be activated by means of the gyroscope and accelerometer self-test registers
    (registers 13 to 16).
    When self-test is activated, the electronics cause the sensors to be actuated and produce a known output signal.
    The output signal is used to observe the self-test response.*/
}
void run_mpu_ecomode()
{
}
/*register PWR_MGMT_1, HEX 6B...DEC 107 read and write
bit0-2 CLKSEL, bit3 TEMP_DIS, bit4 N/A, bit5 CYCLE, bit6 SLEEP, bit7 DEVICE_RESET
With register 6B (or register 107) of the MPU6050, we can create multiple combinations of features and variations
by configuring its individual bits. Here's a breakdown of the potential features and variations:

Clock Source Selection (CLKSEL):
Feature: Choose the clock source for the device.
Variations:
Internal 8MHz oscillator (Bit 0)
PLL with X, Y, or Z axis gyroscope reference (Bits 1-3)
PLL with external 32.768kHz or 19.2MHz reference (Bits 4-5)

Temperature Sensor Control (TEMP_DIS):
Feature: Enable or disable the temperature sensor.
Variations:
Temperature sensor enabled (Bit 3 = 0)
Temperature sensor disabled (Bit 3 = 1)

Cycle Mode (CYCLE):
Feature: Control the cycling behavior of the device.
Variations:
Device does not cycle (Bit 5 = 0)
Device cycles between sleep mode and sampling data (Bit 5 = 1)

Sleep Mode (SLEEP):
Feature: Enable or disable sleep mode for power-saving.
Variations:
Device remains active (Bit 6 = 0)
Device enters sleep mode (Bit 6 = 1)

Device Reset (DEVICE_RESET):
Feature: Reset all internal registers to default values.
Variations:
No reset (Bit 7 = 0)
Reset activated (Bit 7 = 1)
By combining different settings for each bit, we can create a wide range of configurations to suit various application requirements and optimize the performance of the MPU6050 in different scenarios.

When using SPI interface, user should use DEVICE_RESET (register 107) as well as
SIGNAL_PATH_RESET (register 104) to ensure the reset is performed properly. The sequence
used should be:   1.Set DEVICE_RESET = 1 (register PWR_MGMT_1) 2.       Wait 100ms 3.       Set GYRO_RESET = ACCEL_RESET = TEMP_RESET = 1 (register SIGNAL_PATH_RESET) 4.       Wait 100ms


By combining different settings for each bit, we can create a wide range of configurations to optimize the power consumption and performance of the MPU6050 in various scenarios.
These configurations allow us to tailor the behavior of the MPU6050 according to specific application requirements, whether it's conserving power in low-power modes or maximizing data acquisition frequency for high-performance applications.
With the flexibility offered by register 6C (or register 108), we can fine-tune the behavior of the MPU6050 to meet the needs of different projects,
ensuring efficient operation and optimal performance.
With register 6C (or register 108) of the MPU6050, we can create multiple combinations of features and variations by configuring its individual bits. Here's a breakdown of the potential features and variations:

Wake-up Frequency Control (LP_WAKE_CTRL):
Feature: Control the frequency of wake-ups during Accelerometer Only Low Power Mode.
Variations:
Wake-up frequency set to 1.25 Hz (LP_WAKE_CTRL = 0)
Wake-up frequency set to 5 Hz (LP_WAKE_CTRL = 1)
Wake-up frequency set to 20 Hz (LP_WAKE_CTRL = 2)
Wake-up frequency set to 40 Hz (LP_WAKE_CTRL = 3)
Accelerometer Standby Mode (STBY_XA, STBY_YA, STBY_ZA):
Feature: Put individual axes of the accelerometer into standby mode.
Variations:
X axis accelerometer in standby mode (STBY_XA = 1)
Y axis accelerometer in standby mode (STBY_YA = 1)
Z axis accelerometer in standby mode (STBY_ZA = 1)
Gyroscope Standby Mode (STBY_XG, STBY_YG, STBY_ZG):
Feature: Put individual axes of the gyroscope into standby mode.
Variations:
X axis gyroscope in standby mode (STBY_XG = 1)
Y axis gyroscope in standby mode (STBY_YG = 1)
Z axis gyroscope in standby mode (STBY_ZG = 1)


The WHO_AM_I register (Register DEC 117, HEX 75) provides information about the identity of the MPU-60X0 device by containing the upper 6 bits of its 7-bit I2C address. Here's how we can interpret the features and variations of this register:

Read-only Access: The register is read-only, meaning that it cannot be written to. It is solely used for retrieving information about the device identity.
Device Identification: The WHO_AM_I register helps verify the identity of the MPU-60X0 device by providing its unique I2C address.
I2C Address Configuration: The value stored in the WHO_AM_I register represents the upper 6 bits of the device's 7-bit I2C address. The least significant bit (LSB) of the address is determined by the configuration of the AD0 pin.
Default Value: The default value of the register is 0x68, which corresponds to the default I2C address of the MPU-60X0.
Reserved Bits: Bits 0 and 7 of the register are reserved and hardcoded to 0, meaning they cannot be modified or used for other purposes.
Considering these aspects, the WHO_AM_I register serves a specific function of identifying the device through its I2C address. While it doesn't offer multiple features or variations in terms of configuration,
it plays a crucial role in ensuring proper device identification and communication within an I2C network.



SIGNAL_PATH_RESET  Type: Write Only
Register(Hex), Register(Decimal) Bit7 Bit6 Bit5 Bit4 Bit3 Bit2 Bit1 Bit0
68 104 - - - - - GYRO_RESET ACCEL_RESET TEMP_RESET

Description:
This  register  is  used  to  reset  the  analog  and  digital  signal  paths  of  the  gyroscope,  accelerometer,
and temperature sensors.  The  reset  will  revert  the  signal  path  analog  to  digital  converters  and  filters  to  their  power  up
configurations. Note:  This  register  does  not  clear  the  sensor  registers.    The  reset  initializes  the  serial  interface  as
well. Bits 7 to 3 are reserved.
Parameters:
GYRO_RESET When set to 1, this bit resets the gyroscope analog and digital signal paths.
ACCEL_RESET When  set  to  1,  this  bit  resets  the  accelerometer  analog  and  digital  signal
paths.
TEMP_RESET When set to 1, this bit resets the temperature sensor analog and digital signal
paths.


The SIGNAL_PATH_RESET register (Register 104) is a write-only register used to reset the analog and digital signal paths of the gyroscope, accelerometer, and temperature sensors in the MPU6050. Here's an explanation of its features:

Reset Functionality: The register allows resetting the analog and digital signal paths of the gyroscope, accelerometer, and temperature sensors independently.
Gyroscope Reset: Setting the GYRO_RESET bit to 1 resets the analog and digital signal paths of the gyroscope.
Accelerometer Reset: Setting the ACCEL_RESET bit to 1 resets the analog and digital signal paths of the accelerometer.
Temperature Sensor Reset: Setting the TEMP_RESET bit to 1 resets the analog and digital signal paths of the temperature sensor.
Effect on Sensor Registers: It's important to note that while the signal paths are reset, this action does not clear the sensor registers. The reset initializes the serial interface as well.
Reserved Bits: Bits 7 to 3 are reserved and do not have any specified functionality.
The SIGNAL_PATH_RESET register provides a convenient way to reset the signal paths of the sensors in the MPU6050, ensuring they return to their default configurations.
*/
void print_mpu_output()
{ // Prints out PRTY data (gyroscope).
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
        Serial.print("RwD");
    else if (gyro_x + 2 < 0)
        Serial.print("RwU");
    else
        Serial.print("-----------");
    Serial.print("   Yaw");
    if (gyro_z >= 0)
        Serial.print("+");
    Serial.print(gyro_z);
    if (gyro_z - 2 > 0)
        Serial.println("NoR");
    else if (gyro_z + 2 < 0)
        Serial.println("NoL");
    else
        Serial.println("-----------");
}
void set_mpu_fullscale_range()
{ // calibrate MPU according to sensitivity variables,
    // Configure Accelerometer Sensitivity - Full Scale Range (default +/- 2g)
    Wire.beginTransmission(mpu_i2c_address);
    Wire.write(0x1C); // Talk to the ACCEL_CONFIG register (1C hex)
    Wire.write(0x10); // Set the register bits as 00010000 (+/- 8g full scale range)
    Wire.endTransmission(true);
    // Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
    Wire.beginTransmission(mpu_i2c_address);
    Wire.write(0x1B); // Talk to the GYRO_CONFIG register (1B hex)
    Wire.write(0x08); // Set the register bits as 00001000 (500deg/s full scale)
    Wire.endTransmission(true);

    //compensate for different scales, the variables to divide with when reading the data...
    //change the sensitivity dynamically via blizzard,  
    /*
    Accelerometer 2g 16384LBS/g, 4g 8192LSB/g, 8g 4096LSB/g, 16g 2048LSB/sec. it will show that number for every g of force at each full scale
    Gyroscope  250deg/s 131LSB/deg/s , 500deg/s 65.5LSB/deg/s,  1000deg/s 32.8LSB/deg/s, 2000deg/sec 16.4LSB/deg/s. for every degree of movement, it gives the values, based on fullscale 
    */
}