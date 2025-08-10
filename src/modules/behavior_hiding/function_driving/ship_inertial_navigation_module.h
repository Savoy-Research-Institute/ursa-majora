#ifndef SHIP_INERTIAL_NAVIGATION_MODULE_H
#define SHIP_INERTIAL_NAVIGATION_MODULE_H

#include <Arduino.h>
#include "../../hardware_hiding/device_interface/inertial_measurement_interface.h"
#include "../../software_decision/physical_models/aircraft_motion.h"
#include "../../software_decision/application_data_types/numeric_types.h"

// Navigation states
enum class NavigationState {
    INITIALIZING,
    ACQUIRING_GPS,
    NAVIGATION_ACTIVE,
    DEAD_RECKONING,
    ERROR_STATE,
    CALIBRATING
};

// Navigation modes
enum class NavigationMode {
    GROUND_NAVIGATION,
    AIR_NAVIGATION,
    MARINE_NAVIGATION,
    HYBRID_NAVIGATION
};

// Position structure
struct NavigationPosition {
    double latitude;      // Degrees
    double longitude;     // Degrees
    double altitude;      // Meters
    double heading;       // Degrees
    double pitch;         // Degrees
    double roll;          // Degrees
    unsigned long timestamp;
};

// Velocity structure
struct NavigationVelocity {
    double northVelocity;     // m/s
    double eastVelocity;      // m/s
    double downVelocity;      // m/s
    double groundSpeed;       // m/s
    double verticalSpeed;     // m/s
    unsigned long timestamp;
};

// Navigation accuracy
struct NavigationAccuracy {
    double positionAccuracy;  // meters
    double velocityAccuracy;  // m/s
    double attitudeAccuracy;  // degrees
    double headingAccuracy;   // degrees
    uint8_t gpsSatellites;    // Number of satellites
    bool gpsValid;
    bool imuValid;
};

// Navigation configuration
struct NavigationConfig {
    bool enableDeadReckoning;
    bool enableGPSIntegration;
    bool enableIMUIntegration;
    uint16_t updateRate;          // Hz
    uint16_t gpsTimeout;          // ms
    uint16_t imuTimeout;          // ms
    double maxPositionError;      // meters
    double maxVelocityError;      // m/s
    double maxAttitudeError;      // degrees
};

// Navigation event types
enum class NavigationEventType {
    GPS_ACQUIRED,
    GPS_LOST,
    IMU_CALIBRATED,
    IMU_ERROR,
    POSITION_UPDATE,
    VELOCITY_UPDATE,
    ATTITUDE_UPDATE,
    DEAD_RECKONING_ACTIVATED,
    NAVIGATION_ERROR,
    CALIBRATION_COMPLETE
};

// Navigation event structure
struct NavigationEvent {
    NavigationEventType type;
    String description;
    unsigned long timestamp;
    uint8_t severity;
    bool acknowledged;
};

class ShipInertialNavigationModule {
private:
    // Current state and mode
    NavigationState currentState;
    NavigationMode currentMode;
    
    // Navigation data
    NavigationPosition currentPosition;
    NavigationVelocity currentVelocity;
    NavigationAccuracy currentAccuracy;
    
    // Configuration
    NavigationConfig config;
    
    // Event management
    static const uint8_t MAX_EVENTS = 16;
    NavigationEvent events[MAX_EVENTS];
    uint8_t eventCount;
    uint8_t eventIndex;
    
    // Timing
    unsigned long lastUpdate;
    unsigned long lastGPSUpdate;
    unsigned long lastIMUUpdate;
    unsigned long navigationStartTime;
    
    // Error tracking
    uint8_t errorCount;
    uint8_t warningCount;
    uint8_t criticalErrorCount;
    
    // Dead reckoning
    NavigationPosition lastKnownPosition;
    NavigationVelocity lastKnownVelocity;
    bool deadReckoningActive;
    unsigned long deadReckoningStartTime;
    
    // Calibration
    bool calibrationInProgress;
    unsigned long calibrationStartTime;
    uint16_t calibrationSamples;
    double calibrationData[100][6]; // [samples][lat, lon, alt, heading, pitch, roll]
    
    // Component interfaces
    InertialMeasurementInterface* imuInterface;
    AircraftMotion* aircraftMotion;
    NumericTypes* numericTypes;

public:
    ShipInertialNavigationModule();
    ~ShipInertialNavigationModule();
    
    // Initialization and control
    bool initialize();
    void startup();
    void shutdown();
    void reset();
    
    // State management
    NavigationState getCurrentState() const { return currentState; }
    NavigationMode getCurrentMode() const { return currentMode; }
    bool setNavigationMode(NavigationMode mode);
    void changeState(NavigationState newState);
    
    // Navigation data access
    void getCurrentPosition(NavigationPosition& position) const;
    void getCurrentVelocity(NavigationVelocity& velocity) const;
    void getCurrentAccuracy(NavigationAccuracy& accuracy) const;
    
    // Position and velocity updates
    bool updatePosition(double latitude, double longitude, double altitude);
    bool updateVelocity(double northVel, double eastVel, double downVel);
    bool updateAttitude(double heading, double pitch, double roll);
    
    // Dead reckoning
    void activateDeadReckoning();
    void deactivateDeadReckoning();
    bool isDeadReckoningActive() const { return deadReckoningActive; }
    void updateDeadReckoning();
    
    // GPS integration
    bool updateGPSData(double latitude, double longitude, double altitude, uint8_t satellites);
    bool isGPSValid() const { return currentAccuracy.gpsValid; }
    uint8_t getGPSSatellites() const { return currentAccuracy.gpsSatellites; }
    
    // IMU integration
    bool updateIMUData(double heading, double pitch, double roll);
    bool isIMUValid() const { return currentAccuracy.imuValid; }
    
    // Calibration
    void startCalibration();
    void stopCalibration();
    bool isCalibrating() const { return calibrationInProgress; }
    bool addCalibrationSample(double lat, double lon, double alt, double heading, double pitch, double roll);
    double getCalibrationProgress() const;
    
    // Accuracy assessment
    bool isNavigationAccurate() const;
    double getPositionAccuracy() const { return currentAccuracy.positionAccuracy; }
    double getVelocityAccuracy() const { return currentAccuracy.velocityAccuracy; }
    double getAttitudeAccuracy() const { return currentAccuracy.attitudeAccuracy; }
    
    // Error handling
    void addEvent(NavigationEventType type, const String& description, uint8_t severity = 128);
    uint8_t getErrorCount() const { return errorCount; }
    uint8_t getWarningCount() const { return warningCount; }
    uint8_t getCriticalErrorCount() const { return criticalErrorCount; }
    
    // Event management
    uint8_t getEventCount() const { return eventCount; }
    void getEvent(uint8_t index, NavigationEvent& event) const;
    void acknowledgeEvent(uint8_t index);
    void clearEvents();
    
    // Configuration
    void setConfiguration(const NavigationConfig& newConfig);
    NavigationConfig getConfiguration() const { return config; }
    void setUpdateRate(uint16_t rate);
    void setGPSTimeout(uint16_t timeout);
    void setIMUTimeout(uint16_t timeout);
    
    // Component interfaces
    void setIMUInterface(InertialMeasurementInterface* interface) { imuInterface = interface; }
    void setAircraftMotion(AircraftMotion* motion) { aircraftMotion = motion; }
    void setNumericTypes(NumericTypes* types) { numericTypes = types; }
    
    // Main update loop
    void update();
    
    // Diagnostics and testing
    void performDiagnostics();
    bool isNavigationHealthy() const;
    void runNavigationTest();
    void simulateGPSLoss();
    void simulateIMUFailure();
    
    // Utility functions
    double calculateDistance(const NavigationPosition& pos1, const NavigationPosition& pos2) const;
    double calculateBearing(const NavigationPosition& from, const NavigationPosition& to) const;
    double calculateGroundSpeed() const;
    double calculateVerticalSpeed() const;
    
    // Status and information
    unsigned long getNavigationUptime() const;
    String getNavigationStatus() const;
    void printNavigationStatus();
    void printEventLog();
    
    // Recovery and maintenance
    void performRecovery();
    void enterMaintenanceMode();
    void exitMaintenanceMode();
};

#endif // SHIP_INERTIAL_NAVIGATION_MODULE_H 