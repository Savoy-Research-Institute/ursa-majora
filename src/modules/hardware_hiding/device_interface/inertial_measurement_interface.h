#ifndef INERTIAL_MEASUREMENT_INTERFACE_H
#define INERTIAL_MEASUREMENT_INTERFACE_H

#include <Arduino.h>
#include <Wire.h>

// IMU sensor types
enum class IMUSensorType {
    ACCELEROMETER,
    GYROSCOPE,
    MAGNETOMETER,
    TEMPERATURE
};

// IMU data structure
struct IMUData {
    // Accelerometer data (m/s²)
    float accelX, accelY, accelZ;
    float accelMagnitude;
    
    // Gyroscope data (rad/s)
    float gyroX, gyroY, gyroZ;
    float gyroMagnitude;
    
    // Magnetometer data (μT)
    float magX, magY, magZ;
    float magMagnitude;
    
    // Temperature (°C)
    float temperature;
    
    // Orientation (degrees)
    float roll, pitch, yaw;
    
    // Timestamp
    unsigned long timestamp;
    
    // Data validity
    bool accelValid, gyroValid, magValid, tempValid;
    bool orientationValid;
};

// IMU sensor configuration
struct IMUSensorConfig {
    IMUSensorType type;
    uint8_t i2cAddress;
    float calibrationOffset[3];  // X, Y, Z offsets
    float calibrationScale[3];   // X, Y, Z scale factors
    bool enabled;
    uint8_t sampleRate;          // Hz
    uint8_t range;               // Sensor range setting
};

class InertialMeasurementInterface {
private:
    static const uint8_t MAX_SENSORS = 4;
    IMUSensorConfig sensors[MAX_SENSORS];
    uint8_t sensorCount;
    
    // Sensor health monitoring
    struct SensorHealth {
        uint32_t totalReadings;
        uint32_t failedReadings;
        uint32_t lastReadingTime;
        bool isHealthy;
        float lastValidValue[3];
    } sensorHealth[MAX_SENSORS];
    
    // Calibration data
    float accelCalibration[3];
    float gyroCalibration[3];
    float magCalibration[3];
    
    // Filtering and processing
    float accelFiltered[3];
    float gyroFiltered[3];
    float magFiltered[3];
    float filterAlpha;  // Low-pass filter coefficient
    
    // Orientation calculation
    float quaternion[4];  // Quaternion representation
    float eulerAngles[3]; // Roll, pitch, yaw
    
    // Error handling
    uint8_t errorFlags;
    String lastError;
    
    // Private methods
    bool readSensorData(uint8_t sensorIndex, IMUData& data);
    void updateSensorHealth(uint8_t sensorIndex, bool readingSuccess);
    void applyCalibration(float* rawData, float* calibratedData, uint8_t sensorIndex);
    void applyLowPassFilter(float* input, float* output, float alpha);
    void calculateOrientation();
    void updateQuaternion(float dt, float gx, float gy, float gz);
    void quaternionToEuler(float* q, float* euler);
    bool validateMeasurement(const IMUData& data);
    
public:
    InertialMeasurementInterface();
    
    // Sensor management
    bool addSensor(IMUSensorType type, uint8_t i2cAddress, uint8_t sampleRate = 100, uint8_t range = 0);
    bool removeSensor(uint8_t sensorIndex);
    bool enableSensor(uint8_t sensorIndex);
    bool disableSensor(uint8_t sensorIndex);
    
    // Data acquisition
    bool getIMUData(IMUData& data);
    bool getAccelerometer(float& x, float& y, float& z);
    bool getGyroscope(float& x, float& y, float& z);
    bool getMagnetometer(float& x, float& y, float& z);
    bool getTemperature(float& temperature);
    bool getOrientation(float& roll, float& pitch, float& yaw);
    
    // Calibration
    void setAccelerometerCalibration(float offsetX, float offsetY, float offsetZ, float scaleX, float scaleY, float scaleZ);
    void setGyroscopeCalibration(float offsetX, float offsetY, float offsetZ, float scaleX, float scaleY, float scaleZ);
    void setMagnetometerCalibration(float offsetX, float offsetY, float offsetZ, float scaleX, float scaleY, float scaleZ);
    void performAutoCalibration();
    
    // Filtering
    void setFilterCoefficient(float alpha);
    float getFilterCoefficient() const { return filterAlpha; }
    
    // Health monitoring
    bool isSensorHealthy(uint8_t sensorIndex) const;
    void getSensorHealth(uint8_t sensorIndex, SensorHealth& health) const;
    uint8_t getHealthySensorCount() const;
    
    // Error handling
    uint8_t getErrorFlags() const { return errorFlags; }
    String getLastError() const { return lastError; }
    void clearErrors();
    
    // System interface
    bool initialize();
    void update();
    void shutdown();
    
    // Diagnostic functions
    void runSelfTest();
    void printDiagnostics();
    
    // Advanced functions
    void resetOrientation();
    void setReferenceOrientation(float roll, float pitch, float yaw);
    float getAccelerationMagnitude() const;
    float getAngularVelocityMagnitude() const;
    float getMagneticFieldMagnitude() const;
};

#endif // INERTIAL_MEASUREMENT_INTERFACE_H 