// this is the velma_drone file, this is what will be uploaded into the cube, comprising of flight and non-flight code
#include "ppm.h"  // Include the PPM library used for read_radio function
#include <Wire.h> //I2C library connect to mpu6050
#include <SPI.h>  //Serial communication library
#include <SD.h>   //library for the datalogger SDcard reader
#include <EEPROM.h>
#include <math.h>
#include "customEEPROM.h"
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <zeus.h> //cheat code library that will be toggled via blizzard, this library is initiated in velma and blizzard
// PPM channel layout (update for your situation)
// ### the library supports 8 channels only, can it be modified to use 10? assuming memory is the restraint, using 8 bytes?
#define THROTTLE 3 // Variables for radio channels
#define ROLL 1
#define PITCH 2
#define YAW 4
#define SWITCH3WAY_1 5 // Arm and disarm switch
#define SWITCH3WAY_2 6 // Acro Self leveling Auto, (three way switch)
#define POT1 7         // trim-pot
#define POT2 8         // trim-pot
#define USE_PCINT

#ifdef USE_PCINT
PCattachInterrupt(pin, __vector_ppm_interrupt, CHANGE);
else attachInterrupt(digitalPinToInterrupt(pin), __vector_ppm_interrupt, CHANGE);

const int trigPin = 24; // SoftwareSerial RX pin connected to Arduino pin 24 (Trig pin)
const int echoPin = 25; // SoftwareSerial TX pin connected to Arduino pin 25 (Echo pin)
SoftwareSerial hc12_Serial(22, 23);                 // RX, TX HC12
SoftwareSerial ultrasonic_Serial(trigPin, echoPin); // Create a SoftwareSerial instance
SoftwareSerial gps_Serial(26, 27);                  // RX, TX neo6m
customEEPROM customEEPROM;                          // Create an instance of the CustomEEPROM class
// Loop interval time
const long interval = 50;
unsigned long previousMillis = 0;
int access_password = 2445773;
//flags to indicate modes
bool testMode = false;  // Flag to indicate test mode

// Pins connected to the base of the transistor, they will switch the modules on and off via digital pins
const byte vtx_t_PIN = 28;
const byte hc12_transistor_pin = 30;
const byte gps_transistor_pin = 32; // base for mpu transistor
const byte ultrasonic_t_PIN = 34;
const byte magnetometer_transistor_pin = 25;
const byte datalogger_transistor_pin = 36;
const byte onboard_disarm_switch_value = 37; // this is an onboard disarm pin,
const byte radio_transistor_pin = 38;        // Change this to the pin connected to the transistor's base
const byte shell_docking_pin = 40;           // this pin is used to detect the shell, connected to ground
const byte mpu_transistor_pin = 42;          // base for mpu transistor
                                             // The ultrasonic sensor probe pin is connected to the ultrasonic sensor, read from there via SPI,
bool is_radio_on, is_mpu_on;                 //
bool debouncePressed = false;                // variable to track whether onboard disarm switch has been pressed,
bool undockedMessagePrinted = false;         // Variable to track whether the undocked message has been printed
bool dockedMessagePrinted = false;           // Variable to track whether the docked message has been printed
bool docked = false;                         // variable to track whether drone is docked
bool primary_armed = false;                  // Primary(radio) armed variable
bool secondary_armed = false;                // Secondary(onboard button) armed variable
const int mpu_i2c_address = 0x68;            // MPU6050 I2C address
const byte RADIO_PIN = 11;                   // Define the pin connected to the PPM signal
// Battery variables
const int battery_voltdiv_pin = A0;                                         // volt divider on this pin (12.6/2.5=5.04)
int battery_voltage, battery_capacity, batt_anlog_val, low_battery_voltage; // {voltage(11.1__12.6v)}  {capacity(0__100%)}  {analogVal(0__1023)}
float batt_true_val;                                                        // redundant value, to be deleted

float angle_pitch, angle_roll;
bool set_gyro_angles;
float angle_roll_acc, angle_pitch_acc;
float angle_pitch_output, angle_roll_output;
long acc_x, acc_x_raw, acc_y, acc_y_raw, acc_z, acc_z_raw, acc_total_vector;
float gyro_x, gyro_y, gyro_z;
float gyro_x_raw, gyro_y_raw, gyro_z_raw;
float AccX, AccY, AccZ;
float gyroAngleX, gyroAngleY, gyroAngleZ;
float accAngleX, accAngleY;
float gyro_err_x, gyro_err_y, gyro_err_z;
float acc_err_x, acc_err_y;
float roll_level_adjust, pitch_level_adjust;
float elapsedTime, currentTime, previousTime;
int c = 0;
int esc_1, esc_2, esc_3, esc_4;
unsigned long loop_timer;
unsigned long timer_channel_1, timer_channel_2, timer_channel_3, timer_channel_4, esc_timer, esc_loop_timer;

short throttle, roll, pitch, yaw, switch3way_1, switch3way_2, pot1, pot2;
bool auto_level, acro_mode; // variables to toggle acrobatic and self leveling modes
int start;

float pid_error_temp; // PID variables
float pid_i_mem_roll, pid_roll_setpoint, gyro_roll_input, pid_output_roll, pid_last_roll_d_error;
float pid_i_mem_pitch, pid_pitch_setpoint, gyro_pitch_input, pid_output_pitch, pid_last_pitch_d_error;
float pid_i_mem_yaw, pid_yaw_setpoint, gyro_yaw_input, pid_output_yaw, pid_last_yaw_d_error;
float pid_p_gain_roll = 0.45;             // Gain setting for the roll P-controller (1.3)
float pid_i_gain_roll = 0.15;             // Gain setting for the roll I-controller (0.3)
float pid_d_gain_roll = 7.0;              // Gain setting for the roll D-controller (15)
int pid_max_roll = 400;                   // Maximum output of the PID-controller (+/-)
float pid_p_gain_pitch = pid_p_gain_roll; // Gain setting for the pitch P-controller.
float pid_i_gain_pitch = pid_i_gain_roll; // Gain setting for the pitch I-controller.
float pid_d_gain_pitch = pid_d_gain_roll; // Gain setting for the pitch D-controller.
int pid_max_pitch = pid_max_roll;         // Maximum output of the PID-controller (+/-)
float pid_p_gain_yaw = 3.0;               // Gain setting for the pitch P-controller. //4.0
float pid_i_gain_yaw = 0.02;              // Gain setting for the pitch I-controller. //0.02
float pid_d_gain_yaw = 0;                 // Gain setting for the pitch D-controller.0 because yaw has props drag already
int pid_max_yaw = 400;                    // Maximum output of the PID-controller (+/-)in degrees per second,
// this is to be above the current after fine tuning give or take,
const char *cheatCode1 = "osiris"; // Example cheat code
const char *cheatCode2 = "anubis"; // Another example cheat code
const char *cheatCode3 = "Cleopatra";
const char *cheatCode4 = "Turtle_mode";
const char *cheatCode5 = "Pussypower";   // stabilization algorithms to calculate error between greedy orientation and upright, correction kinda like how a cat can
const char *cheatCode6 = "Earthmagnet";  // use height above ground to lock altitude
const char *cheatCode7 = "Nightcrawler"; // the camera brick uses onboard IR LEDs to simulate light amplification night vision
const char *cheatCode8 = "Cruise";       // use height above ground and absolute altitude to perform autopilot, maintain heading, velocity etc,
const char *cheatCode9 = "Shepherd";     // beepers help locate at close distances, onboard flashlights and LEDs help locate at night, GPS data used to locate during a crash
const char *cheatCodes[] = {"cruise", "Shepherd", "Turtle_mode", "Cleopatra", "osiris", "anubis", "Earthmagnet", "Pussypower", "Nightcrawler"};
bool cheatCodeMode = false; // Variable to track cheat code mode

// Instructions on how to calibrate the PID controller explained in velma_ctchme
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{ // This part of the program runs once, until reset, It is used to setup everything for the smooth running of the program
    // Example usage of the custom EEPROM functions
    // customEEPROM.name("Savoy");
    // customEEPROM.
    // String storedName = customEEPROM.readName();
    // Serial.println("Stored name: " + storedName);

    /*

            int velmaUID = b01001001;
        Does not erase during reset,
        Store UID, total flight time, access password, home coordinates,

            record home location on startup, check constellation count, many satelittes reduces error,
            collect multiple readings, compare, set home.
            manually input home location via blizzard
            switch on gyro, wait 30md, calibrate gyro
            wait for blizzard, connect to blizzard via access password,

        */
    // EEPROMclear();
    // EEPROM.read(address);
    /*
    int a = 0;
    int value;
    value = EEPROM.read(a);

    Serial.print(a);
    Serial.print("\t");
    Serial.print(value);
    Serial.println();

    a = a + 1;

    if (a == 512)
        a = 0;
    delay(500);
    */

    // first things to be done, on startup
    Wire.begin();                  // Initialize I2C comunication
    Serial.begin(19200);           // initialize communication with Serial
    Serial1.begin(9600);           // Initialize communication with NEO-6M GPS module
    mySerial.begin(9600);          // HC-12 default baud rate
    ultrasonic_Serial.begin(9600); //

    // Serial.println("HC-12 Receiver ready");
    SD.begin(53);                // You also have to initialize the SD card module at the Chip Select (CS) pin .
    ppm.begin(RADIO_PIN, false); // Start the PPM function on PIN 11. This attaches an interrupt at RADIO_PIN
                                 // Set the transistor pins as an output, the below pins toggle the respective devices on and off by being high or low

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(radio_transistor_pin, OUTPUT);
    pinMode(mpu_transistor_pin, OUTPUT);
    pinMode(vtx_t_PIN, OUTPUT);
    pinMode(hc12_transistor_pin, OUTPUT);
    pinMode(datalogger_transistor_pin, OUTPUT);
    pinMode(ultrasonic_t_PIN, OUTPUT);
    pinMode(gps_transistor_pin, OUTPUT);
    pinMode(onboard_disarm_switch_value, INPUT_PULLUP); // this is an onboard disarm pin, while armed, pressing this pin initiates a disarm
    pinMode(shell_docking_pin, INPUT_PULLUP);           // Docked or Undocked to the shell, this pin is an INT_PULLUP,if the pin is drained, it means the cube is connected to the shell
    //  connected to 5v via pullup resistors, naturally on high state
    //  on the shell, the pin is connected to ground,
    //
    DDRH |= B00010000; // h4,pin7 Declare ESCs pins as OUTPUT
    DDRH |= B00001000; // h3,pin6  Using bitwise operation is significantly faster than digitalWrite..
    DDRE |= B00001000; // e3,pin5
    DDRG |= B00100000; // g5,pin4
    // LED startup indicator comes here
    switch_mpu_on();
    delay(30); // give time for MPU to turn on, as specified in the manual
    reset_mpu_thru_register();
    set_mpu_fullscale_range(); // 8g and 500deg/s
    read_mpu_error();          // Let's take multiple gyro data samples so we can determine the average gyro offset (calibration).
                               // Now that we have 2000 measures, we need to divide by 2000 to get the average gyro offset.
    // The drone should be still at this moment
    switch_radio_on();
    read_radio_allchannels(); // switch3way_1 (1000=Disarm, 2000=Arm)
    check_docking_status();
    on_startup(); // if docked, keep the escs silent
    // Wait until the receiver is active and the Armed, after the flip, it then goes into void loop
    while (switch3way_1 < 1200 && docked) // while docked?
    {                                     // first state of switch on TX startup(disarmed)
        start++;                          // While waiting increment start whith every loop.

        // ESCS ESCS ESCS
        // We don't want the esc's to be beeping annoyingly. So let's give them a 1000us puls while waiting for the receiver inputs.
        PORTH |= B00011000; // set pin 6, 7 high(Set digital port 4, 5, 6 and 7 high.)
        PORTE |= B00001000; // Pin5
        PORTG |= B00100000; // Pin4
        // PH,PE,PG ESCS FOR MEGA
        delayMicroseconds(1000); // Wait 1000us (We can use delayMicroseconds because the receiver interrupt routine is not used).
        PORTH &= B11100111;      // set pin 6, 7 LOW(Set digital ports 4, 5, 6 and 7 low.)
        PORTE &= B11110111;      // Pin5
        PORTG &= B11011111;      // Pin4
        delay(3);                // Wait 3 milliseconds before the next loop.
        if (start == 125)
        {                                     // Every 125 loops (500ms).
            digitalWrite(8, !digitalRead(8)); // Change the led status.
            start = 0;                        // Start again at 0.
        }
        read_radio_allchannels();
    }
    start = 0; // reset start          //Set start back to 0.
    // Use the led on the Arduino for startup indication
    digitalWrite(8, HIGH); // Turn on the warning led.
    // delay(20);
    Serial.println("startup sequence complete");
} // RUN cube VEC////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// knows what invalid data looks like and what valid data looks like for everything, after checking this, turn everything down and go into eco_mode.... wait for radio or blizzard,
void loop()
{ /*  PID setup
    docked flight time
    print docked message,
    Pre and Post vitals examination checkup, before and after propellers
    switchA three way switch (full acro [acro dynamic, self leveling dynamic] full self leveling)
    dynamic, use HC12 for complex control, throttle midstick means to hover due to autonomous compensation
    proper sensor calibration for every module,
    cheatcodes in form of functions. Acro and self leveling code available.

    Autonomous control, GPS based delivery, Machine vision and obstacle avoidance


    undocked, no flight time


*/
    checkAccessCode();
    if (accessGranted)
    {
        // For example, allow cheat code entry, handle auxiliary switch input, etc.
    }
    else
    {
        // Access not yet granted, continue waiting for access code
    }

    // this runs in loops after void setup
    check_docking_status();
    if (!docked) // not docked
    {
        if (!undockedMessagePrinted)
        {
            Serial.println("Drone is undocked. Non-flight mode.");
            undockedMessagePrinted = true; // Set the flag to true after printing the message once
        }
        switch_radio_off();
        switch_gps_on();
        // Add non-flight code here, e.g., GPS-related functionality
    }

    if (docked) // if the cube and the shell are connected, it knows its time to fly
    {           /*Flight control code here*/
        
        // take home location, take current location, time alttitude etc,
        // While docked, it should know its time to fly,
        if (!dockedMessagePrinted)
        {
            Serial.println("Drone is docked.flight mode.");
            dockedMessagePrinted = true; // Set the flag to true after printing the message once
        }
        // radio is in at this point maybe..

    }


        // Normal flight control operations
        imuData, gpsData = readSensorData();
        if (checkFailsafeConditions(imuData, gpsData))
        {
            continue;
        }
        setpoints = getSetpoints(currentMode, imuData, gpsData);
        rollOutput, pitchOutput, yawOutput = computePIDOutput(imuData, setpoints);
        updateMotorSpeeds(rollOutput, pitchOutput, yawOutput);
        logData(imuData, gpsData, rollOutput, pitchOutput, yawOutput);
        sendTelemetryData(imuData, gpsData, rollOutput, pitchOutput, yawOutput);
    }

    readCheatCode();
    handleKeypadInput();

    if (hc12.available())
    {
        String incomingMessage = hc12.readStringUntil('\n');
        Serial.print("Received from Blizzard: ");
        Serial.println(incomingMessage);

        // Check if the message is an ACK request
        if (incomingMessage == "REQUEST_ACK")
        {
            sendAck();
        }
    }

    bool ackReceived = requestAck();
    if (ackReceived)
    {
        Serial.println("ACK received from Blizzard");
    }
    else
    {
        Serial.println("Failed to receive ACK after retries");
    }

    delay(5000); // Delay before the next request
}
/*
for (int i = 0; i < EEPROM.length(); i++)
{
    EEPROM.write(i, 0);
}
// This example code is in the public domain.

// turn the LED on when we're done
digitalWrite(13, HIGH);
}
*/

// Main flight control loop
void mainLoopAA()
{
    initializeFlightControl();
    initializeFlightModes();
    initializeCalibration();
    setupDataLogging();
    setupTelemetry();

    while (true)
    {
        imuData, gpsData = readSensorData();
        if checkFailsafeConditions (imuData, gpsData) : continue

    setpoints = getSetpoints(currentMode, imuData, gpsData);
        rollOutput, pitchOutput, yawOutput = computePIDOutput(imuData, setpoints);
        updateMotorSpeeds(rollOutput, pitchOutput, yawOutput);
        logData(imuData, gpsData, rollOutput, pitchOutput, yawOutput);
        sendTelemetryData(imuData, gpsData, rollOutput, pitchOutput, yawOutput);
    }
}
// Update the main flight control loop to include cheat code reading
void mainLoopBB()
{
    initializeFlightControl();
    initializeFlightModes();
    initializeCalibration();
    setupDataLogging();
    setupTelemetry();
    setupCheatCodes();

    while (true)
    {
        imuData, gpsData = readSensorData();
        if checkFailsafeConditions (imuData, gpsData) : continue

    setpoints = getSetpoints(currentMode, imuData, gpsData);
        rollOutput, pitchOutput, yawOutput = computePIDOutput(imuData, setpoints);
        updateMotorSpeeds(rollOutput, pitchOutput, yawOutput);
        logData(imuData, gpsData, rollOutput, pitchOutput, yawOutput);
        sendTelemetryData(imuData, gpsData, rollOutput, pitchOutput, yawOutput);

        readCheatCode(); // Add this line to read and process cheat codes
    }
}
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^END OF VOID LOOP^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// LEVEL A CODE, this is code that the cube uses to interface with hardware, and to basically run without doing much, any code interfacing with hardware


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^MPU^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
Considering these aspects, the WHO_AM_I register serves a  specific function of identifying the device through its I2C address. While it doesn't offer multiple features or variations in terms of configuration,
it plays a crucial role in ensuring proper device identification and communication within an I2C network.



SIGNAL_PATH_RESET  Type: Write Only
Register(Hex), Register(Decimal) Bit7 Bit6 Bit5 Bit4 Bit3 Bit2 Bit1 Bit0
68 104 - - - - - GYRO_RESET ACCEL_RESET TEMP_RESET

Description:
This  register  is  used  to  reset  the  analog  and  digital  signal  paths  of  the  gyroscope,  accelerometer,
and temperature sensors.  T[he  reset  will  revert  the  signal  path  analog  to  digital  converters  and  filters  to  their  power  up
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

    // compensate for different scales, the variables to divide with when reading the data...
    // change the sensitivity dynamically via blizzard,
    /*
    Accelerometer 2g 16384LBS/g, 4g 8192LSB/g, 8g 4096LSB/g, 16g 2048LSB/sec. it will show that number for every g of force at each full scale
    Gyroscope  250deg/s 131LSB/deg/s , 500deg/s 65.5LSB/deg/s,  1000deg/s 32.8LSB/deg/s, 2000deg/sec 16.4LSB/deg/s. for every degree of movement, it gives the values, based on fullscale
    */
}

/*Neo6M gps,
REad and parse NMEA sentenses to get position, velocity and time data, absolute altitude, track made good, DOP, constellation data
Position of status, System mode indicator, GPRMC, GPGGA, GPGSA, GPGLL.
GPS lock, absolute altitude redundant source, change data sample rate, change power modes, waypoint.read(){}  waypoint.write(){}
the blizzard tells the cube heading and distance to follow by streaming positive and negative error
*/
//************************GPS functions***************************************************************
// the neo6m connects to the arduino via SPI, software serial, switched on and off via a transistor and an arduino pin
// Function to switch the module on
void switch_gps_on()
{
    // Turn the transistor on
    digitalWrite(gps_transistor_pin, HIGH);
}
// Function to switch the module off
void switch_gps_off()
{
    // Turn the transistor off
    digitalWrite(gps_transistor_pin, LOW);
}
void read_gps()
{
    // Check if data is available to read from NEO-6M GPS module
    if (Serial1.available())
    {
        // Read the incoming byte from NEO-6M GPS module
        char incomingByte = Serial1.read(); // change from hardware serial to software serial

        // Print the incoming byte to Serial Monitor
        Serial.print(incomingByte);
    }
}
void change_gps_sample_rate()
{
    // change the gps data sample rate, the blizzard equivalent to this in seting form,
}
void change_gps_power_modes()
{
    // max power, power saving, eco mode, etc
}
void gps_loc_lock()
{
    // this feature, allows the drone to return back to designated location and stay there, it takes the heading between the two greedy and destination coordinates
    // and calculates an error in degrees to, that the drone can use to fly back to, the algorithm that does this...
}
void waypoint_read()
{
    // this reads, and stored waypoint data over a period of time, such that can be used to reconstruct the same.
}
void waypoint_write()
{
    // this uses algorithms to calculate heading and distance between two coordinates, calculates their error data that can influence flight,
}
void absolute_altitude_redundancy()
{
    // this function checks barometer and gps altitude data, if similar to an error of like 5%, it reads good, otherwise it flags,
}
void run_gps_ecomode()
{
    /*
    During a Cold start, a receiver in Eco Mode works exactly as in Maximum Performance Mode. Once a position
    can be calculated and a sufficient number of satellites are being tracked, the acquisition engine is
    powered off resulting in significant power savings. The tracking engine continuously tracks acquired
    satellites and acquires other available or emerging satellites.
    Note that even if the acquisition engine is powered off, satellites continue to be acquired.*/
}

// Function to calibrate GPS
void calibrateGPS()
{ // Perform GPS calibration steps
    gpsCalibrationData = gatherGPSCalibrationData();
    applyGPSCalibration(gpsCalibrationData);
}
// Function to parse GPS data
void parseGPSData(gpsData)
{
    latitude = extractLatitude(gpsData);
    longitude = extractLongitude(gpsData);
    altitude = extractAltitude(gpsData) return (latitude, longitude, altitude);
}

void check_docking_status()
{
    //~~~~~~~~~~~~~~~~~~Docking sequence~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /*Docking refers to the state the cube is in when physically plugged into the shell
        Undocked is when its not plugged
        PIn 40 on arduino is the shell pin, inputpullup by nature,
        meaning it remains high until the cube is docked, then its state changes to low
    */
    // Read the state of the shell pin,
    int shellState = digitalRead(shell_docking_pin); // if low, the shell is connected to the cube,
    // Do flight basically

    // Check if the shell is connected
    if (shellState == LOW)
    { // docked
        docked = true;
        // Shell is detected
        Serial.println("Shell detected!");
        // You can add more actions here if needed
    }
    else
    { // if shell pin is high
        // Shell is not detected
        docked = false;
        Serial.println("No shell detected.");
        // You can add more actions here if needed
    }
}

void idle_mode()
{
    // after startup, the drone remains in idle mode until the blizzard connects to it,
}
/*Timers
Keep a local clock, time sourced and updated from GPS, Read, blizzard streams drone coordintes, saved in the data logger in a FILO format
*/
void begin_local_clock()
{
    // read time data from GPS, keep a local clock and remind(update) timers every 10 minutes
    // capacitors needed,
    // trigger pin, read pin,
    // a 16bit adder,
}
void calculate_total_flight_time()
{
    // calculate total flight time from local clock, any time its above ground,
    // update EEPROM, every time it lands, or update SDcard file,
}
/*Error detection and prevention
Failsafes against GPS, magentometer and barometer failure
Redundant sources of similar data,
*/
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Error prevention, detection, solution^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Function to setup failsafe mechanisms
void setupFailsafe()
{
    configureFailsafeParameters();
}
// Function to check for failsafe conditions
void checkFailsafeConditions(imuData, gpsData)
{ // checks IC registers, on change, there is an error detection thing that gets turned up...
    if imuData
        is invalid or gpsData is invalid : triggerFailsafe() return True return False
}
// Function to trigger failsafe
void triggerFailsafe()
{
    landDrone();
    Serial.print("Failsafe triggered");
}
void fail_safe()
{ // error detection, parameters being the intended failure
  // gps failure, if serial, not available, failsafes(gps),
}

bool accessGranted = false; // Variable to track access status

// Define access code
const char *accessCode = "access123"; // Example access code

void checkAccessCode(const char *receivedCode)
{
    // Compare received code with predefined access code
    if (strcmp(receivedCode, accessCode) == 0)
    {
        // Access code matches, grant access
        accessGranted = true;
        // Implement access granted functionality (e.g., display message on LCD)
    }
    else
    {
        // Access code does not match, deny access
        accessGranted = false;
        // Implement access denied functionality (e.g., display error message on LCD)
    }
}

void receiveAccessCode(const char *receivedCode)
{
    // Check received access code
    checkAccessCode(receivedCode);
}
//
/*HC12 transciever connect to blizzard, the read pin is under an interrupt,
Terminal communication with blizzard, the terminal receives text, inteprets for the drone, CMD, cursor placement
*/
//~~~~~~~~~~~~~~~~~~~~~HC12 functions~~~~~~~~~~~~~~~~~~~~~~~~~
void switch_hc12_on()
{
    // Turn the transistor on, only when off
    if (!digitalRead(hc12_transistor_pin))
    {
        digitalWrite(hc12_transistor_pin, HIGH);
    }
}

// Function to switch the module off
// hardware serial on interrupt, it always listens, if manual controlled and docked, it starts to receive 8channels,
// controls the drone manually, enter cheatcodes, etc,
void switch_hc12_off()
{
    // Turn the transistor off, only when on
    if (digitalRead(hc12_transistor_pin))
    {
        digitalWrite(hc12_transistor_pin, LOW);
    }
}
String receivedMessage;
String messageToSend = "Hello from Arduino!";
// hc12_read(receivedMessage);
void read_hc12(const String &receivedMessage)
{
    if (mySerial.available())
    {
        receivedMessage = mySerial.readStringUntil('\n');
        Serial.println("Message received: " + receivedMessage);
    }
}
// hc12_write(messageToSend);
void write_hc12(const String &messageToSend)
{
    mySerial.println(messageToSend);
    Serial.println("Message sent: " + messageToSend);
    delay(1000); // Send a message every second
}
// Function to setup telemetry
void setupTelemetry()
{
    initializeTelemetryModule();
}
// Function to send telemetry data
void sendTelemetryData(imuData, gpsData, pidOutputs)
{
    sendDataToGroundStation(imuData, gpsData, pidOutputs);
}
void readTelemetryData(imuData, gpsData, pidOutputs);
// Define SoftwareSerial pins for HC-12
SoftwareSerial hc12(10, 11); // RX, TX

const int maxRetries = 5;
const int ackTimeout = 1000; // Timeout in milliseconds

bool requestAck()
{
    int retries = 0;
    bool ackReceived = false;

    while (retries < maxRetries && !ackReceived)
    {
        hc12.println("REQUEST_ACK");
        Serial.println("Sent ACK request");

        unsigned long startTime = millis();
        while (millis() - startTime < ackTimeout)
        {
            if (hc12.available())
            {
                String response = hc12.readStringUntil('\n');
                if (response == "ACK")
                {
                    ackReceived = true;
                    break;
                }
            }
        }

        if (!ackReceived)
        {
            retries++;
            Serial.println("Retrying ACK request...");
        }
    }

    return ackReceived;
}

void sendAck()
{
    hc12.println("ACK");
    Serial.println("Sent ACK");
}
//***************************Blizzard section****************************************************
/*Cheat codes
Cruise use height above ground and absolute altitude to perform autopilot, maintain heading, velocity etc,
Shepherd night and day, beepers help locate at close distances, onboard flashlights and LEDs help locate at night, GPS data used to locate during a crash
Nightcrawler the camera brick uses onboard IR LEDs to simulate light amplification night vision
Earthmagnet use height above ground to lock altitude via a switch,
Ppower, uses gyroscope and accelerometer data,
Turtle_mode
Osiris
Cleopatra


change this such that, if i have one cheat code x, the switches will have functions x1a to x4a,
if i have cheat code d, the switches will have function d1a, im just giving examples for context,
if i write the cheat code  i can access its capabilities and map them to the switches,  i can toggle a switch,  click,
map it to a cheatcode, map a functionality to the digital volume,
*/
// couple this with the hc12, such that it toggles immediately,
//  Define cheat codes and corresponding actions
// make a list store all cheat codes, the index in that list, defines the switch case to use, so that checkCheatcode runs received strings, thru this list
//**************************cheatcode functions*********************
void night_crawler_()
{
    // this cheatcode toggles the infrared band around the camera on and off,
}
void shepherd_()
{ // the blizzard will request fetch for GPS pvt data...
  // onboard flashlight led to help locate at night,
  // beepers help locate at close distances,
  // the drone gps data is used to calculate heading to follow, in form of positive and negative error
}
// search and rescue
void pussy_power()
{
    // cheatcode, finds down direction from gravity, such that it can stabilize the drone, this can be used in the training_mode, set by parent_console
    // this is a reference, to cats and their ability to always land on their feet,
    // find greedy orientation, from upright, calculate an error, that if need be will be factored into the ESC calculations, if the cheatcode is set
    // stabilization algorithms, complimentary ad kalman filters,
}
void cleopatra_()
{
    // this is a cheatcode that reads from the blizzard gyro, uses that data to influence flight
}
void cruise_()
{
    // depending on the user input, the drone will hover or continue moving at the given trajectory by the user
    // using gps accelerometer, magetometer and gyro data
    gps_loc_lock();
    altitude_hold();
}
// Function to execute "osiris" cheat code
void Osiris_()
{
    // Implement the specific action for "osiris"
    performSpecialManeuver();
    Serial.print("Executed cheat code: osiris");
}
// Function to execute "anubis" cheat code
void Anubis_()
{
    // Implement the specific action for "anubis"
    enableHighSpeedMode();
    Serial.print("Executed cheat code: anubis");
} // Implement more cheat code functions as needed
void turtle_mode()
{
    // using a hbridge, reverse the rotation of two motors to flip back
}
void earth_magnet()
{
    // absolute and real altitude lock
}
//**************************************cheatcode management functions********************************************
void enterCheatCodeMode()
{
    // Activate cheat code mode
    cheatCodeMode = true;
    // Display blinking cursor on LCD
    // Implement LCD blinking cursor functionality
}
void exitCheatCodeMode()
{
    // Deactivate cheat code mode
    cheatCodeMode = false;
    // Clear LCD
    // Implement LCD clear functionality
}
// Function to setup cheat code system
void setupCheatCodes()
{
    initializeHC12();
    Serial.println("Cheat code system initialized");
}

// Function to read and process cheat codes
void readCheatCode()
{
    if hc12Available ()
    {
        cheatCode = readHC12();
        processCheatCode(cheatCode);
    }
}
void checkCheatCode(const char *enteredCode)
{
    // Compare entered code with predefined cheat codes
    if (strcmp(enteredCode, cheatCode1) == 0)
    {
        // Execute corresponding action for cheat code 1
        // For example, toggle Blizzard and transmit message via HC12
        // Implement Blizzard toggling and HC12 transmission
    }
    else if (strcmp(enteredCode, cheatCode2) == 0)
    {
        // Execute corresponding action for cheat code 2
        // Implement desired functionality
    }
    else if (strcmp(enteredCode, cheatCode3) == 0)
    {
        // Execute corresponding action for cheat code 3
        // Implement desired functionality
    }
    else if (strcmp(enteredCode, cheatCode4) == 0)
    {
        // Execute corresponding action for cheat code 4
        // Implement desired functionality
    }
    else if (strcmp(enteredCode, cheatCode5) == 0)
    {
        // Execute corresponding action for cheat code 5
        // Implement desired functionality
    }
    else if (strcmp(enteredCode, cheatCode6) == 0)
    {
        // Execute corresponding action for cheat code 6
        // Implement desired functionality
    }
    else if (strcmp(enteredCode, cheatCode7) == 0)
    {
        // Execute corresponding action for cheat code 7
        // Implement desired functionality
    }
    else if (strcmp(enteredCode, cheatCode8) == 0)
    {
        // Execute corresponding action for cheat code 2
        // Implement desired functionality
    }
    else if (strcmp(enteredCode, cheatCode9) == 0)
    {
        // Execute corresponding action for cheat code 2
        // Implement desired functionality
    }
    else
    {
        // Invalid cheat code entered
        // Display error message on LCD or take appropriate action
    }
}

// Function to process cheat code
void processCheatCode(String cheatCode)
{
    switch
    cheatCode:
    case "osiris":
        executeOsiris();
case "anubis":
    executeAnubis();
case "testmode";
    toggleTestMode();
// Add more cheat codes as needed
}
void handleCheatCodeInput(char key) // make it handle many cheats concurrently
{                                   // this is one cheatcode per time,
static char enteredCode[20];    // Array to store entered cheat code
static int cursorPosition = 0;  // Cursor position in entered code

if (key == 'B')
{
    // User confirms entered cheat code
    enteredCode[cursorPosition] = '\0'; // Null terminate entered code
    checkCheatCode(enteredCode);        // Check entered cheat code
    exitCheatCodeMode();                // Exit cheat code mode
}
else
{
    // Append pressed key to entered cheat code
    enteredCode[cursorPosition++] = key;
    // Display entered code on LCD
    // Implement LCD display functionality
}
}

void handleAuxSwitchInput(int switchNumber)
{
if (cheatCodeMode)
{
    // Handle switch input only if in cheat code mode
    // Map switch number to corresponding key press (e.g., A, B, C, D)
    char key;
    switch (switchNumber)
    {
    case 1:
    key = 'A'; // Replace code with radio switches code,
    break;
case 2:
    key = 'B';
    break;
case 3:
    key = 'C';
    break;
case 4:
    key = 'D';
    break;
default:
    // Invalid switch number
    return;
}
handleCheatCodeInput(key); // Handle cheat code input
}
}

/*Ultrasonic sensor
Design hand to hold sensor,
Record height above ground.
Altitude lock using height above ground.
*/
//~~~~~~~~~~~~~~~~~~~~~~~Ultrasonic functions~~~~~~~~~~~~~~~~~~~~~~~
void switch_ultrasonic(bool var)
{
    if (var == 1)
    {
        // Turn the transistor on
        digitalWrite(ultrasonic_t_PIN, HIGH);
    }
    else if (var == 0)
    {
        // Function to switch the module off
        // Turn the transistor off
        digitalWrite(ultrasonic_t_PIN, LOW);
    }
}

void read_ultrasonic_sensor()
{
    long duration, distance_cm;

    // Clear the trigger pin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Send a 10 microsecond pulse to the trigger pin
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Measure the duration of the echo pulse
    duration = pulseIn(echoPin, HIGH);

    // Calculate distance in centimeters (cm)
    distance_cm = duration * 0.034 / 2;

    // Print the distance to the serial monitor
    Serial.print("Distance: ");
    Serial.print(distance_cm);
    Serial.println(" cm");
    //  delay(1000);  // Wait for a moment before taking the next measurement
}
void altitude_hold()
{
}
void calibrate_ultrasonic_sensor()
{
}

/*SD card data logger
Read and write to an SD card, store cheat code scripts, failsafe code, blackbox data
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~DATALOGGER functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*To open a new file in the SD card, you need to create a file object that refers to your data file.
For example: dataFile = SD.open("data.txt", FILE_WRITE);
To write data to the currently open file, you use: dataFile.write(data);
In which the dataFile is the file object created previously and the data is what you want to write
in the file.

You can also use the print() or println() functions to print data into the file:
dataFile.print(data);
dataFile.println(data); // followed by a new line
To read the data saved on your file:
dataFile.read();
You can only write within a file at once, so you need to close a file before proceeding to the next one.
To close the data.txt file we’ve just created:
SD.close("data.txt");
The argument of this function is the file you want to close, in this case data.txt.
*/
void switch_datalogger_on(bool var)
{
    if (var == 1)
    {
        // Turn the transistor on
        digitalWrite(datalogger_transistor_pin, HIGH);
    }
    else if (var == 0)
    {
        // Function to switch the module off
        // Turn the transistor off
        digitalWrite(datalogger_transistor_pin, LOW);
    }
}
void read_from_datalogger()
{
}
void write_to_datalogger()
{
}

// Function to setup data logging
void setupDataLogging()
{
    initializeSDCard();
    createLogFile();
}
// Function to log data
void logData(imuData, gpsData, pidOutputs)
{
    writeLogData(imuData, gpsData, pidOutputs);
}

/************************Periphanelia functions***************************************************
Beepers LEDs Photoresistors section
Battery low beep and LED sequences,
Shepherd day and night sequences, use the photoresistor to know if day or night,*/
void switch_beeper(bool var)
{
    if (var == 0)
    {
        // switch beepers on,
        digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
        delay(1000);                     // wait for a second
    }
    else if (var == 1)
    {
        // switch beepers off,
        digitalWrite(LED_BUILTIN, LOW); // turn the LED off (LOW is the voltage level)
        delay(1000);                    // wait for a second
    }
}
// BEEPER and LEDS sequences
//Different LEDS need different levels of current, thus different current limiting resistors, 
void switch_green_led(bool var)
{
    if (var == 0)
    {
        // switch green LED on,
        digitalWrite(, HIGH); // turn the LED on (HIGH is the voltage level)
        delay(1000);          // wait for a second
    }
    else if (var == 1)
    {
        // switch green LED off,
        digitalWrite(, LOW); // turn the LED off (LOW is the voltage level)
        delay(1000);         // wait for a second
    }
}
void switch_red_led(bool var)
{
    if (var == 0)
    {
        // switch red LED on,
        digitalWrite(, HIGH); // turn the LED on (HIGH is the voltage level)
        delay(1000);          // wait for a second
    }
    else if (var == 1)
    {
        // switch red LED off,
        digitalWrite(, LOW); // turn the LED off (LOW is the voltage level)
        delay(1000);         // wait for a second
    }
}

void switch_LED_BUILTIN(bool built_in_led)
{
    if (built_in_led == 1)
    {
        // switch it on
        digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
        delay(1000);                     // wait for a second
    }
    else if (built_in_led == 0)
    {
        digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
        delay(1000);                    // wait for a second
    }
}
void read_photoresistor()
{
    // reads resistance of a highpin to another that reads, gives out day or night, boolean, islight true or false
}
void switch_IR_camera(bool IR_camera)
{
    if (IR_camera == 1)
    {
        // switch it on
        digitalWrite(, HIGH); // turn the LED on (HIGH is the voltage level)
        delay(1000);          // wait for a second
    }
    else if (IR_camera == 0)
    {
        digitalWrite(, LOW); // turn the LED off by making the voltage LOW
        delay(1000);         // wait for a second
    }
}
/************************Barometer functions**********************************************
Read absolute altitude from air pressure, read surface chip temperature, absolute altitude lock */
void read_barometer()
{
}
void read_barometer_temperature()
{
}

/*Magnetometer measures the earth's magnetic field, method of constructing a digital compass
greedy heading
*/
void read_magnetometer()
{ // gives out greedy heading,
}
void switch_magnetometer(bool var)
{
    if (var == 1)
    {
        // Turn the transistor on
        digitalWrite(datalogger_transistor_pin, HIGH);
    }
    else if (var == 0)
    {
        // Function to switch the module off
        // Turn the transistor off
        digitalWrite(datalogger_transistor_pin, LOW);
    }
}
void switch_datalogger_on(bool var)
{
}
/*Camera and VTX*/
/*Radio RX connect to the
blizzard, the read pin is under interrupt set by PPM library*/
//~~~~~~~~~~~~~~~~~~~~RADIO functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Function to switch the radio on
void switch_radio_on()
{
    // Check if the radio is currently off
    if (!is_radio_on)
    {
        // Turn the transistor on
        digitalWrite(radio_transistor_pin, HIGH);
        // Update the variable to indicate that the radio is on
        is_radio_on = true;
    }
}
void switch_radio_off()
{ // Function to switch the radio off
    // Check if the radio is currently on
    if (is_radio_on)
    {
        // Turn the transistor off
        digitalWrite(radio_t_PIN, LOW);
        // Update the variable to indicate that the radio is off
        is_radio_on = false;
    }
}
void read_radio_allchannels() // reads RADIO_pin behind an interrupt
{
    while (is_radio_on)
    {
        // Interval at which the PPM is updated
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        { // interval is 50
            previousMillis = currentMillis;

            // Acquiring all the channels values
            throttle = ppm.read_channel(THROTTLE);
            roll = ppm.read_channel(ROLL);
            pitch = ppm.read_channel(PITCH);
            yaw = ppm.read_channel(YAW);
            switch3way_1 = ppm.read_channel(SWITCH3WAY_1);
            pot1 = ppm.read_channel(POT1);
            switch3way_2 = ppm.read_channel(SWITCH3WAY_2);
            pot2 = ppm.read_channel(POT2);

            // Print the values for the Arduino Serial Plotter
            // Serial.print("Throttle:");        Serial.print(throttle);       Serial.print(" ");
            // Serial.print("Roll:");            Serial.print(roll);           Serial.print(" ");
            // Serial.print("Pitch:");           Serial.print(pitch);          Serial.print(" ");
            // Serial.print("Yaw:");             Serial.print(yaw);            Serial.print(" ");
            // Serial.print("Switch_3way_1:");   Serial.print(switch3way_1);   Serial.print(" ");
            // Serial.print("Button:");          Serial.print(pot1);         Serial.print(" ");
            // Serial.print("Switch_3way_2:");   Serial.print(switch3way_2);   Serial.print(" ");
            // Serial.print("Pot:");             Serial.print(pot2);            Serial.print(" ");
            // Serial.println();
        }
    }
}
int read_throttle()
{
    while (is_radio_on)
    {
        // Interval at which the PPM is updated
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        { // interval is 50
            previousMillis = currentMillis;
            return ppm.read_channel(THROTTLE);
        }
    }
}
//***************************ESC functions***************************
// Function to setup motors
void setupMotors()
{
    initializeMotorPins();
}

// Function to update motor speeds based on PID output
void updateMotorSpeeds(rollOutput, pitchOutput, yawOutput)
{
    motor1Speed = baseSpeed + rollOutput - pitchOutput + yawOutput;
    motor2Speed = baseSpeed - rollOutput - pitchOutput - yawOutput;
    motor3Speed = baseSpeed + rollOutput + pitchOutput - yawOutput;
    motor4Speed = baseSpeed - rollOutput + pitchOutput + yawOutput;

    setMotorSpeed(motor1, motor1Speed);
    setMotorSpeed(motor2, motor2Speed);
    setMotorSpeed(motor3, motor3Speed);
    setMotorSpeed(motor4, motor4Speed);
}
void esc_output()
{
    // All the information for controlling the motor's is available.
    // The refresh rate is 250Hz. That means the esc's need there pulse every 4ms.
    while (micros() - loop_timer < 4000) // We wait until 4000us are passed.
        loop_timer = micros();           // makes sure refresh rate always 250Hz. Set the timer for the next loop.
    // PORTD |= B11110000;//4,5,6,7 ESCS HIGH
    PORTH |= B00011000;                   // set pin 6, 7 high(//Set digital poort 4, 5, 6 and 7 high.)
    PORTE |= B00001000;                   // Pin5
    PORTG |= B00100000;                   // Pin4
    timer_channel_1 = esc_1 + loop_timer; // Calculate the time of the falling edge of the esc-1 pulse.
    timer_channel_2 = esc_2 + loop_timer; // Calculate the time of the falling edge of the esc-2 pulse.
    timer_channel_3 = esc_3 + loop_timer; // Calculate the time of the falling edge of the esc-3 pulse.
    timer_channel_4 = esc_4 + loop_timer; // Calculate the time of the falling edge of the esc-4 pulse.

    while ((PORTH & B00001000) || (PORTE & B00001000) || (PORTG & B00100000) || (PORTH & B00010000))
    {
        // Stay in this loop until output 4,5,6 and 7 are low.
        esc_loop_timer = micros(); // Read the current time.
        // PORTD for UNO =4,5,6,7 ESCS basically, set them LOW
        if (timer_channel_1 <= esc_loop_timer)
            PORTG &= B11011111; // When the delay time is expired, digital port 4 is set low.
        if (timer_channel_2 <= esc_loop_timer)
            PORTE &= B11110111; // When the delay time is expired, digital port 5 is set low.
        if (timer_channel_3 <= esc_loop_timer)
            PORTH &= B11110111; // When the delay time is expired, digital port 6 is set low.
        if (timer_channel_4 <= esc_loop_timer)
            PORTH &= B11101111; // When the delay time is expired, digital port 7 is set low.
    }
}
void calculate_esc_pulse()
{
    if (primary_armed == true)
    { // if armed, The motors are started.
        // read_radio_allchannels();
        // Step Four calculate pulse for every ESC,
        read_throttle(); // read throttle, instead of reading every channel
        if (throttle > 1800)
        { //  We need the throttle signal as a base signal.
            // We need some room to keep full control at full throttle.
            throttle = 1800;
        }
        esc_1 = throttle - pid_output_pitch + pid_output_roll - pid_output_yaw; // Calculate the pulse for esc 1 (front-right - CCW)
        esc_2 = throttle + pid_output_pitch + pid_output_roll + pid_output_yaw; // Calculate the pulse for esc 2 (rear-right - CW)
        esc_3 = throttle + pid_output_pitch - pid_output_roll - pid_output_yaw; // Calculate the pulse for esc 3 (rear-left - CCW)
        esc_4 = throttle - pid_output_pitch - pid_output_roll + pid_output_yaw; // Calculate the pulse for esc 4 (front-left - CW)
        // Step Five, compensate ESCs for voltage drop //multiply by 100
        compensate_esc_for_voltage();
        if (esc_1 < 1100)
            esc_1 = 1050; // Keep the motors running.
        if (esc_2 < 1100)
            esc_2 = 1050; // Keep the motors running.
        if (esc_3 < 1100)
            esc_3 = 1050; // Keep the motors running.//all were @1200 at one point
        if (esc_4 < 1100)
            esc_4 = 1050; // Keep the motors running.

        if (esc_1 > 2000)
            esc_1 = 2000; // Limit the esc-1 pulse to 2000us.
        if (esc_2 > 2000)
            esc_2 = 2000; // Limit the esc-2 pulse to 2000us.
        if (esc_3 > 2000)
            esc_3 = 2000; // Limit the esc-3 pulse to 2000us.
        if (esc_4 > 2000)
            esc_4 = 2000; // Limit the esc-4 pulse to 2000us.
    }
    else if (primary_armed == false)
    {                 // ESCs disarmed give 1000ms pulse
        esc_1 = 1000; // If start is not 2 keep a 1000us pulse for esc-1.
        esc_2 = 1000; // If start is not 2 keep a 1000us pulse for esc-2.
        esc_3 = 1000; // If start is not 2 keep a 1000us pulse for esc-3.
        esc_4 = 1000; // If start is not 2 keep a 1000us pulse for esc-4.
    }
}
void calibrate_escs()
{
    // code to calibrate the electronic speed controllers
}
void compensate_esc_for_voltage() //~from powermanagement
{
    if (battery_voltage < 1260 && battery_voltage > 1100)
    {                                                              // Is the battery connected?
        esc_1 += esc_1 * ((1240 - battery_voltage) / (float)3000); // Compensate the esc-1 pulse for voltage drop.
        esc_2 += esc_2 * ((1240 - battery_voltage) / (float)3000); // Compensate the esc-2 pulse for voltage drop.
        esc_3 += esc_3 * ((1240 - battery_voltage) / (float)3000); // Compensate the esc-3 pulse for voltage drop.
        esc_4 += esc_4 * ((1240 - battery_voltage) / (float)3000); // Compensate the esc-4 pulse for voltage drop.
    } // increase if drone rises
}

//***************FLight management and navigation section*****************
// code that performs flight found here,

void toggle_arming_sequence()
{
    // Check primary arming status from radio receiver channel 5 (switch3way_1)
    // There are two situations,
    if (switch3way_1 > 1500 && !debouncePressed)
    {
        // Arm the drone
        start = 2;              // Start the motors
        primary_armed = true;   // Set primary armed to true
        secondary_armed = true; // Set secondary armed to true

        // Check secondary arming/disarming button state
        if (digitalRead(onboard_disarm_switch_value) == HIGH && primary_armed)
            P
            {
                // Disarm if button is pressed and primary armed is on
                secondary_armed = false;
                primary_armed = false;
                debouncePressed = true; // Set debouncePressed to true when the button is pressed
            }
        else if (debouncePressed && digitalRead(onboard_disarm_switch_value) == LOW)
        {
            // Reset debouncePressed if the button is released (bounces back) and goes back to LOW
            debouncePressed = true; // keep this variable true, until reset by primary switch
            primary_armed = false;
            secondary_armed = false;
        }

        // Reset PID controllers for a bumpless start when arming the quadcopter
        pid_i_mem_roll = 0;
        pid_last_roll_d_error = 0;
        pid_i_mem_pitch = 0;
        pid_last_pitch_d_error = 0;
        pid_i_mem_yaw = 0;
        pid_last_yaw_d_error = 0;

        // Determine the flight mode based on the position of switch3way_2
        if (switch3way_2 < 1500)
        {
            // Acrobatic mode
            acro_mode = true;
            auto_level = false;
        }
        else if (switch3way_2 > 1500)
        {
            // Self-leveling mode
            auto_level = true;
            acro_mode = false;
        }
    }
    else if (switch3way_1 > 1500 && debouncePressed)
    {
        // Disarm the drone only if the debounce switch is not pressed
        switch3way_2 = 0;        // Reset switch3way_2
        start = 0;               // Stop the motors
        primary_armed = false;   // Reset primary armed
        secondary_armed = false; // Reset secondary armed
    }
    else if (switch3way_1 < 1500 && (!debouncePressed || debouncePressed))
    {
    }
}

// Function to initialize flight modes
void initializeFlightModes()
{                            // switchA
    currentMode = STABILIZE; // this is disarming sequence
}
// Function to handle flight mode change
void changeFlightMode(newMode)
{ // switchB
    currentMode = newMode configureModeParameters(newMode);
}
// Function to configure parameters based on flight mode
void configureModeParameters(mode)
{
    switch
    mode:
    case STABILIZE:
    configureStabilizeMode() case ALT_HOLD:
    configureAltHoldMode() case LOITER:
        configureLoiterMode();
    // Add other modes as needed
}
void calculate_flight_orientation()
{
    gyro_roll_input = (gyro_roll_input * 0.8) + ((gyro_x) * 0.2);   // Gyro pid input is deg/sec.
    gyro_pitch_input = (gyro_pitch_input * 0.8) + ((gyro_y) * 0.2); // *confirm accuracy of complimentary filter
    gyro_yaw_input = (gyro_yaw_input * 0.8) + ((gyro_z) * 0.2);
    // gyro_roll_ input is travelled roll, over time
    // gyro_roll_input goes into calculate_PID(); then outputs come out
    // I dont need to put all the code in these statements, I can change variables
    // this will take drone positional angle (from gravitational vector) vs pid setpoint,
    /* code */ // Gyro angle calculations
    // 0.0000611 = 1 / (250Hz / 65.5)
    angle_pitch += gyro_y_raw * 0.0000611; // Calculate the traveled pitch angle and add this to the angle_pitch variable
    angle_roll += gyro_x_raw * 0.0000611;  // Calculate the traveled roll angle and add this to the angle_roll variable
    // angle_pitch is a collection of gryoX(travelled_pitch_angle) over time
    // 0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
    angle_pitch += angle_roll * sin(gyro_z_raw * 0.000001066); // If the IMU has yawed transfer the roll angle to the pitch angle
    angle_roll -= angle_pitch * sin(gyro_z_raw * 0.000001066); // If the IMU has yawed transfer the pitch angle to the roll angle
    // the above is in radians. Take note
    // Accelerometer angle calculations
    // Corrected code
    acc_total_vector = sqrt((acc_x_raw * acc_x_raw) + (acc_y_raw * acc_y_raw) + (acc_z_raw * acc_z_raw)); // Calculate the total accelerometer vector

    if (acc_total_vector != 0)
    {                                                                         // 57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
        angle_pitch_acc = asin((float)acc_x_raw / acc_total_vector) * 57.296; // Calculate the pitch angle
        angle_roll_acc = asin((float)acc_y_raw / acc_total_vector) * -57.296; // Calculate the roll angle
    }
    // multiplied to get answer in degrees, asin function gives answer in radians
    // Place the MPU-6050 spirit level and note the values in the following two lines for calibration
    angle_pitch_acc -= 1.8; // Accelerometer calibration value for pitch
    angle_roll_acc -= 0;    // Accelerometer calibration value for roll
    if (set_gyro_angles)
    {                                                              // If the IMU is already started
        angle_pitch = angle_pitch * 0.91 + angle_pitch_acc * 0.09; // Correct the drift of the gyro pitch angle with the accelerometer pitch angle
        angle_roll = angle_roll * 0.91 + angle_roll_acc * 0.09;    // Correct the drift of the gyro roll angle with the accelerometer roll angle
    }
    else
    {                                  // At first start
        angle_pitch = angle_pitch_acc; // Set the gyro pitch angle equal to the accelerometer pitch angle
        angle_roll = angle_roll_acc;   // Set the gyro roll angle equal to the accelerometer roll angle
        set_gyro_angles = true;        // Set the IMU started flag
    }
    pitch_level_adjust = angle_pitch * 15; // Calculate the pitch angle correction
    roll_level_adjust = angle_roll * 15;   // Calculate the roll angle correction
                                           // If the quadcopter is not in auto-level mode, reset pitch and roll angle corrections
    if (!auto_level)
    {
        pitch_level_adjust = 0; // Set the pitch angle correction to zero
        roll_level_adjust = 0;  // Set the roll angle correction to zero
                                // Serial.print("not auto level");
    }
    // To dampen the pitch and roll angles a complementary filter is used
    // angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
    // angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1;      //Take 90% of the output roll value and add 10% of the raw roll value
    /*Angle pitch is angle pitch from upright(gravitational vector)*/
}

// Function to perform sensor fusion and calculate orientation
void calculateOrientation(imuData)
{
    orientation = fuseIMUData(imuData);
    return orientation;
}
// Function to compute GPS-based navigation commands
void calculateNavigationCommand(gpsData)
{
    navigationCommand = computeNavigation(gpsData);
    return navigationCommand;
}

/*To write a function that calculates the distance and heading from one set of GPS coordinates to another, we'll use the Haversine formula for distance and basic trigonometry for heading. The input coordinates will be in the specified formats, so we'll need to parse and convert them into decimal degrees.

### Haversine Formula for Distance
The Haversine formula calculates the distance between two points on the Earth given their latitude and longitude in decimal degrees.

### Formula for Heading
The heading or bearing from one point to another can be calculated using the following formula:
\[ \theta = \text{atan2}( \sin(\Delta \lambda) \cdot \cos(\phi_2), \cos(\phi_1) \cdot \sin(\phi_2) - \sin(\phi_1) \cdot \cos(\phi_2) \cdot \cos(\Delta \lambda) ) \]

Here is the function in Arduino code:

```cpp*/

// SHOULd this be on the blizzard?
//  Converts coordinates from DDMM.MMM and DDDMM.MMM format to decimal degrees
double convertToDecimalDegrees(int degrees, double minutes, char direction) // from GPS functions
{
    double decimalDegrees = degrees + (minutes / 60.0);
    if (direction == 'S' || direction == 'W')
    {
        decimalDegrees = -decimalDegrees;
    }
    return decimalDegrees;
}

// Function to calculate the distance between two GPS coordinates using the Haversine formula
double calculateDistance(double lat1, double lon1, double lat2, double lon2)
{
    const double R = 6371000; // Earth's radius in meters
    double lat1Rad = radians(lat1);
    double lat2Rad = radians(lat2);
    double deltaLat = radians(lat2 - lat1);
    double deltaLon = radians(lon2 - lon1);

    double a = sin(deltaLat / 2) * sin(deltaLat / 2) +
               cos(lat1Rad) * cos(lat2Rad) *
                   sin(deltaLon / 2) * sin(deltaLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    double distance = R * c;
    return distance;
}

// Function to calculate the heading from one GPS coordinate to another
double calculateHeading(double lat1, double lon1, double lat2, double lon2)
{
    double lat1Rad = radians(lat1);
    double lat2Rad = radians(lat2);
    double deltaLon = radians(lon2 - lon1);

    double y = sin(deltaLon) * cos(lat2Rad);
    double x = cos(lat1Rad) * sin(lat2Rad) -
               sin(lat1Rad) * cos(lat2Rad) * cos(deltaLon);
    double heading = atan2(y, x);
    heading = degrees(heading);
    heading = fmod((heading + 360), 360); // Normalize to 0-360 degrees
    return heading;
}

// Main function to read GPS coordinates, calculate distance and heading
void calculateDistanceAndHeading(
    int lat1Degrees, double lat1Minutes, char lat1Direction,
    int lon1Degrees, double lon1Minutes, char lon1Direction,
    int lat2Degrees, double lat2Minutes, char lat2Direction,
    int lon2Degrees, double lon2Minutes, char lon2Direction)
{

    // Convert input coordinates to decimal degrees
    double lat1 = convertToDecimalDegrees(lat1Degrees, lat1Minutes, lat1Direction);
    double lon1 = convertToDecimalDegrees(lon1Degrees, lon1Minutes, lon1Direction);
    double lat2 = convertToDecimalDegrees(lat2Degrees, lat2Minutes, lat2Direction);
    double lon2 = convertToDecimalDegrees(lon2Degrees, lon2Minutes, lon2Direction);

    // Calculate distance and heading
    double distance = calculateDistance(lat1, lon1, lat2, lon2);
    double heading = calculateHeading(lat1, lon1, lat2, lon2);

    // Print results
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" meters");
    Serial.print("Heading: ");
    Serial.print(heading);
    Serial.println(" degrees");
}

/*******************Battery management section**************************************
 * it has the parts that manage the power, not compensation in software
Read voltage via divider, code to convert true voltage, compensate, for ESCs,
An actual BMS that accounts for charging and discharging, give max and min discharging and charging currents,
cut off charge and discharge if exceeds bounds
Onboard voltage regulator,
*/
// POwer management functions
void read_battery_voltage()
{ // calculate battery capacity via voltage divider on pin A0(BATT_pin)
    // Load the battery voltage to the batt_anlog_val variable.
    // 65 is the voltage compensation for the diode.
    // 12.6V equals ~5.04V @ Analog 0.
    // 12.6V equals 1023 analogRead(0).
    // 1260 / 1023 = 1.2317.
    // 0.09853 = 0.08 * 1.2317.
    // The variable battery_voltage holds 1050 if the battery voltage is 10.5V.
    // battery_voltage = (analogRead(0) + 65) * 1.2317;

    // read_analog_pin();
    batt_anlog_val = analogRead(battery_voltdiv_pin); // 0-5v after ADC conversion 0-1024
}
void calculate_battery_capacity()
{
    // batt_anlog_val = batt_anlog_val * 0.92 + analogRead(battery_voltdiv_pin) * 0.09853;
    //  A complementary filter is used to reduce noise.
    batt_true_val = (map(batt_anlog_val, 0, 1023, 0, 5)) * 2.5; // volt divider rescaling
    battery_voltage = batt_true_val * 100;                      // 1250-1150
    battery_capacity = map(batt_true_val, 11.2, 12.6, 20, 100); // 20-100
    if (battery_voltage < low_battery_voltage)
    { // send warning, peripheral LEDS, and blizzard
      // turn on onboard buzzers,
    }
}
void adaptive_power_management()
{
    // turn arduino on  power saving mode, other modules too,
    // maximum power mode, the drone can allow spurts of energy draws to liftoff, accelerate,
    // the arduino can turn other modules to low power modes, like gps and gyroscope, goes low, comes up to read and validate data,
}
/*Electronic speed controllers,
control via PWM signal,
Calibration, all ESCs under same timer,
reverse rotation via H-bridge for turtle mode,
Design prop guards,
### Explanation:
1. **Conversion to Decimal Degrees**: The `convertToDecimalDegrees` function converts the DDMM.MMM and DDDMM.MMM formats to decimal degrees.
2. **Distance Calculation**: The `calculateDistance` function uses the Haversine formula to calculate the distance between two points in meters.
3. **Heading Calculation**: The `calculateHeading` function calculates the bearing between two points using trigonometric functions.
4. **Main Function**: The `calculateDistanceAndHeading` function reads the coordinates, converts them, and then calculates the distance and heading, printing the results to the serial monitor.

This approach ensures the coordinates are correctly interpreted and used to calculate the required values.*/

//~~~~~~~~~~~~~~~~~~~PID functions~~~~~~~~~~~~~~~~~~~~~~~~
void calculate_pid_setpoint()
{
    read_radio_allchannels();
    // Acro pid setpoint
    // this part of the code transfers receiver inputs to deg/sec,
    // The PID set point in degrees per second is determined by the roll receiver input.
    // In the case of dividing by 3 the max roll rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
    // Reduce for faster roll rate
    pid_roll_setpoint = 0;
    // We need a little dead band of 16us for better results.
    if (roll > 1508)
        pid_roll_setpoint = roll - 1508;
    else if (roll < 1492)
        pid_roll_setpoint = roll - 1492;    // Reduce the three for faster roll rates
    pid_roll_setpoint -= roll_level_adjust; // Subtract the angle correction from the standardized receiver roll input value.
    pid_roll_setpoint /= 3.0;               // Divide the setpoint for the PID roll controller by 3 to get angles in degrees.

    // The PID set point in degrees per second is determined by the pitch receiver input.
    // In the case of deviding by 3 the max pitch rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
    pid_pitch_setpoint = 0;
    // We need a little dead band of 16us for better results.
    if (pitch > 1508)
        pid_pitch_setpoint = pitch - 1508; // 1853=115deg/sec
    else if (pitch < 1492)
        pid_pitch_setpoint = pitch - 1492;
    pid_pitch_setpoint -= pitch_level_adjust; // Subtract the angle correction from the standardized receiver pitch input value.
    pid_pitch_setpoint /= 3.0;                // Divide the setpoint for the PID pitch controller by 3 to get angles in degrees.

    // The PID set point in degrees per second is determined by the yaw receiver input.
    // In the case of deviding by 3 the max yaw rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
    pid_yaw_setpoint = 0;
    // We need a little dead band of 16us for better results.
    // Do not yaw when turning off the motors.
    if (yaw > 1508)
        pid_yaw_setpoint = (yaw - 1508) / 3.0;
    else if (yaw < 1492)
        pid_yaw_setpoint = (yaw - 1492) / 3.0;
    // PID inputs are known. So we can calculate the pid output.
}
void calculate_pid()
{
    // Roll calculations
    // e.g at max pitch(2000) pid_roll_setpoint(radio_input in deg/sec) will be 164deg
    // read_gyro gives gyro_x in deg/sec
    pid_error_temp = angle_pitch - pid_roll_setpoint;   // error between gyro and receiver is calculated e.g. [+0deg/s-164deg/s]
    pid_i_mem_roll += pid_i_gain_roll * pid_error_temp; // multiplies error with i_gain adds it to pid_i_mem_roll [0.08*164]add
    if (pid_i_mem_roll > pid_max_roll)
        pid_i_mem_roll = pid_max_roll; // limit output, prevent i controller from going out of control
    else if (pid_i_mem_roll < pid_max_roll * -1)
        pid_i_mem_roll = pid_max_roll * -1;
    // complete PID  output is calculated, pgain multiplied by error, dgain multiplied by difference between current error and previous error
    pid_output_roll = pid_p_gain_roll * pid_error_temp + pid_i_mem_roll + pid_d_gain_roll * (pid_error_temp - pid_last_roll_d_error);
    if (pid_output_roll > pid_max_roll)
        pid_output_roll = pid_max_roll; // limit controller, p and d controllers might cause extreme outputs
    else if (pid_output_roll < pid_max_roll * -1)
        pid_output_roll = pid_max_roll * -1;

    pid_last_roll_d_error = pid_error_temp; // current error saved for next calculation

    // Pitch calculations
    pid_error_temp = gyro_pitch_input - pid_pitch_setpoint;
    pid_i_mem_pitch += pid_i_gain_pitch * pid_error_temp;
    if (pid_i_mem_pitch > pid_max_pitch)
        pid_i_mem_pitch = pid_max_pitch;
    else if (pid_i_mem_pitch < pid_max_pitch * -1)
        pid_i_mem_pitch = pid_max_pitch * -1;

    pid_output_pitch = pid_p_gain_pitch * pid_error_temp + pid_i_mem_pitch + pid_d_gain_pitch * (pid_error_temp - pid_last_pitch_d_error);
    if (pid_output_pitch > pid_max_pitch)
        pid_output_pitch = pid_max_pitch;
    else if (pid_output_pitch < pid_max_pitch * -1)
        pid_output_pitch = pid_max_pitch * -1;

    pid_last_pitch_d_error = pid_error_temp;

    // Yaw calculations
    pid_error_temp = gyro_yaw_input - pid_yaw_setpoint;
    pid_i_mem_yaw += pid_i_gain_yaw * pid_error_temp;
    if (pid_i_mem_yaw > pid_max_yaw)
        pid_i_mem_yaw = pid_max_yaw;
    else if (pid_i_mem_yaw < pid_max_yaw * -1)
        pid_i_mem_yaw = pid_max_yaw * -1;

    pid_output_yaw = pid_p_gain_yaw * pid_error_temp + pid_i_mem_yaw + pid_d_gain_yaw * (pid_error_temp - pid_last_yaw_d_error);
    if (pid_output_yaw > pid_max_yaw)
        pid_output_yaw = pid_max_yaw;
    else if (pid_output_yaw < pid_max_yaw * -1)
        pid_output_yaw = pid_max_yaw * -1;

    pid_last_yaw_d_error = pid_error_temp;
}
// Function to setup PID controllers
void setupPIDControllers()
{
    rollPID = initializePID(rollKp, rollKi, rollKd);
    pitchPID = initializePID(pitchKp, pitchKi, pitchKd);
    yawPID = initializePID(yawKp, yawKi, yawKd);
}
// Function to compute PID output for roll, pitch, and yaw
void computePIDOutput(imuData, setpoints)
{
    rollOutput = computePID(rollPID, setpoints.roll, imuData.roll);
    pitchOutput = computePID(pitchPID, setpoints.pitch, imuData.pitch);
    yawOutput = computePID(yawPID, setpoints.yaw, imuData.yaw);
    return (rollOutput, pitchOutput, yawOutput);
}
//~~~~~~~~~~~~~~~~~~~~~~~Transistor functions~~~~~~~~~~~~~~~~~~~~~
/*THis is code to switch modules and components on and off
via physical transistors and digital pins onboard the arduino mega*                                                                                                                                     void EEPROMclear()
{
/***
Iterate through each byte of the EEPROM storage.
Larger AVR processors have larger EEPROM sizes, E.g:
- Arduino Duemilanove: 512 B EEPROM storage.
- Arduino Uno:         1 kB EEPROM storage.
- Arduino Mega:        4 kB EEPROM storage.
Rather than hard-coding the length, you should use the pre-provided length function.
This will make your code portable to all AVR processors.
***/
/****************EEPROM functions********************************************/
void change_access_password()
{
    // on promptchange access password in EEPROM, after receiving admin prompt from blizzard,
}
void store_home_coordinates()
{
    // the drone takes many gps samples, calculates a mean sets that as home location, such that it can always return home, sometimes sets this to EEPROM,
    // due to flight priority settings, or to change batteries, long flights
}
void change_name()
{
    // on admmin prompt, change the drone name on EEPROM,
}
// EEPROM.get
// EEPROM.update
// EEPROM.read
// EEPROM[]
// EEPROM.write

//************distributed processing section********************************
// Arduino nano functions
void nanoA()
{ // the mega will communicate with two peripheral nanos, to allow simultaneous programming,
    // Example: Command to Nano with address 8 to read sensor data
    Wire.beginTransmission(8); // Address of the first Nano
    Wire.write("readSensor");  // Command to read sensor
    Wire.endTransmission();

    delay(1000); // Wait for a while before sending the next command
}
void nanoB()
{
    // Example: Command to Nano with address 9 to control motor
    Wire.beginTransmission(9);  // Address of the second Nano
    Wire.write("controlMotor"); // Command to control motor
    Wire.endTransmission();

    delay(1000); // Wait for a while before sending the next command
}
//**********************Test mode section****************************
// THis will aim to test the hardware and software for faults, either in_ or out_ of flight
/*perform VEC tests, generate crash reports by the drone, etc */
void toggleTestMode()
{ // full tests, vec, specific module tests
    testMode = !testMode;
    if (testMode)
    {
        Serial.println("Test mode activated");
    }
    else
    {
        Serial.println("Test mode deactivated");
    }
}

// Example test function
void runTests()
{
    Serial.println("Running tests...");
    testIMU();
    testGPS();
    testMotors();
    // Add more tests as needed
}

// Function to test IMU
void testIMU()
{
    bool imuStatus = initializeIMU();
    if (imuStatus)
    {
        Serial.println("IMU test passed");
    }
    else
    {
        Serial.println("IMU test failed");
    }
    sendTestResult("IMU", imuStatus);
}

// Function to test GPS
void testGPS()
{
    bool gpsStatus = initializeGPS();
    if (gpsStatus)
    {
        Serial.println("GPS test passed");
    }
    else
    {
        Serial.println("GPS test failed");
    }
    sendTestResult("GPS", gpsStatus);
}

// Function to test motors
void testMotors()
{
    bool motorStatus = initializeMotors();
    if (motorStatus)
    {
        Serial.println("Motors test passed");
    }
    else
    {
        Serial.println("Motors test failed");
    }
    sendTestResult("Motors", motorStatus);
}

// Function to send test results over HC12
void sendTestResult(const String &testName, bool result)
{
    String message = testName + " test " + (result ? "passed" : "failed");
    mySerial.println(message);
    Serial.println("Test result sent: " + message);
}

// Function to initialize flight modes
void initializeFlightModes()
{                           // switchA
    currentMode = STABILIZE // this is disarming sequence
}
// Function to handle flight mode change
void changeFlightMode(String newMode)
{ // switchB
    currentMode = newMode configureModeParameters(newMode)
}
// Function to configure parameters based on flight mode
void configureModeParameters(String mode)
{
    switch
    mode:
    case STABILIZE:
    configureStabilizeMode() case ALT_HOLD:
    configureAltHoldMode() case LOITER:
        configureLoiterMode() 2
    // Add other modes as needed
}
// BLIZZARD section
