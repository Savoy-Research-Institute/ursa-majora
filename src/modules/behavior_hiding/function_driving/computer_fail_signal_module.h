#ifndef COMPUTER_FAIL_SIGNAL_MODULE_H
#define COMPUTER_FAIL_SIGNAL_MODULE_H

#include <Arduino.h>
#include "../../hardware_hiding/device_interface/audible_signal_interface.h"
#include "../../hardware_hiding/device_interface/visual_indicators_interface.h"
#include "../../software_decision/application_data_types/state_events.h"

// Fail signal states
enum class FailSignalState {
    NORMAL,
    WARNING,
    CAUTION,
    ADVISORY,
    FAILURE,
    CRITICAL_FAILURE,
    EMERGENCY,
    SYSTEM_SHUTDOWN
};

// Fail signal types
enum class FailSignalType {
    COMPUTER_FAILURE,
    SENSOR_FAILURE,
    COMMUNICATION_FAILURE,
    POWER_FAILURE,
    MEMORY_FAILURE,
    PROCESSOR_FAILURE,
    PERIPHERAL_FAILURE,
    SOFTWARE_FAILURE,
    HARDWARE_FAILURE,
    SYSTEM_FAILURE
};

// Fail signal priority levels
enum class FailSignalPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL,
    EMERGENCY
};

// Fail signal configuration
struct FailSignalConfig {
    bool enableAudibleAlerts;
    bool enableVisualAlerts;
    bool enableVibrationAlerts;
    bool enableEmergencyShutdown;
    uint16_t alertDuration;      // ms
    uint16_t repeatInterval;     // ms
    uint8_t maxRetries;
    bool requireAcknowledgment;
    bool logFailures;
    uint16_t timeout;            // ms
};

// Fail signal data structure
struct FailSignalData {
    uint8_t signalId;
    FailSignalType type;
    FailSignalState state;
    FailSignalPriority priority;
    String description;
    String location;
    String component;
    unsigned long timestamp;
    unsigned long lastAlert;
    uint8_t alertCount;
    bool acknowledged;
    bool resolved;
    String resolutionNotes;
};

// Fail signal event types
enum class FailSignalEventType {
    FAILURE_DETECTED,
    ALERT_ACTIVATED,
    ALERT_DEACTIVATED,
    FAILURE_ACKNOWLEDGED,
    FAILURE_RESOLVED,
    EMERGENCY_SHUTDOWN,
    SYSTEM_RECOVERY,
    FAILURE_ESCALATED,
    FAILURE_TIMEOUT,
    MAINTENANCE_REQUIRED
};

// Fail signal event structure
struct FailSignalEvent {
    FailSignalEventType type;
    String description;
    unsigned long timestamp;
    uint8_t severity;
    bool acknowledged;
    uint8_t signalId;
    String operatorName;
};

// Alert configuration
struct AlertConfig {
    bool enableTone;
    bool enableVoice;
    bool enableVisual;
    bool enableVibration;
    uint16_t toneFrequency;
    uint16_t toneDuration;
    uint8_t visualPattern;
    uint8_t vibrationPattern;
    uint8_t volume;
    uint8_t brightness;
};

class ComputerFailSignalModule {
private:
    // Current state
    FailSignalState currentState;
    
    // Fail signal management
    static const uint8_t MAX_FAIL_SIGNALS = 32;
    FailSignalData failSignals[MAX_FAIL_SIGNALS];
    uint8_t signalCount;
    uint8_t activeSignalCount;
    
    // Event management
    static const uint8_t MAX_EVENTS = 64;
    FailSignalEvent events[MAX_EVENTS];
    uint8_t eventCount;
    uint8_t eventIndex;
    
    // Timing
    unsigned long lastUpdate;
    unsigned long lastAlertCheck;
    unsigned long failSignalStartTime;
    
    // Error tracking
    uint8_t errorCount;
    uint8_t warningCount;
    uint8_t criticalErrorCount;
    uint8_t emergencyCount;
    
    // Configuration
    FailSignalConfig config;
    AlertConfig alertConfig;
    bool failSignalInProgress;
    bool emergencyShutdownActive;
    
    // Alert management
    bool alertActive;
    unsigned long alertStartTime;
    uint8_t currentAlertId;
    uint8_t alertRetryCount;
    
    // Component interfaces
    AudibleSignalInterface* audibleInterface;
    VisualIndicatorsInterface* visualInterface;
    StateEvents* stateEvents;

public:
    ComputerFailSignalModule();
    ~ComputerFailSignalModule();
    
    // Initialization and control
    bool initialize();
    void startup();
    void shutdown();
    void reset();
    
    // State management
    FailSignalState getCurrentState() const { return currentState; }
    void changeState(FailSignalState newState);
    bool isEmergencyActive() const { return emergencyShutdownActive; }
    
    // Fail signal management
    uint8_t addFailSignal(FailSignalType type, const String& description, FailSignalPriority priority = FailSignalPriority::NORMAL);
    bool removeFailSignal(uint8_t signalId);
    bool updateFailSignal(uint8_t signalId, FailSignalState state, const String& notes = "");
    bool acknowledgeFailSignal(uint8_t signalId, const String& operatorName = "");
    bool resolveFailSignal(uint8_t signalId, const String& resolutionNotes = "");
    
    // Fail signal data access
    uint8_t getSignalCount() const { return signalCount; }
    uint8_t getActiveSignalCount() const { return activeSignalCount; }
    void getFailSignal(uint8_t index, FailSignalData& signal) const;
    bool isSignalActive(uint8_t signalId) const;
    bool isSignalAcknowledged(uint8_t signalId) const;
    bool isSignalResolved(uint8_t signalId) const;
    
    // Specific fail signal types
    bool addComputerFailure(const String& description, FailSignalPriority priority = FailSignalPriority::CRITICAL);
    bool addSensorFailure(const String& sensor, const String& description);
    bool addCommunicationFailure(const String& system, const String& description);
    bool addPowerFailure(const String& component, const String& description);
    bool addMemoryFailure(const String& description);
    bool addProcessorFailure(const String& description);
    bool addPeripheralFailure(const String& peripheral, const String& description);
    bool addSoftwareFailure(const String& module, const String& description);
    bool addHardwareFailure(const String& component, const String& description);
    bool addSystemFailure(const String& description);
    
    // Alert management
    void activateAlert(uint8_t signalId);
    void deactivateAlert(uint8_t signalId);
    bool isAlertActive() const { return alertActive; }
    void updateAlerts();
    void processAlertQueue();
    
    // Audible alerts
    void enableAudibleAlerts(bool enabled);
    void setToneFrequency(uint16_t frequency);
    void setToneDuration(uint16_t duration);
    void setVolume(uint8_t volume);
    void playAlertTone(uint8_t signalId);
    void playVoiceAlert(const String& message);
    void stopAudibleAlert();
    
    // Visual alerts
    void enableVisualAlerts(bool enabled);
    void setVisualPattern(uint8_t pattern);
    void setBrightness(uint8_t brightness);
    void activateVisualAlert(uint8_t signalId);
    void deactivateVisualAlert(uint8_t signalId);
    void setVisualIndicator(uint8_t indicatorId, bool state, uint8_t brightness = 255);
    
    // Vibration alerts
    void enableVibrationAlerts(bool enabled);
    void setVibrationPattern(uint8_t pattern);
    void activateVibrationAlert(uint8_t signalId);
    void deactivateVibrationAlert(uint8_t signalId);
    
    // Emergency handling
    void triggerEmergencyShutdown(const String& reason);
    void cancelEmergencyShutdown();
    bool isEmergencyShutdownActive() const { return emergencyShutdownActive; }
    void performEmergencyShutdown();
    void initiateSystemRecovery();
    
    // Configuration
    void setFailSignalConfiguration(const FailSignalConfig& config);
    void setAlertConfiguration(const AlertConfig& config);
    FailSignalConfig getFailSignalConfiguration() const { return config; }
    AlertConfig getAlertConfiguration() const { return alertConfig; }
    void setAlertDuration(uint16_t duration);
    void setRepeatInterval(uint16_t interval);
    void setMaxRetries(uint8_t retries);
    void setRequireAcknowledgment(bool required);
    
    // Error handling
    void addEvent(FailSignalEventType type, const String& description, uint8_t severity = 128);
    uint8_t getErrorCount() const { return errorCount; }
    uint8_t getWarningCount() const { return warningCount; }
    uint8_t getCriticalErrorCount() const { return criticalErrorCount; }
    uint8_t getEmergencyCount() const { return emergencyCount; }
    
    // Event management
    uint8_t getEventCount() const { return eventCount; }
    void getEvent(uint8_t index, FailSignalEvent& event) const;
    void acknowledgeEvent(uint8_t index);
    void clearEvents();
    
    // Component interfaces
    void setAudibleInterface(AudibleSignalInterface* interface) { audibleInterface = interface; }
    void setVisualInterface(VisualIndicatorsInterface* interface) { visualInterface = interface; }
    void setStateEvents(StateEvents* events) { stateEvents = events; }
    
    // Main update loop
    void update();
    
    // Diagnostics and testing
    void performDiagnostics();
    bool isFailSignalModuleHealthy() const;
    void runFailSignalTest();
    void testAudibleAlerts();
    void testVisualAlerts();
    void testVibrationAlerts();
    void simulateFailSignal(FailSignalType type, const String& description);
    void simulateEmergencyShutdown();
    
    // Utility functions
    bool validateFailSignalConfiguration(const FailSignalConfig& config) const;
    bool checkAlertTimeout() const;
    void updateFailSignalStatus();
    void logFailSignalData();
    
    // Status and information
    String getFailSignalStatus() const;
    void printFailSignalStatus();
    void printActiveSignals();
    void printEventLog();
    void exportFailSignalData(const String& filename);
    
    // Recovery and maintenance
    void performRecovery();
    void enterMaintenanceMode();
    void exitMaintenanceMode();
    void clearFailSignalHistory();
    void resetFailSignalCounters();
    
    // Statistical analysis
    uint8_t getFailuresByType(FailSignalType type) const;
    uint8_t getFailuresByPriority(FailSignalPriority priority) const;
    double getAverageResolutionTime() const;
    bool generateFailSignalReport(String& report);
    
    // Safety functions
    bool isSystemSafe() const;
    bool canContinueOperation() const;
    bool requiresImmediateAttention() const;
    void escalateFailSignal(uint8_t signalId);
    void deescalateFailSignal(uint8_t signalId);
};

#endif // COMPUTER_FAIL_SIGNAL_MODULE_H 