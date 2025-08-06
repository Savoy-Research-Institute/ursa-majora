#include "flight_controller.h"
#include <math.h>

flight_controller::flight_controller()
    : primary_armed(false), secondary_armed(false), debouncePressed(false),
      auto_level(false), acro_mode(false), start(0),
      switch3way_1(0), switch3way_2(0),
      angle_pitch_acc(0), angle_roll_acc(0),
      acc_total_vector(0),
      gyro_x_raw(0), gyro_y_raw(0), gyro_z_raw(0),
      acc_x_raw(0), acc_y_raw(0), acc_z_raw(0),
      set_gyro_angles(false)

void flight_controller::update_flight()
{
    calculate_flight_orientation();
    if (is_armed())
    {
        if (is_auto_level())
        {
            self_leveling_mode();
        }
        else
        {
            acrobatic_mode();
        }
    }
}
void flight_controller::toggle_arming_sequence() {
    static bool lastOnboardButtonState = LOW;
    bool currentOnboardButtonState = digitalRead(onboard_disarm_switch_value);

    // Arming from radio transmitter
    if (switch3way_1 > 1500) {
        if (!primary_armed && !secondary_armed) {
            start = 2;
            primary_armed = true;
            secondary_armed = true;
            reset_pid_controllers();
            set_flight_mode(switch3way_2 > 1500);
        }
    } else {
        // Disarming from radio transmitter
        start = 0;
        primary_armed = false;
        secondary_armed = false;
    }

    // Onboard button logic
    if (currentOnboardButtonState == HIGH && lastOnboardButtonState == LOW) {
        // Button press detected
        if (primary_armed) {
            secondary_armed = false;
        }
    }

    // Re-arming logic
    if (!secondary_armed && switch3way_1 <= 1500) {
        // Allow re-arming only after the radio transmitter has disarmed
        secondary_armed = true;
    }

    lastOnboardButtonState = currentOnboardButtonState;
}

void flight_controller::calculate_flight_orientation()
{//update this according to previous code...
    gyro_roll_input = (gyro_roll_input * 0.8) + ((gyro_x_raw) * 0.2);
    gyro_pitch_input = (gyro_pitch_input * 0.8) + ((gyro_y_raw) * 0.2);
    gyro_yaw_input = (gyro_yaw_input * 0.8) + ((gyro_z_raw) * 0.2);

    angle_pitch += gyro_y_raw * GYRO_SENSITIVITY;
    angle_roll += gyro_x_raw * GYRO_SENSITIVITY;

    angle_pitch += angle_roll * sin(gyro_z_raw * GYRO_SENSITIVITY * (PI / 180.0f));
    angle_roll -= angle_pitch * sin(gyro_z_raw * GYRO_SENSITIVITY * (PI / 180.0f));

    acc_total_vector = sqrt((acc_x_raw * acc_x_raw) + (acc_y_raw * acc_y_raw) + (acc_z_raw * acc_z_raw));

    if (acc_total_vector != 0)
    {
        angle_pitch_acc = asin((float)acc_x_raw / acc_total_vector) * RAD_TO_DEG;
        angle_roll_acc = asin((float)acc_y_raw / acc_total_vector) * -RAD_TO_DEG;
    }

    angle_pitch_acc -= 1.8; // Calibration value
    angle_roll_acc -= 0;    // Calibration value

    if (set_gyro_angles)
    {
        angle_pitch = angle_pitch * COMPLEMENTARY_FILTER_COEFF + angle_pitch_acc * (1.0f - COMPLEMENTARY_FILTER_COEFF);
        angle_roll = angle_roll * COMPLEMENTARY_FILTER_COEFF + angle_roll_acc * (1.0f - COMPLEMENTARY_FILTER_COEFF);
    }
    else
    {
        angle_pitch = angle_pitch_acc;
        angle_roll = angle_roll_acc;
        set_gyro_angles = true;
    }

    pitch_level_adjust = angle_pitch * 15;
    roll_level_adjust = angle_roll * 15;

    if (!auto_level)
    {
        pitch_level_adjust = 0;
        roll_level_adjust = 0;
    }
}

void flight_controller::set_flight_mode(bool auto_level_mode)
{
    auto_level = auto_level_mode;
    acro_mode = !auto_level_mode;
}

void flight_controller::reset_pid_controllers()
{
    roll_pid.reset();
    pitch_pid.reset();
    yaw_pid.reset();
}

void flight_controller::calibrate_imu(float pitch_calibration, float roll_calibration)
{
    angle_pitch_acc -= pitch_calibration;
    angle_roll_acc -= roll_calibration;
}

void flight_controller::set_switch_values(int switch3way_1_val, int switch3way_2_val)
{
    switch3way_1 = switch3way_1_val;
    switch3way_2 = switch3way_2_val;
}

void flight_controller::set_gyro_values(float gyro_x, float gyro_y, float gyro_z)
{
    gyro_x_raw = gyro_x;
    gyro_y_raw = gyro_y;
    gyro_z_raw = gyro_z;
}

void flight_controller::set_acc_values(float acc_x, float acc_y, float acc_z)
{
    acc_x_raw = acc_x;
    acc_y_raw = acc_y;
    acc_z_raw = acc_z;
}

void flight_controller::acrobatic_mode()
{
    // Implement acrobatic mode logic here
    // This might involve direct mapping of stick inputs to motor outputs
    // without any self-leveling correction
}

void flight_controller::self_leveling_mode()
{
    // Implement self-leveling mode logic here
    // This should use the PID controllers to maintain level flight
    float roll_correction = roll_pid.update_pid(roll_level_adjust);
    float pitch_correction = pitch_pid.update_pid(pitch_level_adjust);
    float yaw_correction = yaw_pid.update_pid(gyro_yaw_input);

    // Apply these corrections to your motor outputs
    // The exact implementation will depend on your motor configuration
}
pid_controller::pid_controller() : p_gain(0), i_gain(0), d_gain(0), min_limit(0), max_limit(0), setpoint(0), last_error(0), i_mem(0) {}

void pid_controller::setGains(float p_gain, float i_gain, float d_gain)
{
    this->p_gain = p_gain;
    this->i_gain = i_gain;
    this->d_gain = d_gain;
}

void pid_controller::setLimits(float min_limit, float max_limit)
{
    this->min_limit = min_limit;
    this->max_limit = max_limit;
}

void pid_controller::setSetpoint(float setpoint)
{
    this->setpoint = setpoint;
}

float pid_controller::update_pid(float measurement)
{
    float error = setpoint - measurement;
    i_mem += i_gain * error;
    i_mem = std::clamp(i_mem, min_limit, max_limit);

    float output = p_gain * error + i_mem + d_gain * (error - last_error);
    output = std::clamp(output, min_limit, max_limit);

    last_error = error;
    return output;
}

void pid_controller::reset()
{
    last_error = 0;
    i_mem = 0;
}

pid_controller::pid_controller()
    : roll(0), pitch(0), yaw(0),
      roll_level_adjust(0), pitch_level_adjust(0),
      gyro_roll_input(0), gyro_pitch_input(0), gyro_yaw_input(0),
      pid_roll_setpoint(0), pid_pitch_setpoint(0), pid_yaw_setpoint(0),
      pid_output_roll(0), pid_output_pitch(0), pid_output_yaw(0),
      angle_pitch(0), angle_roll(0), roll_pid(0), pitch_pid(0), yaw_pid(0)
{

    // Initialize PID controllers with default values
    roll_pid.setGains(1.0, 0.05, 0.5); // p_gain, i_gain, d_gain....
    pitch_pid.setGains(1.0, 0.05, 0.5);
    yaw_pid.setGains(1.0, 0.05, 0.5);

    roll_pid.setLimits(-400, 400); // min_limits, max_limits
    pitch_pid.setLimits(-400, 400);
    yaw_pid.setLimits(-400, 400);
}

void pid_controller::calculate_pid_setpoint()
{
    // readRadioAllChannels immediately before this function

    // Roll setpoint calculation
    pid_roll_setpoint = 0;
    if (roll > 1508)
        pid_roll_setpoint = roll - 1508;
    else if (roll < 1492)
        pid_roll_setpoint = roll - 1492;
    pid_roll_setpoint -= roll_level_adjust;
    pid_roll_setpoint /= 3.0;

    // Pitch setpoint calculation
    pid_pitch_setpoint = 0;
    if (pitch > 1508)
        pid_pitch_setpoint = pitch - 1508;
    else if (pitch < 1492)
        pid_pitch_setpoint = pitch - 1492;
    pid_pitch_setpoint -= pitch_level_adjust;
    pid_pitch_setpoint /= 3.0;

    // Yaw setpoint calculation
    pid_yaw_setpoint = 0;
    if (yaw > 1508)
        pid_yaw_setpoint = (yaw - 1508) / 3.0;
    else if (yaw < 1492)
        pid_yaw_setpoint = (yaw - 1492) / 3.0;

    // Set the calculated setpoints
    roll_pid.setSetpoint(pid_roll_setpoint);
    pitch_pid.setSetpoint(pid_pitch_setpoint);
    yaw_pid.setSetpoint(pid_yaw_setpoint);
}

void pid_controller::calculate_pid()
{
    pid_output_roll = roll_pid.update_pid(gyro_roll_input);
    pid_output_pitch = pitch_pid.update_pid(gyro_pitch_input);
    pid_output_yaw = yaw_pid.update_pid(gyro_yaw_input);
}

// void drone_controller::readRadioAllChannels() {
//     // Implement your radio reading logic here
//     // This function should update roll, pitch, and yaw values
// }