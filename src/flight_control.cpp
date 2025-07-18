/**
 * @file flight_control.cpp
 * @brief Velma Flight Control System - Implementation
 * @author Velma Development Team
 * @version 1.0
 * @date 2025
 */

 #include "flight_control.h"
 #include <Arduino.h>
 #include <math.h>
 
 namespace Velma {
 namespace FlightControl {
 
 // ============================================================================
 // FLIGHT CONTROLLER IMPLEMENTATION
 // ============================================================================
 
 FlightController::FlightController() 
     : current_mode_(FlightMode::RATE_MODE)
     , arm_state_(ArmState::DISARMED)
     , system_status_(static_cast<uint16_t>(SystemStatus::NORMAL))
     , last_rc_update_(0)
     , last_imu_update_(0)
     , failsafe_active_(false)
 {
     // Initialize PID states with default values
     // Constructor initializes all members to safe defaults
 }
 
 FlightController::~FlightController() {
     // Ensure system is disarmed on destruction
     emergencyStop();
 }
 
 bool FlightController::initialize(const FlightControlConfig& config) {
     // Store configuration
     config_ = config;
     
     // Reset all PID controllers
     resetPIDControllers();
     
     // Set initial state
     current_mode_ = FlightMode::RATE_MODE;
     arm_state_ = ArmState::DISARMED;
     system_status_ = static_cast<uint16_t>(SystemStatus::NORMAL);
     failsafe_active_ = false;
     
     // Validate configuration parameters
     if (config_.max_roll_rate <= 0.0f || config_.max_pitch_rate <= 0.0f || 
         config_.max_yaw_rate <= 0.0f || config_.max_roll_angle <= 0.0f || 
         config_.max_pitch_angle <= 0.0f) {
         return false;
     }
     
     return true;
 }
 
 ControlOutputs FlightController::update(const RCInputs& rc_inputs, 
                                        const IMUData& imu_data,
                                        float battery_voltage,
                                        uint32_t dt_us) {
     
     ControlOutputs outputs;
     
     // Update timestamps
     if (rc_inputs.valid) {
         last_rc_update_ = micros();
     }
     if (imu_data.valid) {
         last_imu_update_ = micros();
     }
     
     // Check failsafe conditions
     checkFailsafes(rc_inputs, imu_data, battery_voltage);
     
     // Update arm state based on RC inputs and system state
     updateArmState(rc_inputs, imu_data);
     
     // If disarmed or in failsafe, return zero outputs
     if (arm_state_ != ArmState::ARMED || failsafe_active_) {
         return outputs; // All outputs initialized to 0
     }
     
     // Calculate setpoints from RC inputs
     calculateSetpoints(rc_inputs);
     
     // Execute control loop based on current mode
     switch (current_mode_) {
         case FlightMode::RATE_MODE:
             calculateRateModePID(imu_data, dt_us);
             break;
             
         case FlightMode::ANGLE_MODE:
             calculateAngleModePID(imu_data, dt_us);
             break;
             
         case FlightMode::ALTITUDE_HOLD:
             // TODO: Implement altitude hold mode
             // For now, fall back to angle mode
             calculateAngleModePID(imu_data, dt_us);
             break;
             
         case FlightMode::POSITION_HOLD:
             // TODO: Implement position hold mode
             // For now, fall back to angle mode
             calculateAngleModePID(imu_data, dt_us);
             break;
     }
     
     // Set outputs
     outputs.roll = roll_rate_pid_.output;
     outputs.pitch = pitch_rate_pid_.output;
     outputs.yaw = yaw_rate_pid_.output;
     outputs.throttle = scaleRCInput(rc_inputs.throttle, 1000.0f); // Scale throttle to 0-1000
     
     return outputs;
 }
 
 bool FlightController::setFlightMode(FlightMode mode) {
     // Only allow mode changes when disarmed or in specific conditions
     if (arm_state_ == ArmState::ARMED && failsafe_active_) {
         return false;
     }
     
     // Reset PID controllers when changing modes
     if (mode != current_mode_) {
         resetPIDControllers();
         current_mode_ = mode;
     }
     
     return true;
 }
 
 bool FlightController::setArmed(bool arm) {
     if (arm) {
         // Check arming conditions
         if (arm_state_ == ArmState::DISARMED && !failsafe_active_) {
             arm_state_ = ArmState::ARMING;
             return true;
         }
     } else {
         // Always allow disarming
         arm_state_ = ArmState::DISARMING;
         return true;
     }
     
     return false;
 }
 
 void FlightController::updatePIDGains(uint8_t axis, bool is_rate, const PIDConfig& config) {
     // Update PID gains for adaptive control
     // This allows real-time tuning and gain scheduling
     
     switch (axis) {
         case 0: // Roll
             if (is_rate) {
                 config_.roll_rate_pid = config;
             } else {
                 config_.roll_angle_pid = config;
             }
             break;
             
         case 1: // Pitch
             if (is_rate) {
                 config_.pitch_rate_pid = config;
             } else {
                 config_.pitch_angle_pid = config;
             }
             break;
             
         case 2: // Yaw
             if (is_rate) {
                 config_.yaw_rate_pid = config;
             }
             // Note: Yaw typically doesn't have angle mode
             break;
     }
 }
 
 void FlightController::resetPIDControllers() {
     // Reset all PID controller states
     roll_rate_pid_ = PIDState();
     pitch_rate_pid_ = PIDState();
     yaw_rate_pid_ = PIDState();
     roll_angle_pid_ = PIDState();
     pitch_angle_pid_ = PIDState();
 }
 
 void FlightController::emergencyStop() {
     arm_state_ = ArmState::DISARMED;
     resetPIDControllers();
     system_status_ |= static_cast<uint16_t>(SystemStatus::FAILSAFE_ACTIVE);
 }
 
 // ============================================================================
 // PRIVATE METHODS
 // ============================================================================
 
 void FlightController::calculateSetpoints(const RCInputs& rc_inputs) {
     // Apply deadband to RC inputs
     float roll_input = applyDeadband(rc_inputs.roll, RC_CENTER, RC_DEADBAND);
     float pitch_input = applyDeadband(rc_inputs.pitch, RC_CENTER, RC_DEADBAND);
     float yaw_input = applyDeadband(rc_inputs.yaw, RC_CENTER, RC_DEADBAND);
     
     if (current_mode_ == FlightMode::RATE_MODE) {
         // Rate mode: RC inputs directly control angular rates
         roll_rate_pid_.setpoint = rcToRate(roll_input, config_.max_roll_rate);
         pitch_rate_pid_.setpoint = rcToRate(pitch_input, config_.max_pitch_rate);
         yaw_rate_pid_.setpoint = rcToRate(yaw_input, config_.max_yaw_rate);
     } else {
         // Angle mode: RC inputs control target angles
         roll_angle_pid_.setpoint = rcToAngle(roll_input, config_.max_roll_angle);
         pitch_angle_pid_.setpoint = rcToAngle(pitch_input, config_.max_pitch_angle);
         yaw_rate_pid_.setpoint = rcToRate(yaw_input, config_.max_yaw_rate); // Yaw is always rate
     }
 }
 
 void FlightController::calculateRateModePID(const IMUData& imu_data, uint32_t dt_us) {
     if (!imu_data.valid) return;
     
     // Update rate PID controllers directly with gyro rates
     roll_rate_pid_.output = updatePID(roll_rate_pid_, config_.roll_rate_pid, 
                                      roll_rate_pid_.setpoint, imu_data.roll_rate, dt_us);
     
     pitch_rate_pid_.output = updatePID(pitch_rate_pid_, config_.pitch_rate_pid, 
                                       pitch_rate_pid_.setpoint, imu_data.pitch_rate, dt_us);
     
     yaw_rate_pid_.output = updatePID(yaw_rate_pid_, config_.yaw_rate_pid, 
                                     yaw_rate_pid_.setpoint, imu_data.yaw_rate, dt_us);
 }
 
 void FlightController::calculateAngleModePID(const IMUData& imu_data, uint32_t dt_us) {
     if (!imu_data.valid) return;
     
     // First, update angle PID controllers to get rate setpoints
     float roll_rate_setpoint = updatePID(roll_angle_pid_, config_.roll_angle_pid, 
                                         roll_angle_pid_.setpoint, imu_data.roll_angle, dt_us);
     
     float pitch_rate_setpoint = updatePID(pitch_angle_pid_, config_.pitch_angle_pid, 
                                          pitch_angle_pid_.setpoint, imu_data.pitch_angle, dt_us);
     
     // Constrain rate setpoints
     roll_rate_setpoint = constrain(roll_rate_setpoint, -config_.max_roll_rate, config_.max_roll_rate);
     pitch_rate_setpoint = constrain(pitch_rate_setpoint, -config_.max_pitch_rate, config_.max_pitch_rate);
     
     // Then update rate PID controllers with the rate setpoints
     roll_rate_pid_.output = updatePID(roll_rate_pid_, config_.roll_rate_pid, 
                                      roll_rate_setpoint, imu_data.roll_rate, dt_us);
     
     pitch_rate_pid_.output = updatePID(pitch_rate_pid_, config_.pitch_rate_pid, 
                                       pitch_rate_setpoint, imu_data.pitch_rate, dt_us);
     
     // Yaw is always rate control
     yaw_rate_pid_.output = updatePID(yaw_rate_pid_, config_.yaw_rate_pid, 
                                     yaw_rate_pid_.setpoint, imu_data.yaw_rate, dt_us);
 }
 
 float FlightController::updatePID(PIDState& pid_state, const PIDConfig& config, 
                                  float setpoint, float measurement, uint32_t dt_us) {
     
     // Calculate error
     pid_state.error = setpoint - measurement;
     
     // Convert dt to seconds
     float dt_s = dt_us / 1000000.0f;
     
     // Proportional term
     float p_term = config.p_gain * pid_state.error;
     
     // Integral term with anti-windup
     pid_state.integral += pid_state.error * dt_s;
     pid_state.integral = constrain(pid_state.integral, -config.max_i_term, config.max_i_term);
     float i_term = config.i_gain * pid_state.integral;
     
     // Derivative term with filtering
     float d_input = (pid_state.error - pid_state.last_error) / dt_s;
     pid_state.derivative = lowPassFilter(d_input, pid_state.derivative, config.d_filter_alpha);
     float d_term = config.d_gain * pid_state.derivative;
     
     // Calculate output
     float output = p_term + i_term + d_term;
     
     // Apply output limits
     output = constrain(output, -config.max_output, config.max_output);
     
     // Update state
     pid_state.last_error = pid_state.error;
     pid_state.output = output;
     
     return output;
 }
 
 void FlightController::checkFailsafes(const RCInputs& rc_inputs, const IMUData& imu_data, 
                                      float battery_voltage) {
     
     uint32_t current_time = micros();
     bool failsafe_triggered = false;
     
     // Check RC signal timeout
     if (!rc_inputs.valid || (current_time - last_rc_update_) > (RC_TIMEOUT_MS * 1000)) {
         system_status_ |= static_cast<uint16_t>(SystemStatus::RC_SIGNAL_LOST);
         executeFailsafeAction(config_.rc_failsafe);
         failsafe_triggered = true;
     } else {
         system_status_ &= ~static_cast<uint16_t>(SystemStatus::RC_SIGNAL_LOST);
     }
     
     // Check IMU data validity
     if (!imu_data.valid || (current_time - last_imu_update_) > 100000) { // 100ms timeout
         system_status_ |= static_cast<uint16_t>(SystemStatus::IMU_FAILURE);
         executeFailsafeAction(config_.imu_failsafe);
         failsafe_triggered = true;
     } else {
         system_status_ &= ~static_cast<uint16_t>(SystemStatus::IMU_FAILURE);
     }
     
     // Check battery voltage
     if (battery_voltage < MIN_BATTERY_VOLTAGE) {
         system_status_ |= static_cast<uint16_t>(SystemStatus::LOW_BATTERY);
         executeFailsafeAction(config_.battery_failsafe);
         failsafe_triggered = true;
     } else {
         system_status_ &= ~static_cast<uint16_t>(SystemStatus::LOW_BATTERY);
     }
     
     // Update failsafe state
     if (failsafe_triggered) {
         system_status_ |= static_cast<uint16_t>(SystemStatus::FAILSAFE_ACTIVE);
         failsafe_active_ = true;
     } else {
         system_status_ &= ~static_cast<uint16_t>(SystemStatus::FAILSAFE_ACTIVE);
         failsafe_active_ = false;
     }
 }
 
 void FlightController::executeFailsafeAction(FailsafeAction action) {
     switch (action) {
         case FailsafeAction::DISARM:
             emergencyStop();
             break;
             
         case FailsafeAction::LAND:
             // TODO: Implement controlled landing sequence
             // For now, disarm as safety measure
             emergencyStop();
             break;
             
         case FailsafeAction::HOLD_POSITION:
             // TODO: Implement position hold failsafe
             // Requires GPS and position control
             emergencyStop();
             break;
             
         case FailsafeAction::RETURN_TO_HOME:
             // TODO: Implement return to home failsafe
             // Requires GPS and navigation
             emergencyStop();
             break;
     }
 }
 
 bool FlightController::checkArmingConditions(const RCInputs& rc_inputs, const IMUData& imu_data) {
     // Check basic arming conditions
     if (failsafe_active_) return false;
     
     // Check RC signal validity
     if (!rc_inputs.valid) return false;
     
     // Check IMU data validity
     if (!imu_data.valid) return false;
     
     // Check throttle is at minimum
     if (rc_inputs.throttle > (RC_MIN + 50)) return false;
     
     // Check sticks are centered (except throttle)
     if (abs(rc_inputs.roll - RC_CENTER) > 50) return false;
     if (abs(rc_inputs.pitch - RC_CENTER) > 50) return false;
     if (abs(rc_inputs.yaw - RC_CENTER) > 50) return false;
     
     // Check gyro rates are stable (not moving too fast)
     if (abs(imu_data.roll_rate) > 5.0f) return false;
     if (abs(imu_data.pitch_rate) > 5.0f) return false;
     if (abs(imu_data.yaw_rate) > 5.0f) return false;
     
     // Check angles are reasonable (not upside down)
     if (abs(imu_data.roll_angle) > 45.0f) return false;
     if (abs(imu_data.pitch_angle) > 45.0f) return false;
     
     return true;
 }
 
 void FlightController::updateArmState(const RCInputs& rc_inputs, const IMUData& imu_data) {
     static uint32_t arm_sequence_start = 0;
     static uint32_t disarm_sequence_start = 0;
     const uint32_t SEQUENCE_TIME_MS = 1000; // 1 second
     
     uint32_t current_time = millis();
     
     // Check for arming sequence (throttle low, yaw right for 1 second)
     bool arm_sequence = (rc_inputs.throttle < (RC_MIN + 50)) && 
                        (rc_inputs.yaw > (RC_MAX - 100)) &&
                        (abs(rc_inputs.roll - RC_CENTER) < 50) &&
                        (abs(rc_inputs.pitch - RC_CENTER) < 50);
     
     // Check for disarming sequence (throttle low, yaw left for 1 second)
     bool disarm_sequence = (rc_inputs.throttle < (RC_MIN + 50)) && 
                           (rc_inputs.yaw < (RC_MIN + 100)) &&
                           (abs(rc_inputs.roll - RC_CENTER) < 50) &&
                           (abs(rc_inputs.pitch - RC_CENTER) < 50);
     
     switch (arm_state_) {
         case ArmState::DISARMED:
             if (arm_sequence) {
                 if (arm_sequence_start == 0) {
                     arm_sequence_start = current_time;
                 } else if ((current_time - arm_sequence_start) >= SEQUENCE_TIME_MS) {
                     if (checkArmingConditions(rc_inputs, imu_data)) {
                         arm_state_ = ArmState::ARMED;
                         resetPIDControllers();
                     }
                     arm_sequence_start = 0;
                 }
             } else {
                 arm_sequence_start = 0;
             }
             break;
             
         case ArmState::ARMED:
             if (disarm_sequence) {
                 if (disarm_sequence_start == 0) {
                     disarm_sequence_start = current_time;
                 } else if ((current_time - disarm_sequence_start) >= SEQUENCE_TIME_MS) {
                     arm_state_ = ArmState::DISARMED;
                     resetPIDControllers();
                     disarm_sequence_start = 0;
                 }
             } else {
                 disarm_sequence_start = 0;
             }
             
             // Auto-disarm if throttle is low for extended period
             static uint32_t low_throttle_start = 0;
             if (rc_inputs.throttle < (RC_MIN + 50)) {
                 if (low_throttle_start == 0) {
                     low_throttle_start = current_time;
                 } else if ((current_time - low_throttle_start) >= 30000) { // 30 seconds
                     arm_state_ = ArmState::DISARMED;
                     resetPIDControllers();
                     low_throttle_start = 0;
                 }
             } else {
                 low_throttle_start = 0;
             }
             break;
             
         case ArmState::ARMING:
             // Transition to armed if conditions are met
             if (checkArmingConditions(rc_inputs, imu_data)) {
                 arm_state_ = ArmState::ARMED;
                 resetPIDControllers();
             } else {
                 arm_state_ = ArmState::DISARMED;
             }
             break;
             
         case ArmState::DISARMING:
             // Always transition to disarmed
             arm_state_ = ArmState::DISARMED;
             resetPIDControllers();
             break;
     }
 }
 
 float FlightController::applyDeadband(uint16_t input, uint16_t center, uint16_t deadband) {
     if (input > (center + deadband)) {
         return input - deadband;
     } else if (input < (center - deadband)) {
         return input + deadband;
     } else {
         return center;
     }
 }
 
 float FlightController::scaleRCInput(uint16_t input, float max_output) {
     // Scale RC input (1000-2000) to output range (-max_output to +max_output)
     float normalized = (input - RC_CENTER) / 500.0f; // Normalize to -1.0 to +1.0
     return constrain(normalized * max_output, -max_output, max_output);
 }
 
 // ============================================================================
 // UTILITY FUNCTIONS
 // ============================================================================
 
 float rcToRate(uint16_t input, float max_rate) {
     // Convert RC input to rate setpoint
     float normalized = (input - RC_CENTER) / 500.0f; // Normalize to -1.0 to +1.0
     return constrain(normalized * max_rate, -max_rate, max_rate);
 }
 
 float rcToAngle(uint16_t input, float max_angle) {
     // Convert RC input to angle setpoint
     float normalized = (input - RC_CENTER) / 500.0f; // Normalize to -1.0 to +1.0
     return constrain(normalized * max_angle, -max_angle, max_angle);
 }
 
 float constrain(float value, float min, float max) {
     if (value < min) return min;
     if (value > max) return max;
     return value;
 }
 
 float lowPassFilter(float input, float output, float alpha) {
     // Simple low-pass filter: output = alpha * input + (1-alpha) * previous_output
     return alpha * input + (1.0f - alpha) * output;
 }
 
 } // namespace FlightControl
 } // namespace Velma