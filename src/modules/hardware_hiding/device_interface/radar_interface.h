#ifndef RADAR_INTERFACE_H
#define RADAR_INTERFACE_H

#include <Arduino.h>

// Radar system types
enum class RadarType {
    FORWARD_LOOKING,
    DOPPLER,
    RADAR_ALTIMETER,
    WEATHER_RADAR,
    TARGET_TRACKING
};

// Radar operating modes
enum class RadarMode {
    OFF,
    STANDBY,
    SEARCH,
    TRACK,
    WEATHER,
    TERRAIN_FOLLOWING,
    CALIBRATION
};

// Radar target information
struct RadarTarget {
    uint16_t range;          // Range in meters
    uint16_t bearing;        // Bearing in degrees (0-359)
    int16_t velocity;        // Velocity in m/s (positive = approaching)
    uint8_t signalStrength;  // Signal strength (0-255)
    uint8_t targetType;      // Target classification
    bool valid;              // Whether target data is valid
    unsigned long timestamp; // When target was detected
};

// Radar configuration
struct RadarConfig {
    RadarType type;
    uint8_t powerLevel;      // Power level (0-100%)
    uint16_t frequency;      // Operating frequency in MHz
    uint16_t pulseWidth;     // Pulse width in microseconds
    uint16_t prf;            // Pulse repetition frequency in Hz
    uint8_t gain;            // Receiver gain (0-255)
    bool autoGain;           // Automatic gain control
    uint16_t maxRange;       // Maximum range in meters
    uint8_t scanPattern;     // Scan pattern type
};

// Radar status
struct RadarStatus {
    RadarMode mode;
    bool transmitting;
    bool receiving;
    uint8_t powerOutput;     // Current power output (0-100%)
    uint8_t temperature;     // System temperature in Celsius
    uint8_t errorFlags;      // Error status flags
    uint16_t uptime;         // System uptime in seconds
    bool healthy;            // Overall system health
};

class RadarInterface {
private:
    static const uint8_t MAX_TARGETS = 32;
    static const uint8_t MAX_CHANNELS = 4;
    
    RadarConfig config;
    RadarStatus status;
    
    // Target tracking
    RadarTarget targets[MAX_TARGETS];
    uint8_t targetCount;
    
    // Hardware control
    uint8_t antennaPin;      // Antenna control pin
    uint8_t powerPin;        // Power control pin
    uint8_t modePin;         // Mode control pin
    uint8_t statusPin;       // Status input pin
    
    // Signal processing
    uint16_t lastRange;
    uint16_t lastBearing;
    int16_t lastVelocity;
    
    // Timing and intervals
    unsigned long lastUpdateTime;
    unsigned long lastScanTime;
    unsigned long lastCalibrationTime;
    
    // Error handling
    uint8_t errorFlags;
    String lastError;
    
    // Private methods
    bool initializeHardware();
    bool setAntennaPosition(uint16_t azimuth, uint16_t elevation);
    bool setTransmitterPower(uint8_t power);
    bool setReceiverGain(uint8_t gain);
    void processReturnSignal();
    bool validateConfig(const RadarConfig& config);
    void updateTargets();
    void clearTargets();
    
public:
    RadarInterface();
    
    // Configuration
    bool configure(const RadarConfig& newConfig);
    RadarConfig getConfiguration() const { return config; }
    bool setRadarType(RadarType type);
    bool setPowerLevel(uint8_t power);
    bool setFrequency(uint16_t frequency);
    bool setPulseWidth(uint16_t width);
    bool setPRF(uint16_t prf);
    bool setGain(uint8_t gain);
    bool setMaxRange(uint16_t range);
    
    // Mode control
    bool setMode(RadarMode mode);
    RadarMode getMode() const { return status.mode; }
    bool startSearch();
    bool startTrack();
    bool startWeather();
    bool startTerrainFollowing();
    bool stop();
    
    // Target information
    uint8_t getTargetCount() const { return targetCount; }
    bool getTarget(uint8_t index, RadarTarget& target) const;
    bool getClosestTarget(RadarTarget& target) const;
    bool getStrongestTarget(RadarTarget& target) const;
    uint16_t getLastRange() const { return lastRange; }
    uint16_t getLastBearing() const { return lastBearing; }
    int16_t getLastVelocity() const { return lastVelocity; }
    
    // Status and health
    RadarStatus getStatus() const { return status; }
    bool isTransmitting() const { return status.transmitting; }
    bool isReceiving() const { return status.receiving; }
    bool isHealthy() const { return status.healthy; }
    uint8_t getPowerOutput() const { return status.powerOutput; }
    uint8_t getTemperature() const { return status.temperature; }
    
    // System control
    bool initialize();
    void update();
    void shutdown();
    bool performCalibration();
    bool performSelfTest();
    
    // Error handling
    uint8_t getErrorFlags() const { return errorFlags; }
    String getLastError() const { return lastError; }
    void clearErrors();
    
    // Diagnostics
    void printStatus();
    void printTargets();
    void printConfiguration();
    
    // Hardware-specific methods
    bool setAntennaAzimuth(uint16_t azimuth);
    bool setAntennaElevation(uint16_t elevation);
    bool setTransmitterState(bool enabled);
    bool setReceiverState(bool enabled);
};

#endif // RADAR_INTERFACE_H 