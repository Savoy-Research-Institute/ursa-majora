#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#include <Arduino.h>

class pid_controller {
public:
    pid_controller();
    void setGains(float p_gain, float i_gain, float d_gain);
    void setLimits(float min_limit, float max_limit);
    void setSetpoint(float setpoint);
    float update_pid(float measurement);
    void reset();
    void calculate_pid_setpoint();
    void calculate_pid();

    // Getters for PID outputs
    float getRollOutput() const { return pid_output_roll; }
    float getPitchOutput() const { return pid_output_pitch; }
    float getYawOutput() const { return pid_output_yaw; }

private:
    float p_gain, i_gain, d_gain;
    float min_limit, max_limit;
    float setpoint;
    float last_error;
    float i_mem;
    float roll_pid, pitch_pid, yaw_pid;

    // Radio inputs
    float roll, pitch, yaw;

    // Level adjustments
    float roll_level_adjust, pitch_level_adjust;

    // Gyro inputs
    float gyro_roll_input, gyro_pitch_input, gyro_yaw_input;

    // PID setpoints and outputs
    float pid_roll_setpoint, pid_pitch_setpoint, pid_yaw_setpoint;
    float pid_output_roll, pid_output_pitch, pid_output_yaw;

    // Angle measurements
    float angle_pitch, angle_roll;
};

class flight_controller {
public:

    flight_controller();

    void update_flight();
    void toggle_arming_sequence();
    void calculate_flight_orientation();
    void set_flight_mode(bool auto_level_mode);
    void reset_pid_controllers();
    void calibrate_imu(float pitch_calibration, float roll_calibration);

    // Getters
    bool is_armed() const { return primary_armed && secondary_armed; }
    bool is_auto_level() const { return auto_level; }
    float get_angle_pitch() const { return angle_pitch; }
    float get_angle_roll() const { return angle_roll; }

    // Setters
    void set_switch_values(int switch3way_1_val, int switch3way_2_val);
    void set_gyro_values(float gyro_x, float gyro_y, float gyro_z);
    void set_acc_values(float acc_x, float acc_y, float acc_z);

private:

    pid_controller roll_pid, pitch_pid, yaw_pid;

    // Flight modes
    void acrobatic_mode();
    void self_leveling_mode();

    // Flight control variables
    float gyro_roll_input, gyro_pitch_input, gyro_yaw_input;
    float angle_pitch, angle_roll, pitch_level_adjust, roll_level_adjust;

    // Arming and mode variables
    bool primary_armed;
    bool secondary_armed;
    bool debouncePressed;
    bool auto_level;
    bool acro_mode;
    int start;
    bool onboard_disarm_switch_value;
    // Switch values
    int switch3way_1;
    int switch3way_2;

    // IMU variables
    float angle_pitch_acc, angle_roll_acc;
    float acc_total_vector;
    float gyro_x_raw, gyro_y_raw, gyro_z_raw;
    float acc_x_raw, acc_y_raw, acc_z_raw;
    bool set_gyro_angles;

    // Constants
    static constexpr float GYRO_SENSITIVITY = 0.0000611f; // 1 / (250Hz / 65.5)
    static constexpr float RAD_TO_DEG = 57.296f;          // 180 / PI
    static constexpr float COMPLEMENTARY_FILTER_COEFF = 0.91f;
};

#endif // FLIGHT_CONTROLLER_H