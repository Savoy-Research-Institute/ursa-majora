//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//Objectives, manual control code, acrobatic and self leveling code, altitude and geo locks, 
/*works in conjunction with PID controller library for acrobatic and self-leveling code, 

*/
//BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
//this file will enable the software to work with different UAS quadcopter or glider etc, it holds instructions to fly, 

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

void acro_flight()
{
    // when toggled, the drone will do flight in acrobatic mode,
}
void self_leveling()
{
    // when toggled, the drone will do flight in self leveling mode,
}
/
void on_startup() // code runs once at void setup
{
    calibrate_cube(); // calculate all these errors, calibrate everything, once
    keep_local_clock();
    // wait for blizzard request, while waiting, if docked, keep ESCs silent, if not docked, good,
    password_interface(); // access_granted is true, the drone, that cube, red not connected, orange calibrating, green for ok,
    // and I can arm and disarm the drone if docked, the drone logs who is connected to it at any time into the sdcard,
    store_home_coordinates();

    /*
     */
}
// Function to read sensor data
void readSensorData()
{
    imuData = getIMUData()
        gpsData = getGPSData() return (imuData, gpsData)
}
// Function to initialize flight control system
void initializeFlightControl()
{
    setupIMU();
    setupGPS();
    setupMotors();
    setupPIDControllers();
    setupFailsafe();
    Serial.print("Flight control initialization complete");
}
// Function to initialize flight modes
void initializeFlightModes()
{                           // switchA
    currentMode = STABILIZE // this is disarming sequence
}
// Function to handle flight mode change
void changeFlightMode(newMode)
{ // switchB
    currentMode = newMode;
    configureModeParameters(newMode)
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