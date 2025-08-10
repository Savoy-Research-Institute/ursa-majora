#ifndef ESC_OUTPUT_INTERFACE_H
#define ESC_OUTPUT_INTERFACE_H

#include <Arduino.h>

// ESC States
enum class ESCState {
    DISABLED,
    INITIALIZING,
    READY,
    RUNNING,
    ERROR
};

// ESC Configuration
enum class ESCProtocol {
    PWM,
    ONESHOT125,
    ONESHOT42,
    MULTISHOT,
    DSHOT150,
    DSHOT300,
    DSHOT600,
    DSHOT1200
};

// ESC Output Data
struct ESCOutputData {
    uint16_t motor1, motor2, motor3, motor4;
    uint16_t motor5, motor6, motor7, motor8;
    uint32_t timestamp;
    bool is_valid;
};

// ESC Configuration
struct ESCConfig {
    ESCProtocol protocol;
    uint16_t min_pulse;
    uint16_t max_pulse;
    uint16_t idle_pulse;
    uint8_t motor_count;
    uint16_t update_rate_hz;
    bool enable_telemetry;
};

// ESC Error Information
struct ESCError {
    bool communication_error;
    bool motor_error[8];
    bool protocol_error;
    bool voltage_error;
    bool temperature_error;
    String error_message;
};

class ESCOutputInterface {
private:
    // Hardware Configuration
    uint8_t motor_pins[8];
    uint8_t motor_count;
    ESCProtocol protocol;
    
    // State Management
    ESCState currentState;
    ESCOutputData currentOutput;
    ESCConfig config;
    ESCError errorInfo;
    
    // Timing
    unsigned long lastUpdateTime;
    uint16_t updateInterval;
    
    // Output Limits
    uint16_t minPulse, maxPulse, idlePulse;
    
    // Private Methods
    bool initializeHardware();
    bool configureProtocol();
    void writePWMOutput();
    void writeDShotOutput();
    void writeOneShotOutput();
    bool validateOutput();
    void updateErrorInfo();
    
public:
    ESCOutputInterface();
    ~ESCOutputInterface();
    
    // Initialization and Control
    bool initialize();
    void enable();
    void disable();
    bool isEnabled() const;
    void reset();
    
    // Configuration
    void setMotorPins(const uint8_t* pins, uint8_t count);
    void setProtocol(ESCProtocol protocol);
    void setPulseLimits(uint16_t min, uint16_t max, uint16_t idle);
    void setUpdateRate(uint16_t rate_hz);
    void setMotorCount(uint8_t count);
    
    // Output Control
    void setMotorOutput(uint8_t motor, uint16_t value);
    void setAllMotors(uint16_t value);
    void setMotorOutputs(const uint16_t* values, uint8_t count);
    void update();
    
    // Data Access
    ESCOutputData getOutput() const { return currentOutput; }
    uint16_t getMotorOutput(uint8_t motor) const;
    ESCState getState() const { return currentState; }
    bool isReady() const { return currentState == ESCState::READY; }
    
    // Safety
    void emergencyStop();
    void setThrottleLimit(uint8_t percentage);
    void enableArmingSequence(bool enable);
    
    // Calibration
    void startCalibration();
    void stopCalibration();
    bool isCalibrating() const;
    void setCalibrationData(const uint16_t* min_pulses, const uint16_t* max_pulses);
    
    // Error Handling
    bool hasError() const;
    ESCError getError() const { return errorInfo; }
    void clearErrors();
    
    // Diagnostics
    bool performSelfTest();
    void getDiagnostics(String& diagnostics);
    void printOutput();
    
    // Advanced Features
    void enableTelemetry(bool enable);
    void setMotorDirection(uint8_t motor, bool reverse);
    void setMotorCurve(uint8_t motor, const float* curve, uint8_t points);
    void enableMotor(uint8_t motor, bool enable);
    
    // Protocol Specific
    void setDShotTelemetry(bool enable);
    void setOneShotTiming(uint16_t timing_us);
    void setPWMFrequency(uint16_t frequency_hz);
};

#endif // ESC_OUTPUT_INTERFACE_H 