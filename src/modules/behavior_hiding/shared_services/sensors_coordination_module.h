#ifndef SENSORS_COORDINATION_MODULE_H
#define SENSORS_COORDINATION_MODULE_H

#include <Arduino.h>
#include <Wire.h>

// Sensor Types
enum class SensorType {
    MPU6050,           // Inertial Measurement Unit
    BMP280,            // Barometric Pressure Sensor
    GPS,               // Global Positioning System
    MAGNETOMETER,      // Magnetic Compass
    ULTRASONIC,        // Distance Sensor
    TEMPERATURE,       // Temperature Sensor
    HUMIDITY,          // Humidity Sensor
    CUSTOM             // Custom sensor type
};

// Sensor States
enum class SensorState {
    DISABLED,
    INITIALIZING,
    READY,
    READING,
    ERROR,
    CALIBRATING
};

// Sensor Data Structure
struct SensorData {
    // MPU6050 Data
    float accelerometer[3];    // [x, y, z] in m/s²
    float gyroscope[3];        // [x, y, z] in rad/s
    float temperature;          // in °C
    
    // BMP280 Data
    float pressure;             // in Pa
    float altitude;             // in meters
    float temperature_bmp;      // in °C
    
    // GPS Data
    float latitude;             // in degrees
    float longitude;            // in degrees
    float altitude_gps;         // in meters
    float speed;                // in m/s
    float heading;              // in degrees
    uint8_t satellites;         // number of satellites
    bool gps_fix;               // GPS fix status
    
    // Magnetometer Data
    float magnetic_field[3];    // [x, y, z] in μT
    float heading_mag;          // in degrees
    
    // Ultrasonic Data
    float distance;             // in meters
    
    // Metadata
    uint32_t timestamp;         // timestamp of reading
    SensorType source;          // source sensor
    bool is_valid;              // data validity flag
};

// Sensor Configuration
struct SensorConfig {
    SensorType type;
    uint8_t i2c_address;        // I2C address for I2C sensors
    uint8_t pin;                // Digital pin for digital sensors
    uint32_t update_interval;   // Update interval in milliseconds
    bool enable_calibration;    // Enable sensor calibration
    bool enable_filtering;      // Enable data filtering
    float filter_alpha;         // Filter coefficient (0.0 to 1.0)
};

// Sensor Error Information
struct SensorError {
    bool hardware_error;        // Hardware communication error
    bool calibration_error;     // Calibration error
    bool timeout_error;         // Sensor timeout
    bool data_error;            // Invalid data received
    String error_message;       // Detailed error description
};

// Sensor Calibration Data
struct SensorCalibration {
    float offset[3];            // Offset values for 3-axis sensors
    float scale[3];             // Scale factors for 3-axis sensors
    float temperature_offset;   // Temperature compensation offset
    bool is_calibrated;         // Calibration status
    uint32_t calibration_time;  // When calibration was performed
};

class SensorsCoordinationModule {
private:
    // Configuration
    SensorConfig configs[10];   // Support up to 10 sensors
    uint8_t sensor_count;
    
    // State Management
    SensorState currentState;
    SensorData currentData;
    SensorData lastData;
    SensorError errorInfo;
    
    // Sensor Management
    bool sensors_initialized[10];
    uint32_t last_update[10];
    SensorCalibration calibrations[10];
    
    // I2C Management
    bool i2c_initialized;
    uint8_t i2c_errors;
    
    // Private Methods
    bool initializeI2C();
    bool initializeSensor(uint8_t sensor_index);
    void processSensorData(uint8_t sensor_index);
    void updateSensorStatus(uint8_t sensor_index);
    void handleSensorError(uint8_t sensor_index, const String& error);
    bool validateSensorData(const SensorData& data);
    void applyCalibration(SensorData& data, uint8_t sensor_index);
    void applyFiltering(SensorData& data, uint8_t sensor_index);
    
    // Individual sensor methods
    bool initMPU6050(uint8_t sensor_index);
    bool initBMP280(uint8_t sensor_index);
    bool initGPS(uint8_t sensor_index);
    bool initMagnetometer(uint8_t sensor_index);
    bool initUltrasonic(uint8_t sensor_index);
    
    void readMPU6050(uint8_t sensor_index);
    void readBMP280(uint8_t sensor_index);
    void readGPS(uint8_t sensor_index);
    void readMagnetometer(uint8_t sensor_index);
    void readUltrasonic(uint8_t sensor_index);

public:
    // Constructor
    SensorsCoordinationModule();
    
    // Initialization
    bool initialize();
    bool addSensor(SensorType type, uint8_t i2c_address = 0, uint8_t pin = 0);
    void setSensorConfig(uint8_t sensor_index, const SensorConfig& config);
    void enableSensor(uint8_t sensor_index);
    void disableSensor(uint8_t sensor_index);
    
    // Configuration
    void setUpdateInterval(uint8_t sensor_index, uint32_t interval_ms);
    void enableCalibration(uint8_t sensor_index, bool enable);
    void enableFiltering(uint8_t sensor_index, bool enable);
    void setFilterCoefficient(uint8_t sensor_index, float alpha);
    
    // Data Reading
    bool readAllSensors();
    bool readSensor(uint8_t sensor_index);
    SensorData getSensorData(uint8_t sensor_index) const;
    SensorData getCurrentData() const { return currentData; }
    SensorData getLastData() const { return lastData; }
    
    // Specific sensor data access
    void getMPU6050Data(float* accel, float* gyro, float* temp);
    void getBMP280Data(float* pressure, float* altitude, float* temp);
    void getGPSData(float* lat, float* lon, float* alt, float* speed, float* heading);
    float getMagnetometerHeading() const;
    float getUltrasonicDistance() const;
    
    // Calibration
    bool calibrateSensor(uint8_t sensor_index);
    bool calibrateMPU6050();
    bool calibrateMagnetometer();
    void setCalibrationData(uint8_t sensor_index, const SensorCalibration& cal);
    SensorCalibration getCalibrationData(uint8_t sensor_index) const;
    void resetCalibration(uint8_t sensor_index);
    
    // Status and Control
    SensorState getState() const { return currentState; }
    bool isReady() const { return currentState == SensorState::READY; }
    bool isSensorEnabled(uint8_t sensor_index) const;
    bool isSensorCalibrated(uint8_t sensor_index) const;
    uint8_t getSensorCount() const { return sensor_count; }
    
    // Error Handling
    bool hasError() const;
    SensorError getError() const { return errorInfo; }
    void clearErrors();
    String getSensorErrorString(uint8_t sensor_index) const;
    
    // Diagnostics and Testing
    bool performSelfTest();
    void getDiagnostics(String& diagnostics);
    void printStatus();
    void testSensor(uint8_t sensor_index);
    
    // Utility Methods
    SensorType stringToSensorType(const String& type);
    String sensorTypeToString(SensorType type);
    bool isValidSensorIndex(uint8_t sensor_index) const;
    void printSensorInfo(uint8_t sensor_index);
    
    // Event Handlers
    void onSensorDataUpdated(uint8_t sensor_index, const SensorData& data);
    void onSensorError(uint8_t sensor_index, const String& error);
    void onCalibrationComplete(uint8_t sensor_index);
    void onStateChanged(SensorState oldState, SensorState newState);
    
    // Legacy compatibility methods (from old sensors.h)
    void sensors_init();
    void read_mpu6050(float* accel, float* gyro);
    float read_ultrasonic();
    float read_barometer();
    void read_gps(float* lat, float* lon, float* alt);
    float read_magnetometer();
};

// Global instance for legacy compatibility
extern SensorsCoordinationModule sensorsModule;

#endif // SENSORS_COORDINATION_MODULE_H 