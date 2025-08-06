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