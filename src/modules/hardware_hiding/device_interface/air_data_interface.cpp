#include "air_data_interface.h"
#include <math.h>

// Error flag definitions
#define ERROR_SENSOR_READ_FAILED    0x01
#define ERROR_CALIBRATION_INVALID   0x02
#define ERROR_SENSOR_TIMEOUT        0x04
#define ERROR_INVALID_DATA          0x08
#define ERROR_I2C_COMMUNICATION     0x10

AirDataInterface::AirDataInterface() 
    : sensorCount(0), altitudeCalibration(0.0), airspeedCalibration(0.0), 
      temperatureCalibration(0.0), errorFlags(0) {
    
    // Initialize sensor arrays
    for (uint8_t i = 0; i < MAX_SENSORS; i++) {
        sensors[i].enabled = false;
        sensorHealth[i].totalReadings = 0;
        sensorHealth[i].failedReadings = 0;
        sensorHealth[i].lastReadingTime = 0;
        sensorHealth[i].isHealthy = true;
    }
    
    lastError = "";
}

bool AirDataInterface::initialize() {
    Wire.begin();
    
    // Initialize I2C communication
    if (!Wire.begin()) {
        lastError = "Failed to initialize I2C";
        errorFlags |= ERROR_I2C_COMMUNICATION;
        return false;
    }
    
    // Scan for available sensors
    for (uint8_t address = 0x08; address <= 0x77; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            // Found a device, check if it's a known air data sensor
            if (addSensor(AirDataSensorType::STATIC_PRESSURE, address)) {
                Serial.print("Found air data sensor at address 0x");
                Serial.println(address, HEX);
            }
        }
    }
    
    return sensorCount > 0;
}

bool AirDataInterface::addSensor(AirDataSensorType type, uint8_t i2cAddress, float offset, float scale) {
    if (sensorCount >= MAX_SENSORS) {
        lastError = "Maximum sensor count reached";
        return false;
    }
    
    // Check if address is already in use
    for (uint8_t i = 0; i < sensorCount; i++) {
        if (sensors[i].i2cAddress == i2cAddress) {
            lastError = "Sensor address already in use";
            return false;
        }
    }
    
    sensors[sensorCount].type = type;
    sensors[sensorCount].i2cAddress = i2cAddress;
    sensors[sensorCount].calibrationOffset = offset;
    sensors[sensorCount].calibrationScale = scale;
    sensors[sensorCount].enabled = true;
    
    sensorCount++;
    return true;
}

bool AirDataInterface::removeSensor(uint8_t sensorIndex) {
    if (sensorIndex >= sensorCount) {
        lastError = "Invalid sensor index";
        return false;
    }
    
    // Shift remaining sensors
    for (uint8_t i = sensorIndex; i < sensorCount - 1; i++) {
        sensors[i] = sensors[i + 1];
        sensorHealth[i] = sensorHealth[i + 1];
    }
    
    sensorCount--;
    return true;
}

bool AirDataInterface::getAirData(AirDataMeasurement& measurement) {
    if (sensorCount == 0) {
        lastError = "No sensors available";
        return false;
    }
    
    // Initialize measurement structure
    measurement.timestamp = millis();
    measurement.isValid = false;
    
    // Aggregate data from all healthy sensors
    float altitudeSum = 0.0, airspeedSum = 0.0, pressureSum = 0.0;
    float temperatureSum = 0.0, aoaSum = 0.0, sideslipSum = 0.0;
    uint8_t validReadings = 0;
    
    for (uint8_t i = 0; i < sensorCount; i++) {
        if (!sensors[i].enabled || !sensorHealth[i].isHealthy) {
            continue;
        }
        
        AirDataMeasurement sensorMeasurement;
        if (readSensorData(i, sensorMeasurement)) {
            altitudeSum += sensorMeasurement.altitude;
            airspeedSum += sensorMeasurement.airspeed;
            pressureSum += sensorMeasurement.pressure;
            temperatureSum += sensorMeasurement.temperature;
            aoaSum += sensorMeasurement.angleOfAttack;
            sideslipSum += sensorMeasurement.sideslip;
            validReadings++;
            updateSensorHealth(i, true);
        } else {
            updateSensorHealth(i, false);
        }
    }
    
    if (validReadings == 0) {
        lastError = "No valid sensor readings";
        errorFlags |= ERROR_SENSOR_READ_FAILED;
        return false;
    }
    
    // Calculate averages
    measurement.altitude = altitudeSum / validReadings;
    measurement.airspeed = airspeedSum / validReadings;
    measurement.pressure = pressureSum / validReadings;
    measurement.temperature = temperatureSum / validReadings;
    measurement.angleOfAttack = aoaSum / validReadings;
    measurement.sideslip = sideslipSum / validReadings;
    
    // Calculate air density using ideal gas law
    const float R = 287.1; // Specific gas constant for air (J/kg·K)
    const float T = measurement.temperature + 273.15; // Convert to Kelvin
    measurement.density = measurement.pressure / (R * T);
    
    measurement.isValid = validateMeasurement(measurement);
    
    if (!measurement.isValid) {
        errorFlags |= ERROR_INVALID_DATA;
    }
    
    return measurement.isValid;
}

bool AirDataInterface::readSensorData(uint8_t sensorIndex, AirDataMeasurement& measurement) {
    if (sensorIndex >= sensorCount || !sensors[sensorIndex].enabled) {
        return false;
    }
    
    const AirDataSensorConfig& sensor = sensors[sensorIndex];
    
    // Read data based on sensor type
    switch (sensor.type) {
        case AirDataSensorType::STATIC_PRESSURE:
            return readPressureSensor(sensor, measurement);
        case AirDataSensorType::TEMPERATURE_PROBE:
            return readTemperatureSensor(sensor, measurement);
        case AirDataSensorType::PITOT_TUBE:
            return readPitotSensor(sensor, measurement);
        case AirDataSensorType::ANGLE_OF_ATTACK:
            return readAoASensor(sensor, measurement);
        case AirDataSensorType::SIDESLIP:
            return readSideslipSensor(sensor, measurement);
        default:
            return false;
    }
}

bool AirDataInterface::readPressureSensor(const AirDataSensorConfig& sensor, AirDataMeasurement& measurement) {
    Wire.beginTransmission(sensor.i2cAddress);
    Wire.write(0x00); // Register address for pressure data
    if (Wire.endTransmission() != 0) {
        return false;
    }
    
    if (Wire.requestFrom(sensor.i2cAddress, 3) != 3) {
        return false;
    }
    
    uint32_t rawPressure = 0;
    rawPressure |= Wire.read() << 16;
    rawPressure |= Wire.read() << 8;
    rawPressure |= Wire.read();
    
    // Convert raw value to pressure (Pa) - this is a simplified conversion
    float pressure = (float)rawPressure * 0.01; // Adjust based on actual sensor
    
    // Apply calibration
    pressure = applyCalibration(pressure, sensor.calibrationOffset, sensor.calibrationScale);
    
    // Calculate altitude using standard atmosphere model
    const float P0 = 101325.0; // Standard sea level pressure (Pa)
    const float T0 = 288.15;   // Standard sea level temperature (K)
    const float L = -0.0065;   // Temperature lapse rate (K/m)
    const float g = 9.80665;   // Gravitational acceleration (m/s²)
    const float R = 287.1;     // Specific gas constant for air (J/kg·K)
    
    if (pressure > 0) {
        measurement.altitude = (T0 / L) * (pow(pressure / P0, -L * R / g) - 1);
    } else {
        measurement.altitude = 0;
    }
    
    measurement.pressure = pressure;
    measurement.timestamp = millis();
    
    return true;
}

bool AirDataInterface::readTemperatureSensor(const AirDataSensorConfig& sensor, AirDataMeasurement& measurement) {
    Wire.beginTransmission(sensor.i2cAddress);
    Wire.write(0x03); // Register address for temperature data
    if (Wire.endTransmission() != 0) {
        return false;
    }
    
    if (Wire.requestFrom(sensor.i2cAddress, 2) != 2) {
        return false;
    }
    
    uint16_t rawTemp = Wire.read() << 8 | Wire.read();
    float temperature = (float)rawTemp * 0.01 - 273.15; // Convert to Celsius
    
    // Apply calibration
    temperature = applyCalibration(temperature, sensor.calibrationOffset, sensor.calibrationScale);
    
    measurement.temperature = temperature;
    measurement.timestamp = millis();
    
    return true;
}

bool AirDataInterface::readPitotSensor(const AirDataSensorConfig& sensor, AirDataMeasurement& measurement) {
    // Read dynamic pressure from pitot tube
    Wire.beginTransmission(sensor.i2cAddress);
    Wire.write(0x01); // Register address for dynamic pressure
    if (Wire.endTransmission() != 0) {
        return false;
    }
    
    if (Wire.requestFrom(sensor.i2cAddress, 2) != 2) {
        return false;
    }
    
    uint16_t rawPressure = Wire.read() << 8 | Wire.read();
    float dynamicPressure = (float)rawPressure * 0.01; // Convert to Pa
    
    // Apply calibration
    dynamicPressure = applyCalibration(dynamicPressure, sensor.calibrationOffset, sensor.calibrationScale);
    
    // Calculate airspeed using Bernoulli's equation
    // v = sqrt(2 * P_dynamic / ρ)
    // For now, use standard air density
    const float rho = 1.225; // kg/m³ at sea level
    if (dynamicPressure > 0) {
        measurement.airspeed = sqrt(2 * dynamicPressure / rho);
    } else {
        measurement.airspeed = 0;
    }
    
    measurement.timestamp = millis();
    
    return true;
}

bool AirDataInterface::readAoASensor(const AirDataSensorConfig& sensor, AirDataMeasurement& measurement) {
    Wire.beginTransmission(sensor.i2cAddress);
    Wire.write(0x02); // Register address for AoA data
    if (Wire.endTransmission() != 0) {
        return false;
    }
    
    if (Wire.requestFrom(sensor.i2cAddress, 2) != 2) {
        return false;
    }
    
    uint16_t rawAoA = Wire.read() << 8 | Wire.read();
    float angleOfAttack = (float)rawAoA * 0.01 - 90.0; // Convert to degrees, center at 0
    
    // Apply calibration
    angleOfAttack = applyCalibration(angleOfAttack, sensor.calibrationOffset, sensor.calibrationScale);
    
    measurement.angleOfAttack = angleOfAttack;
    measurement.timestamp = millis();
    
    return true;
}

bool AirDataInterface::readSideslipSensor(const AirDataSensorConfig& sensor, AirDataMeasurement& measurement) {
    Wire.beginTransmission(sensor.i2cAddress);
    Wire.write(0x04); // Register address for sideslip data
    if (Wire.endTransmission() != 0) {
        return false;
    }
    
    if (Wire.requestFrom(sensor.i2cAddress, 2) != 2) {
        return false;
    }
    
    uint16_t rawSideslip = Wire.read() << 8 | Wire.read();
    float sideslip = (float)rawSideslip * 0.01 - 90.0; // Convert to degrees, center at 0
    
    // Apply calibration
    sideslip = applyCalibration(sideslip, sensor.calibrationOffset, sensor.calibrationScale);
    
    measurement.sideslip = sideslip;
    measurement.timestamp = millis();
    
    return true;
}

void AirDataInterface::updateSensorHealth(uint8_t sensorIndex, bool readingSuccess) {
    if (sensorIndex >= sensorCount) return;
    
    SensorHealth& health = sensorHealth[sensorIndex];
    health.totalReadings++;
    health.lastReadingTime = millis();
    
    if (!readingSuccess) {
        health.failedReadings++;
        // Mark sensor as unhealthy if failure rate is too high
        if (health.failedReadings > 10 && health.totalReadings > 20) {
            health.isHealthy = false;
        }
    } else {
        // Mark sensor as healthy if recent readings are successful
        if (health.failedReadings < 5) {
            health.isHealthy = true;
        }
    }
}

float AirDataInterface::applyCalibration(float rawValue, float offset, float scale) {
    return (rawValue + offset) * scale;
}

bool AirDataInterface::validateMeasurement(const AirDataMeasurement& measurement) {
    // Check for reasonable ranges
    if (measurement.altitude < -1000 || measurement.altitude > 50000) return false;
    if (measurement.airspeed < 0 || measurement.airspeed > 300) return false;
    if (measurement.pressure < 1000 || measurement.pressure > 120000) return false;
    if (measurement.temperature < -60 || measurement.temperature > 60) return false;
    if (measurement.angleOfAttack < -45 || measurement.angleOfAttack > 45) return false;
    if (measurement.sideslip < -45 || measurement.sideslip > 45) return false;
    
    return true;
}

// Individual getter methods
bool AirDataInterface::getAltitude(float& altitude) {
    AirDataMeasurement measurement;
    if (getAirData(measurement)) {
        altitude = measurement.altitude;
        return true;
    }
    return false;
}

bool AirDataInterface::getAirspeed(float& airspeed) {
    AirDataMeasurement measurement;
    if (getAirData(measurement)) {
        airspeed = measurement.airspeed;
        return true;
    }
    return false;
}

bool AirDataInterface::getPressure(float& pressure) {
    AirDataMeasurement measurement;
    if (getAirData(measurement)) {
        pressure = measurement.pressure;
        return true;
    }
    return false;
}

bool AirDataInterface::getTemperature(float& temperature) {
    AirDataMeasurement measurement;
    if (getAirData(measurement)) {
        temperature = measurement.temperature;
        return true;
    }
    return false;
}

bool AirDataInterface::getAngleOfAttack(float& angleOfAttack) {
    AirDataMeasurement measurement;
    if (getAirData(measurement)) {
        angleOfAttack = measurement.angleOfAttack;
        return true;
    }
    return false;
}

bool AirDataInterface::getSideslip(float& sideslip) {
    AirDataMeasurement measurement;
    if (getAirData(measurement)) {
        sideslip = measurement.sideslip;
        return true;
    }
    return false;
}

// Calibration methods
void AirDataInterface::setAltitudeCalibration(float offset, float scale) {
    altitudeCalibration = offset;
    // Apply to all altitude-related sensors
    for (uint8_t i = 0; i < sensorCount; i++) {
        if (sensors[i].type == AirDataSensorType::STATIC_PRESSURE) {
            sensors[i].calibrationOffset = offset;
            sensors[i].calibrationScale = scale;
        }
    }
}

void AirDataInterface::setAirspeedCalibration(float offset, float scale) {
    airspeedCalibration = offset;
    // Apply to all airspeed-related sensors
    for (uint8_t i = 0; i < sensorCount; i++) {
        if (sensors[i].type == AirDataSensorType::PITOT_TUBE) {
            sensors[i].calibrationOffset = offset;
            sensors[i].calibrationScale = scale;
        }
    }
}

void AirDataInterface::setTemperatureCalibration(float offset, float scale) {
    temperatureCalibration = offset;
    // Apply to all temperature sensors
    for (uint8_t i = 0; i < sensorCount; i++) {
        if (sensors[i].type == AirDataSensorType::TEMPERATURE_PROBE) {
            sensors[i].calibrationOffset = offset;
            sensors[i].calibrationScale = scale;
        }
    }
}

void AirDataInterface::performAutoCalibration() {
    // Simple auto-calibration: average readings over time and adjust offsets
    const int calibrationSamples = 100;
    float altitudeSum = 0, airspeedSum = 0, temperatureSum = 0;
    int validSamples = 0;
    
    for (int i = 0; i < calibrationSamples; i++) {
        AirDataMeasurement measurement;
        if (getAirData(measurement)) {
            altitudeSum += measurement.altitude;
            airspeedSum += measurement.airspeed;
            temperatureSum += measurement.temperature;
            validSamples++;
        }
        delay(10);
    }
    
    if (validSamples > 0) {
        // Adjust calibration offsets based on expected values
        float expectedAltitude = 0; // Ground level
        float expectedAirspeed = 0; // Ground speed
        float expectedTemperature = 20; // Room temperature
        
        float altitudeOffset = expectedAltitude - (altitudeSum / validSamples);
        float airspeedOffset = expectedAirspeed - (airspeedSum / validSamples);
        float temperatureOffset = expectedTemperature - (temperatureSum / validSamples);
        
        setAltitudeCalibration(altitudeOffset, 1.0);
        setAirspeedCalibration(airspeedOffset, 1.0);
        setTemperatureCalibration(temperatureOffset, 1.0);
    }
}

// Health monitoring methods
bool AirDataInterface::isSensorHealthy(uint8_t sensorIndex) const {
    if (sensorIndex >= sensorCount) return false;
    return sensorHealth[sensorIndex].isHealthy;
}

void AirDataInterface::getSensorHealth(uint8_t sensorIndex, SensorHealth& health) const {
    if (sensorIndex >= sensorCount) {
        health = SensorHealth{};
        return;
    }
    health = sensorHealth[sensorIndex];
}

uint8_t AirDataInterface::getHealthySensorCount() const {
    uint8_t count = 0;
    for (uint8_t i = 0; i < sensorCount; i++) {
        if (sensorHealth[i].isHealthy) count++;
    }
    return count;
}

// Error handling methods
void AirDataInterface::clearErrors() {
    errorFlags = 0;
    lastError = "";
}

// System interface methods
void AirDataInterface::update() {
    // Update sensor health status
    for (uint8_t i = 0; i < sensorCount; i++) {
        if (sensors[i].enabled) {
            // Check if sensor is responding
            Wire.beginTransmission(sensors[i].i2cAddress);
            if (Wire.endTransmission() != 0) {
                updateSensorHealth(i, false);
            }
        }
    }
}

void AirDataInterface::shutdown() {
    // Disable all sensors
    for (uint8_t i = 0; i < sensorCount; i++) {
        sensors[i].enabled = false;
    }
}

// Diagnostic functions
void AirDataInterface::runSelfTest() {
    Serial.println("=== Air Data Interface Self Test ===");
    
    // Test sensor communication
    for (uint8_t i = 0; i < sensorCount; i++) {
        Serial.print("Sensor ");
        Serial.print(i);
        Serial.print(" (Type: ");
        Serial.print((int)sensors[i].type);
        Serial.print(", Address: 0x");
        Serial.print(sensors[i].i2cAddress, HEX);
        Serial.print("): ");
        
        Wire.beginTransmission(sensors[i].i2cAddress);
        if (Wire.endTransmission() == 0) {
            Serial.println("OK");
        } else {
            Serial.println("FAIL");
        }
    }
    
    // Test data acquisition
    AirDataMeasurement measurement;
    if (getAirData(measurement)) {
        Serial.println("Data acquisition: OK");
        Serial.print("Altitude: "); Serial.print(measurement.altitude); Serial.println(" m");
        Serial.print("Airspeed: "); Serial.print(measurement.airspeed); Serial.println(" m/s");
        Serial.print("Pressure: "); Serial.print(measurement.pressure); Serial.println(" Pa");
        Serial.print("Temperature: "); Serial.print(measurement.temperature); Serial.println(" °C");
    } else {
        Serial.println("Data acquisition: FAIL");
        Serial.print("Error: "); Serial.println(lastError);
    }
    
    Serial.println("=== Self Test Complete ===");
}

void AirDataInterface::printDiagnostics() {
    Serial.println("=== Air Data Interface Diagnostics ===");
    Serial.print("Total sensors: "); Serial.println(sensorCount);
    Serial.print("Healthy sensors: "); Serial.println(getHealthySensorCount());
    Serial.print("Error flags: 0x"); Serial.println(errorFlags, HEX);
    
    for (uint8_t i = 0; i < sensorCount; i++) {
        Serial.print("Sensor ");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(sensors[i].enabled ? "ENABLED" : "DISABLED");
        Serial.print(", ");
        Serial.print(sensorHealth[i].isHealthy ? "HEALTHY" : "UNHEALTHY");
        Serial.print(", Readings: ");
        Serial.print(sensorHealth[i].totalReadings);
        Serial.print(", Failures: ");
        Serial.println(sensorHealth[i].failedReadings);
    }
} 