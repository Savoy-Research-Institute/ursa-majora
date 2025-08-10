#ifndef AIR_DATA_COMPUTER_MODULE_H
#define AIR_DATA_COMPUTER_MODULE_H

#include <Arduino.h>
#include "../../hardware_hiding/device_interface/air_data_interface.h"
#include "../../software_decision/physical_models/aircraft_motion.h"
#include "../../software_decision/application_data_types/numeric_types.h"

// Air data states
enum class AirDataState {
    INITIALIZING,
    CALIBRATING,
    OPERATIONAL,
    DEGRADED,
    ERROR,
    MAINTENANCE
};

// Air data modes
enum class AirDataMode {
    GROUND_MODE,
    TAKEOFF_MODE,
    CRUISE_MODE,
    APPROACH_MODE,
    LANDING_MODE,
    EMERGENCY_MODE
};

// Air data parameters
enum class AirDataParameter {
    ALTITUDE,
    AIRSPEED,
    MACH_NUMBER,
    VERTICAL_SPEED,
    TEMPERATURE,
    PRESSURE,
    DENSITY,
    ANGLE_OF_ATTACK,
    ANGLE_OF_SIDESLIP,
    TOTAL_PRESSURE,
    STATIC_PRESSURE,
    DYNAMIC_PRESSURE
};

// Air data configuration
struct AirDataConfig {
    bool enableAltitudeCalculation;
    bool enableAirspeedCalculation;
    bool enableMachCalculation;
    bool enableDensityCalculation;
    bool enableTemperatureCompensation;
    bool enablePressureCompensation;
    uint16_t updateRate;          // Hz
    uint16_t calibrationInterval; // seconds
    uint8_t averagingSamples;
    double seaLevelPressure;      // hPa
    double standardTemperature;   // K
    double lapseRate;             // K/m
};

// Air data structure
struct AirData {
    double altitude;              // meters
    double airspeed;              // m/s
    double machNumber;            // dimensionless
    double verticalSpeed;         // m/s
    double temperature;           // K
    double pressure;              // hPa
    double density;               // kg/m³
    double angleOfAttack;         // degrees
    double angleOfSideslip;       // degrees
    double totalPressure;         // hPa
    double staticPressure;        // hPa
    double dynamicPressure;       // hPa
    unsigned long timestamp;
    bool valid;
    uint8_t quality;              // 0-100
};

// Air data event types
enum class AirDataEventType {
    DATA_UPDATED,
    CALIBRATION_STARTED,
    CALIBRATION_COMPLETED,
    SENSOR_ERROR,
    CALCULATION_ERROR,
    QUALITY_DEGRADED,
    MODE_CHANGED,
    MAINTENANCE_REQUIRED
};

// Air data event structure
struct AirDataEvent {
    AirDataEventType type;
    String description;
    unsigned long timestamp;
    uint8_t severity;
    bool acknowledged;
    AirDataParameter parameter;
};

// Calibration data
struct CalibrationData {
    double altitudeOffset;
    double airspeedOffset;
    double pressureOffset;
    double temperatureOffset;
    double angleOfAttackOffset;
    double angleOfSideslipOffset;
    bool calibrated;
    unsigned long calibrationTime;
    uint8_t calibrationQuality;
};

// Performance data
struct PerformanceData {
    double maxAltitude;
    double maxAirspeed;
    double maxMachNumber;
    double minAirspeed;
    double stallSpeed;
    double serviceCeiling;
    double range;
    double endurance;
    double fuelConsumption;
    double climbRate;
    double descentRate;
};

class AirDataComputerModule {
private:
    // Current state and mode
    AirDataState currentState;
    AirDataMode currentMode;
    
    // Air data management
    AirData currentAirData;
    AirData previousAirData;
    AirData filteredAirData;
    
    // Configuration
    AirDataConfig config;
    CalibrationData calibration;
    PerformanceData performance;
    
    // Event management
    static const uint8_t MAX_EVENTS = 32;
    AirDataEvent events[MAX_EVENTS];
    uint8_t eventCount;
    uint8_t eventIndex;
    
    // Timing
    unsigned long lastUpdate;
    unsigned long lastCalibration;
    unsigned long airDataStartTime;
    
    // Error tracking
    uint8_t errorCount;
    uint8_t warningCount;
    uint8_t criticalErrorCount;
    
    // Data processing
    bool dataProcessingEnabled;
    uint8_t processedDataCount;
    uint8_t validDataCount;
    
    // Filtering and averaging
    double altitudeBuffer[100];
    double airspeedBuffer[100];
    double pressureBuffer[100];
    double temperatureBuffer[100];
    uint8_t bufferIndex;
    uint8_t bufferCount;
    
    // Calibration
    bool calibrationInProgress;
    unsigned long calibrationStartTime;
    uint16_t calibrationSamples;
    double calibrationData[100][6]; // [samples][alt, airspeed, pressure, temp, aoa, aos]
    
    // Component interfaces
    AirDataInterface* airDataInterface;
    AircraftMotion* aircraftMotion;
    NumericTypes* numericTypes;

public:
    AirDataComputerModule();
    ~AirDataComputerModule();
    
    // Initialization and control
    bool initialize();
    void startup();
    void shutdown();
    void reset();
    
    // State management
    AirDataState getCurrentState() const { return currentState; }
    AirDataMode getCurrentMode() const { return currentMode; }
    bool setAirDataMode(AirDataMode mode);
    void changeState(AirDataState newState);
    
    // Air data access
    void getCurrentAirData(AirData& data) const;
    void getPreviousAirData(AirData& data) const;
    void getFilteredAirData(AirData& data) const;
    bool isAirDataValid() const { return currentAirData.valid; }
    uint8_t getAirDataQuality() const { return currentAirData.quality; }
    
    // Specific air data parameters
    double getAltitude() const { return currentAirData.altitude; }
    double getAirspeed() const { return currentAirData.airspeed; }
    double getMachNumber() const { return currentAirData.machNumber; }
    double getVerticalSpeed() const { return currentAirData.verticalSpeed; }
    double getTemperature() const { return currentAirData.temperature; }
    double getPressure() const { return currentAirData.pressure; }
    double getDensity() const { return currentAirData.density; }
    double getAngleOfAttack() const { return currentAirData.angleOfAttack; }
    double getAngleOfSideslip() const { return currentAirData.angleOfSideslip; }
    double getTotalPressure() const { return currentAirData.totalPressure; }
    double getStaticPressure() const { return currentAirData.staticPressure; }
    double getDynamicPressure() const { return currentAirData.dynamicPressure; }
    
    // Air data calculations
    bool calculateAltitude(double staticPressure, double& altitude);
    bool calculateAirspeed(double totalPressure, double staticPressure, double& airspeed);
    bool calculateMachNumber(double totalPressure, double staticPressure, double& machNumber);
    bool calculateVerticalSpeed(double altitude, double previousAltitude, double timeDelta, double& verticalSpeed);
    bool calculateDensity(double pressure, double temperature, double& density);
    bool calculateAngleOfAttack(double alphaSensor, double& angleOfAttack);
    bool calculateAngleOfSideslip(double betaSensor, double& angleOfSideslip);
    
    // Performance calculations
    bool calculateStallSpeed(double weight, double density, double& stallSpeed);
    bool calculateServiceCeiling(double& serviceCeiling);
    bool calculateRange(double fuel, double consumption, double& range);
    bool calculateEndurance(double fuel, double consumption, double& endurance);
    bool calculateClimbRate(double thrust, double drag, double weight, double& climbRate);
    bool calculateDescentRate(double drag, double weight, double& descentRate);
    
    // Data processing
    void processAirData();
    void filterAirData();
    void averageAirData();
    void validateAirData();
    void updateAirDataQuality();
    
    // Calibration
    void startCalibration();
    void stopCalibration();
    bool isCalibrating() const { return calibrationInProgress; }
    bool addCalibrationSample(double altitude, double airspeed, double pressure, double temperature, double aoa, double aos);
    double getCalibrationProgress() const;
    bool calibrateAltitude(double knownAltitude);
    bool calibrateAirspeed(double knownAirspeed);
    bool calibratePressure(double knownPressure);
    bool calibrateTemperature(double knownTemperature);
    bool calibrateAngleOfAttack(double knownAOA);
    bool calibrateAngleOfSideslip(double knownAOS);
    
    // Configuration
    void setAirDataConfiguration(const AirDataConfig& config);
    AirDataConfig getAirDataConfiguration() const { return config; }
    void setUpdateRate(uint16_t rate);
    void setCalibrationInterval(uint16_t interval);
    void setAveragingSamples(uint8_t samples);
    void setSeaLevelPressure(double pressure);
    void setStandardTemperature(double temperature);
    void setLapseRate(double rate);
    
    // Error handling
    void addEvent(AirDataEventType type, const String& description, uint8_t severity = 128);
    uint8_t getErrorCount() const { return errorCount; }
    uint8_t getWarningCount() const { return warningCount; }
    uint8_t getCriticalErrorCount() const { return criticalErrorCount; }
    
    // Event management
    uint8_t getEventCount() const { return eventCount; }
    void getEvent(uint8_t index, AirDataEvent& event) const;
    void acknowledgeEvent(uint8_t index);
    void clearEvents();
    
    // Component interfaces
    void setAirDataInterface(AirDataInterface* interface) { airDataInterface = interface; }
    void setAircraftMotion(AircraftMotion* motion) { aircraftMotion = motion; }
    void setNumericTypes(NumericTypes* types) { numericTypes = types; }
    
    // Main update loop
    void update();
    
    // Diagnostics and testing
    void performDiagnostics();
    bool isAirDataComputerHealthy() const;
    void runAirDataTest();
    void testAltitudeCalculation();
    void testAirspeedCalculation();
    void testMachCalculation();
    void testPressureCalculation();
    void testTemperatureCalculation();
    void simulateSensorFailure();
    void simulateDataCorruption();
    
    // Utility functions
    bool validateAirDataConfiguration(const AirDataConfig& config) const;
    bool checkCalibrationTimeout() const;
    void updateAirDataStatus();
    void logAirData();
    
    // Status and information
    String getAirDataStatus() const;
    void printAirDataStatus();
    void printCalibrationData();
    void printPerformanceData();
    void printEventLog();
    void exportAirData(const String& filename);
    
    // Recovery and maintenance
    void performRecovery();
    void enterMaintenanceMode();
    void exitMaintenanceMode();
    void clearAirDataHistory();
    void resetAirDataCounters();
    
    // Statistical analysis
    double calculateAverageAltitude() const;
    double calculateAverageAirspeed() const;
    double calculateAverageTemperature() const;
    double calculateDataVariance(AirDataParameter parameter) const;
    bool generateAirDataReport(String& report);
    
    // Safety functions
    bool isAltitudeSafe() const;
    bool isAirspeedSafe() const;
    bool isTemperatureSafe() const;
    bool detectStallCondition() const;
    bool detectOverspeedCondition() const;
    bool detectAltitudeLimit() const;
};

#endif // AIR_DATA_COMPUTER_MODULE_H 