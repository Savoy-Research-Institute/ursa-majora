/**
 * @file flight_control.h
 * @brief Velma Flight Control System - Main control loop and PID implementation
 * @author Velma Development Team
 * @version 1.0
 * @date 2025
 * 
 * This module provides the core flight control functionality for the Velma quadcopter
 * project, including PID control loops, flight mode management, and safety systems.
 */

 #ifndef VELMA_FLIGHT_CONTROL_H
 #define VELMA_FLIGHT_CONTROL_H
 
 #include <stdint.h>
 
 namespace Velma {
 namespace FlightControl {
 
 // ============================================================================
 // CONFIGURATION CONSTANTS
 // ============================================================================
 
 /** @brief Maximum number of control channels */
 static const uint8_t MAX_CHANNELS = 8;
 
 /** @brief Default RC signal timeout in milliseconds */
 static const uint16_t RC_TIMEOUT_MS = 1000;
 
 /** @brief RC signal deadband in microseconds */
 static const uint16_t RC_DEADBAND = 8;
 
 /** @brief RC signal center position in microseconds */
 static const uint16_t RC_CENTER = 1500;
 
 /** @brief RC signal minimum value in microseconds */
 static const uint16_t RC_MIN = 1000;
 
 /** @brief RC signal maximum value in microseconds */
 static const uint16_t RC_MAX = 2000;
 
 /** @brief Maximum PID output value */
 static const float MAX_PID_OUTPUT = 400.0f;
 
 /** @brief Minimum battery voltage for safe operation (in volts) */
 static const float MIN_BATTERY_VOLTAGE = 10.5f;
 
 // ============================================================================
 // ENUMERATIONS
 // ============================================================================
 
 /**
  * @brief Flight control modes
  */
 enum class FlightMode : uint8_t {
     RATE_MODE = 0,      ///< Rate/Acro mode - direct gyro rate control
     ANGLE_MODE = 1,     ///< Angle/Stabilize mode - self-leveling
     ALTITUDE_HOLD = 2,  ///< Altitude hold mode (future expansion)
     POSITION_HOLD = 3   ///< Position hold mode (future expansion)
 };
 
 /**
  * @brief System armed state
  */
 enum class ArmState : uint8_t {
     DISARMED = 0,       ///< System is disarmed
     ARMED = 1,          ///< System is armed and ready
     ARMING = 2,         ///< System is in arming sequence
     DISARMING = 3       ///< System is in disarming sequence
 };
 
 /**
  * @brief Failsafe actions
  */
 enum class FailsafeAction : uint8_t {
     DISARM = 0,         ///< Immediately disarm motors
     LAND = 1,           ///< Attempt controlled landing
     HOLD_POSITION = 2,  ///< Hold current position (if GPS available)
     RETURN_TO_HOME = 3  ///< Return to launch point (if GPS available)
 };
 
 /**
  * @brief System status flags
  */
 enum class SystemStatus : uint16_t {
     NORMAL = 0x0000,
     RC_SIGNAL_LOST = 0x0001,
     IMU_FAILURE = 0x0002,
     LOW_BATTERY = 0x0004,
     MOTOR_FAILURE = 0x0008,
     GYRO_CALIBRATING = 0x0010,
     ACCEL_CALIBRATING = 0x0020,
     COMPASS_CALIBRATING = 0x0040,
     FAILSAFE_ACTIVE = 0x0080
 };
 
 // ============================================================================
 // DATA STRUCTURES
 // ============================================================================
 
 /**
  * @brief PID controller configuration
  */
 struct PIDConfig {
     float p_gain;           ///< Proportional gain
     float i_gain;           ///< Integral gain
     float d_gain;           ///< Derivative gain
     float max_output;       ///< Maximum output limit
     float max_i_term;       ///< Maximum integral term (anti-windup)
     float d_filter_alpha;   ///< Derivative filter coefficient (0.0-1.0)
     
     /**
      * @brief Default constructor with safe initial values
      */
     PIDConfig() : p_gain(0.0f), i_gain(0.0f), d_gain(0.0f), 
                   max_output(MAX_PID_OUTPUT), max_i_term(MAX_PID_OUTPUT), 
                   d_filter_alpha(0.1f) {}
 };
 
 /**
  * @brief PID controller state
  */
 struct PIDState {
     float setpoint;         ///< Current setpoint
     float error;            ///< Current error
     float last_error;       ///< Previous error for derivative
     float integral;         ///< Integral accumulator
     float derivative;       ///< Filtered derivative term
     float output;           ///< PID output
     uint32_t last_time;     ///< Last update time in microseconds
     
     /**
      * @brief Default constructor
      */
     PIDState() : setpoint(0.0f), error(0.0f), last_error(0.0f),
                  integral(0.0f), derivative(0.0f), output(0.0f), 
                  last_time(0) {}
 };
 
 /**
  * @brief RC input data
  */
 struct RCInputs {
     uint16_t throttle;      ///< Throttle channel (1000-2000)
     uint16_t roll;          ///< Roll channel (1000-2000)
     uint16_t pitch;         ///< Pitch channel (1000-2000)
     uint16_t yaw;           ///< Yaw channel (1000-2000)
     uint16_t aux1;          ///< Auxiliary channel 1
     uint16_t aux2;          ///< Auxiliary channel 2
     uint16_t aux3;          ///< Auxiliary channel 3
     uint16_t aux4;          ///< Auxiliary channel 4
     uint32_t last_update;   ///< Last update timestamp
     bool valid;             ///< Signal validity flag
     
     /**
      * @brief Default constructor
      */
     RCInputs() : throttle(RC_CENTER), roll(RC_CENTER), pitch(RC_CENTER),
                  yaw(RC_CENTER), aux1(RC_CENTER), aux2(RC_CENTER),
                  aux3(RC_CENTER), aux4(RC_CENTER), last_update(0), valid(false) {}
 };
 
 /**
  * @brief IMU sensor data
  */
 struct IMUData {
     float roll_angle;       ///< Roll angle in degrees
     float pitch_angle;      ///< Pitch angle in degrees
     float yaw_angle;        ///< Yaw angle in degrees
     float roll_rate;        ///< Roll rate in degrees/second
     float pitch_rate;       ///< Pitch rate in degrees/second
     float yaw_rate;         ///< Yaw rate in degrees/second
     uint32_t timestamp;     ///< Data timestamp
     bool valid;             ///< Data validity flag
     
     /**
      * @brief Default constructor
      */
     IMUData() : roll_angle(0.0f), pitch_angle(0.0f), yaw_angle(0.0f),
                 roll_rate(0.0f), pitch_rate(0.0f), yaw_rate(0.0f),
                 timestamp(0), valid(false) {}
 };
 
 /**
  * @brief Control outputs
  */
 struct ControlOutputs {
     float roll;             ///< Roll control output
     float pitch;            ///< Pitch control output
     float yaw;              ///< Yaw control output
     float throttle;         ///< Throttle output
     
     /**
      * @brief Default constructor
      */
     ControlOutputs() : roll(0.0f), pitch(0.0f), yaw(0.0f), throttle(0.0f) {}
 };
 
 /**
  * @brief Flight control system configuration
  */
 struct FlightControlConfig {
     PIDConfig roll_rate_pid;        ///< Roll rate PID configuration
     PIDConfig pitch_rate_pid;       ///< Pitch rate PID configuration
     PIDConfig yaw_rate_pid;         ///< Yaw rate PID configuration
     PIDConfig roll_angle_pid;       ///< Roll angle PID configuration
     PIDConfig pitch_angle_pid;      ///< Pitch angle PID configuration
     
     float max_roll_rate;            ///< Maximum roll rate (deg/s)
     float max_pitch_rate;           ///< Maximum pitch rate (deg/s)
     float max_yaw_rate;             ///< Maximum yaw rate (deg/s)
     float max_roll_angle;           ///< Maximum roll angle (degrees)
     float max_pitch_angle;          ///< Maximum pitch angle (degrees)
     
     FailsafeAction rc_failsafe;     ///< RC signal loss failsafe action
     FailsafeAction imu_failsafe;    ///< IMU failure failsafe action
     FailsafeAction battery_failsafe; ///< Low battery failsafe action
     
     /**
      * @brief Default constructor with safe initial values
      */
     FlightControlConfig() : max_roll_rate(180.0f), max_pitch_rate(180.0f),
                            max_yaw_rate(90.0f), max_roll_angle(45.0f),
                            max_pitch_angle(45.0f), rc_failsafe(FailsafeAction::DISARM),
                            imu_failsafe(FailsafeAction::DISARM),
                            battery_failsafe(FailsafeAction::LAND) {}
 };
 
 // ============================================================================
 // MAIN FLIGHT CONTROL CLASS
 // ============================================================================
 
 /**
  * @brief Main flight control system class
  */
 class FlightController {
 public:
     /**
      * @brief Constructor
      */
     FlightController();
     
     /**
      * @brief Destructor
      */
     ~FlightController();
     
     /**
      * @brief Initialize the flight control system
      * @param config System configuration
      * @return true if initialization successful
      */
     bool initialize(const FlightControlConfig& config);
     
     /**
      * @brief Update the control loop (call at high frequency)
      * @param rc_inputs RC input data
      * @param imu_data IMU sensor data
      * @param battery_voltage Current battery voltage
      * @param dt_us Delta time in microseconds
      * @return Control outputs
      */
     ControlOutputs update(const RCInputs& rc_inputs, 
                          const IMUData& imu_data,
                          float battery_voltage,
                          uint32_t dt_us);
     
     /**
      * @brief Set flight mode
      * @param mode Desired flight mode
      * @return true if mode change successful
      */
     bool setFlightMode(FlightMode mode);
     
     /**
      * @brief Get current flight mode
      * @return Current flight mode
      */
     FlightMode getFlightMode() const { return current_mode_; }
     
     /**
      * @brief Get current arm state
      * @return Current arm state
      */
     ArmState getArmState() const { return arm_state_; }
     
     /**
      * @brief Get system status
      * @return System status flags
      */
     uint16_t getSystemStatus() const { return system_status_; }
     
     /**
      * @brief Manual arm/disarm control
      * @param arm true to arm, false to disarm
      * @return true if successful
      */
     bool setArmed(bool arm);
     
     /**
      * @brief Update PID gains (for adaptive control)
      * @param axis 0=roll, 1=pitch, 2=yaw
      * @param is_rate true for rate PID, false for angle PID
      * @param config New PID configuration
      */
     void updatePIDGains(uint8_t axis, bool is_rate, const PIDConfig& config);
     
     /**
      * @brief Reset all PID controllers
      */
     void resetPIDControllers();
     
     /**
      * @brief Emergency stop - immediately disarm
      */
     void emergencyStop();
 
 private:
     // Configuration
     FlightControlConfig config_;
     
     // Control states
     FlightMode current_mode_;
     ArmState arm_state_;
     uint16_t system_status_;
     
     // PID controllers
     PIDState roll_rate_pid_;
     PIDState pitch_rate_pid_;
     PIDState yaw_rate_pid_;
     PIDState roll_angle_pid_;
     PIDState pitch_angle_pid_;
     
     // Internal state
     uint32_t last_rc_update_;
     uint32_t last_imu_update_;
     bool failsafe_active_;
     
     // Private methods
     void calculateSetpoints(const RCInputs& rc_inputs);
     void calculateRateModePID(const IMUData& imu_data, uint32_t dt_us);
     void calculateAngleModePID(const IMUData& imu_data, uint32_t dt_us);
     float updatePID(PIDState& pid_state, const PIDConfig& config, 
                    float setpoint, float measurement, uint32_t dt_us);
     void checkFailsafes(const RCInputs& rc_inputs, const IMUData& imu_data, 
                        float battery_voltage);
     void executeFailsafeAction(FailsafeAction action);
     bool checkArmingConditions(const RCInputs& rc_inputs, const IMUData& imu_data);
     void updateArmState(const RCInputs& rc_inputs, const IMUData& imu_data);
     float applyDeadband(uint16_t input, uint16_t center, uint16_t deadband);
     float scaleRCInput(uint16_t input, float max_output);
 };
 
 // ============================================================================
 // UTILITY FUNCTIONS
 // ============================================================================
 
 /**
  * @brief Convert microseconds to degrees per second for rate mode
  * @param input RC input value (1000-2000)
  * @param max_rate Maximum rate output
  * @return Scaled rate value
  */
 float rcToRate(uint16_t input, float max_rate);
 
 /**
  * @brief Convert microseconds to degrees for angle mode
  * @param input RC input value (1000-2000)
  * @param max_angle Maximum angle output
  * @return Scaled angle value
  */
 float rcToAngle(uint16_t input, float max_angle);
 
 /**
  * @brief Constrain value to min/max limits
  * @param value Input value
  * @param min Minimum limit
  * @param max Maximum limit
  * @return Constrained value
  */
 float constrain(float value, float min, float max);
 
 /**
  * @brief Low-pass filter implementation
  * @param input Current input value
  * @param output Previous output value
  * @param alpha Filter coefficient (0.0-1.0)
  * @return Filtered output
  */
 float lowPassFilter(float input, float output, float alpha);
 
 } // namespace FlightControl
 } // namespace Velma
 
 #endif // VELMA_FLIGHT_CONTROL_H