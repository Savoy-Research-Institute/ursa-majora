#ifndef SYSTEM_MODULE_H
#define SYSTEM_MODULE_H

#include <Arduino.h>
#include "../../hardware_hiding/device_interface/air_data_interface.h"
#include "../../hardware_hiding/device_interface/audible_signal_interface.h"
#include "../../hardware_hiding/device_interface/inertial_measurement_interface.h"
#include "../../hardware_hiding/extended_computer/computer_state.h"
#include "../../shared_services/mode_determination.h"
#include "../../shared_services/system_values.h"

// System states
enum class SystemState {
    POWER_OFF,
    INITIALIZING,
    STANDBY,
    OPERATIONAL,
    MAINTENANCE,
    EMERGENCY,
    SHUTDOWN
};

// System modes
enum class SystemMode {
    GROUND_TEST,
    PREFLIGHT,
    TAKEOFF,
    CRUISE,
    MANEUVER,
    APPROACH,
    LANDING,
    POSTFLIGHT
};

// System health status
enum class SystemHealth {
    EXCELLENT,
    GOOD,
    FAIR,
    POOR,
    CRITICAL
};

// System event types
enum class SystemEventType {
    SYSTEM_STARTUP,
    MODE_CHANGE,
    ERROR_DETECTED,
    WARNING_ACTIVATED,
    EMERGENCY_TRIGGERED,
    SYSTEM_SHUTDOWN,
    CALIBRATION_COMPLETE,
    SENSOR_FAILURE,
    COMMUNICATION_LOSS
};

// System event structure
struct SystemEvent {
    SystemEventType type;
    String description;
    unsigned long timestamp;
    uint8_t severity;  // 0-255, higher = more severe
    bool acknowledged;
};

// System configuration
struct SystemConfig {
    bool autoCalibration;
    bool emergencyModeEnabled;
    uint16_t systemCheckInterval;
    uint8_t maxRetryAttempts;
    bool loggingEnabled;
    uint16_t logInterval;
};

class SystemModule {
private:
    // Current system state
    SystemState currentState;
    SystemMode currentMode;
    SystemHealth systemHealth;
    
    // System configuration
    SystemConfig config;
    
    // Event management
    static const uint8_t MAX_EVENTS = 32;
    SystemEvent events[MAX_EVENTS];
    uint8_t eventCount;
    uint8_t eventIndex;
    
    // Timing and intervals
    unsigned long lastSystemCheck;
    unsigned long lastLogTime;
    unsigned long systemStartTime;
    
    // System components
    AirDataInterface* airDataInterface;
    AudibleSignalInterface* audibleSignalInterface;
    InertialMeasurementInterface* inertialMeasurementInterface;
    ComputerState* computerState;
    ModeDetermination* modeDetermination;
    SystemValues* systemValues;
    
    // Error tracking
    uint8_t errorCount;
    uint8_t warningCount;
    uint8_t criticalErrorCount;
    
    // Private methods
    void changeState(SystemState newState);
    void changeMode(SystemMode newMode);
    void updateSystemHealth();
    void addEvent(SystemEventType type, const String& description, uint8_t severity = 128);
    void performSystemCheck();
    void logSystemStatus();
    bool validateStateTransition(SystemState newState);
    bool validateModeTransition(SystemMode newMode);
    void handleEmergency();
    void performRecoveryActions();
    
public:
    SystemModule();
    
    // System lifecycle management
    bool initialize();
    void startup();
    void shutdown();
    void emergencyShutdown();
    
    // State and mode management
    SystemState getCurrentState() const { return currentState; }
    SystemMode getCurrentMode() const { return currentMode; }
    SystemHealth getSystemHealth() const { return systemHealth; }
    
    bool setSystemMode(SystemMode mode);
    bool requestStateChange(SystemState newState);
    
    // System monitoring
    void update();
    void performDiagnostics();
    bool isSystemHealthy() const;
    uint8_t getErrorCount() const { return errorCount; }
    uint8_t getWarningCount() const { return warningCount; }
    uint8_t getCriticalErrorCount() const { return criticalErrorCount; }
    
    // Event management
    uint8_t getEventCount() const { return eventCount; }
    void getEvent(uint8_t index, SystemEvent& event) const;
    void acknowledgeEvent(uint8_t index);
    void clearEvents();
    
    // Configuration
    void setConfiguration(const SystemConfig& newConfig);
    SystemConfig getConfiguration() const { return config; }
    void setAutoCalibration(bool enabled);
    void setEmergencyMode(bool enabled);
    void setSystemCheckInterval(uint16_t interval);
    
    // Component access
    void setAirDataInterface(AirDataInterface* interface) { airDataInterface = interface; }
    void setAudibleSignalInterface(AudibleSignalInterface* interface) { audibleSignalInterface = interface; }
    void setInertialMeasurementInterface(InertialMeasurementInterface* interface) { inertialMeasurementInterface = interface; }
    void setComputerState(ComputerState* state) { computerState = state; }
    void setModeDetermination(ModeDetermination* mode) { modeDetermination = mode; }
    void setSystemValues(SystemValues* values) { systemValues = values; }
    
    // Emergency handling
    void triggerEmergency(const String& reason);
    void clearEmergency();
    bool isEmergencyActive() const;
    
    // Calibration
    void performSystemCalibration();
    bool isCalibrationRequired() const;
    
    // Logging and diagnostics
    void enableLogging(bool enabled);
    void setLogInterval(uint16_t interval);
    void printSystemStatus();
    void printEventLog();
    
    // System information
    unsigned long getUptime() const;
    String getSystemVersion() const;
    void getSystemInfo(String& info);
    
    // Recovery and maintenance
    void performRecovery();
    void enterMaintenanceMode();
    void exitMaintenanceMode();
    bool isMaintenanceMode() const;
    
    // Testing functions
    void runSystemTest();
    void runComponentTest(const String& component);
    void simulateError(SystemEventType errorType);
};

#endif // SYSTEM_MODULE_H 