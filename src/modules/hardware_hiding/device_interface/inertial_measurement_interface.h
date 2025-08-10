#ifndef INERTIAL_MEASUREMENT_INTERFACE_H
#define INERTIAL_MEASUREMENT_INTERFACE_H

#include <Arduino.h>
#include <Wire.h>

// IMU States
enum class IMUState {
    OFF,
    INITIALIZING,
    READY,
    ERROR,
    CALIBRATING
};

// IMU Configuration
enum class IMUConfig {
    GYRO_250DPS,
    GYRO_500DPS,
    GYRO_1000DPS,
    GYRO_2000DPS,
    ACCEL_2G,
    ACCEL_4G,
    ACCEL_8G,
    ACCEL_16G
};

// MPU6050 specific configuration
enum class GyroRange {
    RANGE_250_DPS = 0,    ///< ±250 degrees/second
    RANGE_500_DPS = 1,    ///< ±500 degrees/second
    RANGE_1000_DPS = 2,   ///< ±1000 degrees/second
    RANGE_2000_DPS = 3    ///< ±2000 degrees/second
};

enum class AccelRange {
    RANGE_2G = 0,         ///< ±2g
    RANGE_4G = 1,         ///< ±4g
    RANGE_8G = 2,         ///< ±8g
    RANGE_16G = 3         ///< ±16g
};

enum class DLPF {
    DLPF_260HZ = 0,      ///< 260Hz bandwidth
    DLPF_184HZ = 1,      ///< 184Hz bandwidth
    DLPF_94HZ = 2,       ///< 94Hz bandwidth
    DLPF_44HZ = 3,       ///< 44Hz bandwidth
    DLPF_21HZ = 4,       ///< 21Hz bandwidth
    DLPF_10HZ = 5,       ///< 10Hz bandwidth
    DLPF_5HZ = 6,        ///< 5Hz bandwidth
    DLPF_RESERVED = 7    ///< Reserved
};

// IMU Data Structure
struct IMUData {
    float accel_x, accel_y, accel_z;
    float gyro_x, gyro_y, gyro_z;
    float temp;
    float angle_pitch, angle_roll, angle_yaw;
    float accel_angle_x, accel_angle_y;
    uint32_t timestamp;
    bool is_valid;
};

// Advanced MPU6050 data structures
struct RawIMUData {
    int16_t accel_x;  ///< Raw accelerometer X-axis
    int16_t accel_y;  ///< Raw accelerometer Y-axis
    int16_t accel_z;  ///< Raw accelerometer Z-axis
    int16_t gyro_x;   ///< Raw gyroscope X-axis
    int16_t gyro_y;   ///< Raw gyroscope Y-axis
    int16_t gyro_z;   ///< Raw gyroscope Z-axis
    int16_t temp;     ///< Raw temperature
};

struct ScaledIMUData {
    float accel_x;    ///< Accelerometer X-axis (m/s²)
    float accel_y;    ///< Accelerometer Y-axis (m/s²)
    float accel_z;    ///< Accelerometer Z-axis (m/s²)
    float gyro_x;     ///< Gyroscope X-axis (deg/s)
    float gyro_y;     ///< Gyroscope Y-axis (deg/s)
    float gyro_z;     ///< Gyroscope Z-axis (deg/s)
    float temp;       ///< Temperature (°C)
};

struct EulerAngles {
    float roll;       ///< Roll angle (degrees)
    float pitch;      ///< Pitch angle (degrees)
    float yaw;        ///< Yaw angle (degrees)
};

struct Quaternion {
    float w;          ///< W component
    float x;          ///< X component
    float y;          ///< Y component
    float z;          ///< Z component
};

// IMU Calibration Data
struct IMUCalibration {
    float accel_offset_x, accel_offset_y, accel_offset_z;
    float gyro_offset_x, gyro_offset_y, gyro_offset_z;
    float temp_offset;
    bool is_calibrated;
};

// Advanced MPU6050 calibration structure
struct CalibrationOffsets {
    int16_t accel_x_offset;   ///< Accelerometer X-axis offset
    int16_t accel_y_offset;   ///< Accelerometer Y-axis offset
    int16_t accel_z_offset;   ///< Accelerometer Z-axis offset
    int16_t gyro_x_offset;    ///< Gyroscope X-axis offset
    int16_t gyro_y_offset;    ///< Gyroscope Y-axis offset
    int16_t gyro_z_offset;    ///< Gyroscope Z-axis offset
};

// IMU Error Information
struct IMUError {
    bool gyro_error;
    bool accel_error;
    bool temp_error;
    bool communication_error;
    String error_message;
};

class InertialMeasurementInterface {
private:
    // Hardware Configuration
    uint8_t i2c_address;
    uint8_t transistor_pin;
    IMUConfig gyro_config;
    IMUConfig accel_config;
    
    // State Management
    IMUState currentState;
    IMUData currentData;
    IMUCalibration calibration;
    IMUError errorInfo;
    
    // Timing
    unsigned long previousTime, currentTime, elapsedTime;
    
    // Error Tracking
    float acc_err_x, acc_err_y, gyro_err_x, gyro_err_y, gyro_err_z;
    
    // Private Methods
    bool initializeHardware();
    bool configureSensors();
    void readRawData();
    void processData();
    void calculateAngles();
    bool validateData();
    void updateErrorInfo();
    
public:
    InertialMeasurementInterface(uint8_t address = 0x68, uint8_t pin = -1);
    ~InertialMeasurementInterface();
    
    // Initialization and Control
    bool initialize();
    void powerOn();
    void powerOff();
    bool isPowered() const;
    void reset();
    
    // Configuration
    void setGyroConfig(IMUConfig config);
    void setAccelConfig(IMUConfig config);
    void setI2CAddress(uint8_t address);
    void setTransistorPin(uint8_t pin);
    
    // Advanced MPU6050 Configuration
    void setGyroRange(GyroRange range);
    void setAccelRange(AccelRange range);
    void setDLPF(DLPF setting);
    void setSampleRate(uint8_t rate_hz);
    void setInterruptPin(uint8_t pin);
    
    // Data Access
    bool read();
    IMUData getData() const { return currentData; }
    float getAccelX() const { return currentData.accel_x; }
    float getAccelY() const { return currentData.accel_y; }
    float getAccelZ() const { return currentData.accel_z; }
    float getGyroX() const { return currentData.gyro_x; }
    float getGyroY() const { return currentData.gyro_y; }
    float getGyroZ() const { return currentData.gyro_z; }
    float getTemperature() const { return currentData.temp; }
    float getAnglePitch() const { return currentData.angle_pitch; }
    float getAngleRoll() const { return currentData.angle_roll; }
    float getAngleYaw() const { return currentData.angle_yaw; }
    
    // Advanced MPU6050 Data Access
    bool readRawData(RawIMUData& data);
    bool readScaledData(ScaledIMUData& data);
    bool getPitchRoll(EulerAngles& angles);
    bool getEulerAngles(EulerAngles& angles, float dt);
    float getTemperatureRaw();
    
    // Calibration
    void startCalibration();
    void stopCalibration();
    bool isCalibrating() const;
    bool isCalibrated() const { return calibration.is_calibrated; }
    void resetCalibration();
    IMUCalibration getCalibration() const { return calibration; }
    void setCalibration(const IMUCalibration& cal);
    
    // Advanced MPU6050 Calibration
    CalibrationOffsets getCalibrationOffsets() const;
    bool setCalibrationOffsets(const CalibrationOffsets& offsets);
    bool calibrate(uint16_t samples = 2000, uint8_t delay_ms = 3);
    
    // DMP (Digital Motion Processor) Support
    bool initDMP();
    bool readDMPQuaternion(Quaternion& quat);
    void quaternionToEuler(const Quaternion& quat, EulerAngles& angles);
    
    // FIFO Buffer Support
    bool initFIFO(bool enable_accel = true, bool enable_gyro = true);
    uint16_t getFIFOCount();
    uint16_t readFIFO(uint8_t* buffer, uint16_t length);
    bool resetFIFO();
    
    // Motion Detection
    bool configureMotionInterrupt(uint8_t threshold = 2, uint8_t duration = 40);
    bool isMotionDetected();
    bool clearMotionInterrupt();
    
    // Self-Test
    bool performSelfTest();
    void enableEcoMode();
    void disableEcoMode();
    
    // Error Handling
    bool hasError() const;
    IMUError getError() const { return errorInfo; }
    void clearErrors();
    
    // State Information
    IMUState getState() const { return currentState; }
    bool isReady() const { return currentState == IMUState::READY; }
    
    // Diagnostics
    void getDiagnostics(String& diagnostics);
    void printOutput();
    
    // Advanced Features
    void setFilterCoefficient(float coefficient);
    void enableTemperatureCompensation(bool enable);
    void setUpdateRate(uint8_t rate_hz);
};

#endif // INERTIAL_MEASUREMENT_INTERFACE_H 