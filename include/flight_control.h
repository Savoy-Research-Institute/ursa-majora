#ifndef FLIGHT_CONTROL_H
#define FLIGHT_CONTROL_H

// Declare external variables (these should be defined elsewhere in your project)
extern int roll, pitch, yaw;
extern float roll_level_adjust, pitch_level_adjust;
extern float angle_pitch, gyro_pitch_input, gyro_yaw_input;

// PID setpoints and outputs
extern float pid_roll_setpoint, pid_pitch_setpoint, pid_yaw_setpoint;
extern float pid_output_roll, pid_output_pitch, pid_output_yaw;

// PID memory and gains
extern float pid_i_mem_roll, pid_i_mem_pitch, pid_i_mem_yaw;
extern float pid_last_roll_d_error, pid_last_pitch_d_error, pid_last_yaw_d_error;
extern float pid_p_gain_roll, pid_i_gain_roll, pid_d_gain_roll, pid_max_roll;
extern float pid_p_gain_pitch, pid_i_gain_pitch, pid_d_gain_pitch, pid_max_pitch;
extern float pid_p_gain_yaw, pid_i_gain_yaw, pid_d_gain_yaw, pid_max_yaw;

// Function declarations
void calculate_pid_setpoint();
void calculate_pid();
void read_radio_allchannels();

#endif // FLIGHT_CONTROL_H