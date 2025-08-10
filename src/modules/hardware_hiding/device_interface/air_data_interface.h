#ifndef AIR_DATA_INTERFACE_H
#define AIR_DATA_INTERFACE_H

#include <Arduino.h>
#include <Wire.h>

// Air data sensor types
enum class AirDataSensorType {
    PITOT_TUBE,
    STATIC_PRESSURE,
    TEMPERATURE_PROBE,
    ANGLE_OF_ATTACK,
    SIDESLIP
};

// Air data measurement structure
struct AirDataMeasurement {
    float altitude;           // Altitude in meters
    float airspeed;           // Airspeed in m/s
    float pressure;           // Pressure in Pa
    float temperature;        // Temperature in Celsius
    float angleOfAttack;      // Angle of attack in degrees
    float sideslip;           // Sideslip angle in degrees
    float density;            // Air density in kg/m³
    unsigned long timestamp;  // Measurement timestamp
    bool isValid;             // Data validity flag
};

// Air data sensor configuration
struct AirDataSensorConfig {
    AirDataSensorType type;
    uint8_t i2cAddress;
    float calibrationOffset;
    float calibrationScale;
    bool enabled;
};

class AirDataInterface {
private:
    static const uint8_t MAX_SENSORS = 8;
    AirDataSensorConfig sensors[MAX_SENSORS];
    uint8_t sensorCount;
    
    // Sensor health monitoring
    struct SensorHealth {
        uint32_t totalReadings;
        uint32_t failedReadings;
        uint32_t lastReadingTime;
        bool isHealthy;
    } sensorHealth[MAX_SENSORS];
    
    // Calibration data
    float altitudeCalibration;
    float airspeedCalibration;
    float temperatureCalibration;
    
    // Error handling
    uint8_t errorFlags;
    String lastError;
    
    // Private methods
    bool readSensorData(uint8_t sensorIndex, AirDataMeasurement& measurement);
    void updateSensorHealth(uint8_t sensorIndex, bool readingSuccess);
    float applyCalibration(float rawValue, float offset, float scale);
    bool validateMeasurement(const AirDataMeasurement& measurement);
    
public:
    AirDataInterface();
    
    // Sensor management
    bool addSensor(AirDataSensorType type, uint8_t i2cAddress, float offset = 0.0, float scale = 1.0);
    bool removeSensor(uint8_t sensorIndex);
    bool enableSensor(uint8_t sensorIndex);
    bool disableSensor(uint8_t sensorIndex);
    
    // Data acquisition
    bool getAirData(AirDataMeasurement& measurement);
    bool getAltitude(float& altitude);
    bool getAirspeed(float& airspeed);
    bool getPressure(float& pressure);
    bool getTemperature(float& temperature);
    bool getAngleOfAttack(float& angleOfAttack);
    bool getSideslip(float& sideslip);
    
    // Calibration
    void setAltitudeCalibration(float offset, float scale);
    void setAirspeedCalibration(float offset, float scale);
    void setTemperatureCalibration(float offset, float scale);
    void performAutoCalibration();
    
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
};

#endif // AIR_DATA_INTERFACE_H 