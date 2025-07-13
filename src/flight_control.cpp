#include "flight_control.h"

// Temporary variable for PID error calculation
static float pid_error_temp = 0;

void calculate_pid_setpoint()
{
    read_radio_allchannels();

    // Roll setpoint
    pid_roll_setpoint = 0;
    if (roll > 1508)
        pid_roll_setpoint = roll - 1508;
    else if (roll < 1492)
        pid_roll_setpoint = roll - 1492;
    pid_roll_setpoint -= roll_level_adjust;
    pid_roll_setpoint /= 3.0;

    // Pitch setpoint
    pid_pitch_setpoint = 0;
    if (pitch > 1508)
        pid_pitch_setpoint = pitch - 1508;
    else if (pitch < 1492)
        pid_pitch_setpoint = pitch - 1492;
    pid_pitch_setpoint -= pitch_level_adjust;
    pid_pitch_setpoint /= 3.0;

    // Yaw setpoint
    pid_yaw_setpoint = 0;
    if (yaw > 1508)
        pid_yaw_setpoint = (yaw - 1508) / 3.0;
    else if (yaw < 1492)
        pid_yaw_setpoint = (yaw - 1492) / 3.0;
}

void calculate_pid()
{
    // Roll PID
    pid_error_temp = angle_pitch - pid_roll_setpoint;
    pid_i_mem_roll += pid_i_gain_roll * pid_error_temp;
    if (pid_i_mem_roll > pid_max_roll)
        pid_i_mem_roll = pid_max_roll;
    else if (pid_i_mem_roll < -pid_max_roll)
        pid_i_mem_roll = -pid_max_roll;
    pid_output_roll = pid_p_gain_roll * pid_error_temp + pid_i_mem_roll +
                      pid_d_gain_roll * (pid_error_temp - pid_last_roll_d_error);
    if (pid_output_roll > pid_max_roll)
        pid_output_roll = pid_max_roll;
    else if (pid_output_roll < -pid_max_roll)
        pid_output_roll = -pid_max_roll;
    pid_last_roll_d_error = pid_error_temp;

    // Pitch PID
    pid_error_temp = gyro_pitch_input - pid_pitch_setpoint;
    pid_i_mem_pitch += pid_i_gain_pitch * pid_error_temp;
    if (pid_i_mem_pitch > pid_max_pitch)
        pid_i_mem_pitch = pid_max_pitch;
    else if (pid_i_mem_pitch < -pid_max_pitch)
        pid_i_mem_pitch = -pid_max_pitch;
    pid_output_pitch = pid_p_gain_pitch * pid_error_temp + pid_i_mem_pitch +
                       pid_d_gain_pitch * (pid_error_temp - pid_last_pitch_d_error);
    if (pid_output_pitch > pid_max_pitch)
        pid_output_pitch = pid_max_pitch;
    else if (pid_output_pitch < -pid_max_pitch)
        pid_output_pitch = -pid_max_pitch;
    pid_last_pitch_d_error = pid_error_temp;

    // Yaw PID
    pid_error_temp = gyro_yaw_input - pid_yaw_setpoint;
    pid_i_mem_yaw += pid_i_gain_yaw * pid_error_temp;
    if (pid_i_mem_yaw > pid_max_yaw)
        pid_i_mem_yaw = pid_max_yaw;
    else if (pid_i_mem_yaw < -pid_max_yaw)
        pid_i_mem_yaw = -pid_max_yaw;
    pid_output_yaw = pid_p_gain_yaw * pid_error_temp + pid_i_mem_yaw +
                     pid_d_gain_yaw * (pid_error_temp - pid_last_yaw_d_error);
    if (pid_output_yaw > pid_max_yaw)
        pid_output_yaw = pid_max_yaw;
    else if (pid_output_yaw < -pid_max_yaw)
        pid_output_yaw = -pid_max_yaw;
    pid_last_yaw_d_error =pid_error_temp;
}