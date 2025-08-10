#ifndef FLIGHT_CONTROL_MODULE_H
#define FLIGHT_CONTROL_MODULE_H

#include <Arduino.h>
#include "../../hardware_hiding/device_interface/inertial_measurement_interface.h"
#include "../../hardware_hiding/device_interface/esc_output_interface.h"
#include "../../software_decision/physical_models/aircraft_motion.h"

// Flight Control States
enum class FlightControlState {
    DISARMED,
    ARMING,
    ARMED,
    FLYING,
    LANDING,
    ERROR
};

// Flight Modes
enum class FlightMode {
    ACROBATIC,
    SELF_LEVELING,
    AUTONOMOUS,
    RTH,  // Return to Home
    MANUAL
};

// Flight Control Events
enum class FlightControlEventType {
    ARM_REQUEST,
    DISARM_REQUEST,
    MODE_CHANGE,
    SENSOR_UPDATE,
    CONTROL_INPUT,
    ERROR_DETECTED
};

// PID Controller Structure
struct PIDController {
    float p_gain, i_gain, d_gain;
    float min_limit, max_limit;
    float setpoint;
    float last_error;
    float i_mem;
    float output;
    
    void setGains(float p, float i, float d);
    void setLimits(float min, float max);
    void setSetpoint(float sp);
    float update(float measurement);
    void reset();
};

// Flight Control Data Structure
struct FlightControlData {
    float roll_input, pitch_input, yaw_input, throttle_input;
    float roll_output, pitch_output, yaw_output;
    float angle_pitch, angle_roll;
    float gyro_roll_input, gyro_pitch_input, gyro_yaw_input;
    bool auto_level;
    FlightMode current_mode;
};

// Flight Control Configuration
struct FlightControlConfig {
    float roll_p_gain, roll_i_gain, roll_d_gain;
    float pitch_p_gain, pitch_i_gain, pitch_d_gain;
    float yaw_p_gain, yaw_i_gain, yaw_d_gain;
    float roll_level_adjust, pitch_level_adjust;
    bool enable_auto_level;
    float max_angle;
    float min_throttle, max_throttle;
};

class FlightControlModule {
private:
    // State Management
    FlightControlState currentState;
    FlightMode currentMode;
    FlightControlConfig config;
    
    // PID Controllers
    PIDController rollPID, pitchPID, yawPID;
    
    // Flight Data
    FlightControlData flightData;
    
    // Arming and Safety
    bool primaryArmed;
    bool secondaryArmed;
    bool debouncePressed;
    bool onboardDisarmSwitchValue;
    
    // Switch Values
    int switch3way_1, switch3way_2;
    
    // IMU Data
    float angle_pitch_acc, angle_roll_acc;
    float acc_total_vector;
    float gyro_x_raw, gyro_y_raw, gyro_z_raw;
    float acc_x_raw, acc_y_raw, acc_z_raw;
    bool set_gyro_angles;
    
    // Hardware Interfaces
    InertialMeasurementInterface* imuInterface;
    ESCOutputInterface* escInterface;
    
    // Private Methods
    void acrobaticMode();
    void selfLevelingMode();
    void autonomousMode();
    void returnToHomeMode();
    void manualMode();
    
    void calculateFlightOrientation();
    void resetPIDControllers();
    void calibrateIMU(float pitch_calibration, float roll_calibration);
    
public:
    FlightControlModule();
    ~FlightControlModule();
    
    // Initialization
    void initialize(InertialMeasurementInterface* imu, ESCOutputInterface* esc);
    void setConfiguration(const FlightControlConfig& cfg);
    
    // State Management
    void updateFlight();
    void toggleArmingSequence();
    void setFlightMode(FlightMode mode);
    void setSwitchValues(int switch3way_1_val, int switch3way_2_val);
    
    // Data Access
    bool isArmed() const { return primaryArmed && secondaryArmed; }
    bool isAutoLevel() const { return config.enable_auto_level; }
    float getAnglePitch() const { return flightData.angle_pitch; }
    float getAngleRoll() const { return flightData.angle_roll; }
    FlightMode getCurrentMode() const { return currentMode; }
    FlightControlState getCurrentState() const { return currentState; }
    
    // Sensor Data Input
    void setGyroValues(float gyro_x, float gyro_y, float gyro_z);
    void setAccValues(float acc_x, float acc_y, float acc_z);
    
    // Configuration
    void setPIDGains(float roll_p, float roll_i, float roll_d,
                     float pitch_p, float pitch_i, float pitch_d,
                     float yaw_p, float yaw_i, float yaw_d);
    
    // Calibration
    void calibrateSensors();
    void resetCalibration();
    
    // Safety
    void emergencyDisarm();
    void failsafeActivation();
    
    // Diagnostics
    bool performSelfTest();
    void getDiagnostics(String& diagnostics);
};

#endif // FLIGHT_CONTROL_MODULE_H 