#ifndef POWER_UP_INITIALIZATION_H
#define POWER_UP_INITIALIZATION_H

#include <Arduino.h>
#include "../application_data_types/state_events.h"

// Initialization states
enum class InitializationState {
    POWER_OFF,
    STARTING,
    HARDWARE_INIT,
    SOFTWARE_INIT,
    SYSTEM_CHECK,
    CALIBRATION,
    READY,
    ERROR,
    FAILED
};

// Initialization phases
enum class InitializationPhase {
    POWER_ON_SEQUENCE,
    HARDWARE_DETECTION,
    FIRMWARE_LOAD,
    SYSTEM_CONFIGURATION,
    SENSOR_CALIBRATION,
    COMMUNICATION_SETUP,
    SAFETY_CHECK,
    OPERATIONAL_READY
};

// Initialization priority levels
enum class InitializationPriority {
    CRITICAL,
    HIGH,
    NORMAL,
    LOW,
    OPTIONAL
};

// Initialization result
enum class InitializationResult {
    SUCCESS,
    FAILURE,
    WARNING,
    PARTIAL,
    TIMEOUT
};

// Initialization configuration
struct InitializationConfig {
    bool enableHardwareCheck;
    bool enableSoftwareCheck;
    bool enableCalibration;
    bool enableCommunicationSetup;
    bool enableSafetyCheck;
    uint16_t timeout;            // seconds
    uint8_t retryCount;
    bool autoRecovery;
    bool requireOperatorConfirmation;
    uint16_t checkInterval;      // ms
};

// Initialization step structure
struct InitializationStep {
    uint8_t stepId;
    String description;
    InitializationPhase phase;
    InitializationPriority priority;
    uint16_t timeout;            // ms
    bool completed;
    InitializationResult result;
    String notes;
    bool required;
    unsigned long startTime;
    unsigned long endTime;
};

// Initialization data structure
struct InitializationData {
    uint8_t initId;
    InitializationState state;
    InitializationPhase currentPhase;
    InitializationResult result;
    unsigned long startTime;
    unsigned long endTime;
    uint16_t duration;
    uint8_t stepCount;
    uint8_t completedSteps;
    uint8_t failedSteps;
    uint8_t warningSteps;
    String description;
    String operatorName;
};

// Initialization event types
enum class InitializationEventType {
    INIT_STARTED,
    PHASE_STARTED,
    STEP_STARTED,
    STEP_COMPLETED,
    STEP_FAILED,
    PHASE_COMPLETED,
    CALIBRATION_STARTED,
    CALIBRATION_COMPLETED,
    SYSTEM_CHECK_STARTED,
    SYSTEM_CHECK_COMPLETED,
    INIT_COMPLETED,
    INIT_FAILED,
    INIT_TIMEOUT,
    RECOVERY_ATTEMPTED
};

// Initialization event structure
struct InitializationEvent {
    InitializationEventType type;
    String description;
    unsigned long timestamp;
    uint8_t severity;
    bool acknowledged;
    uint8_t initId;
    uint8_t stepId;
};

// Hardware initialization data
struct HardwareInitData {
    bool cpuInitialized;
    bool memoryInitialized;
    bool peripheralsInitialized;
    bool sensorsInitialized;
    bool actuatorsInitialized;
    bool communicationInitialized;
    bool powerSystemInitialized;
    bool safetySystemInitialized;
    uint8_t errorCount;
    String errorMessages[16];
};

// Software initialization data
struct SoftwareInitData {
    bool firmwareLoaded;
    bool configurationLoaded;
    bool modulesInitialized;
    bool communicationStackInitialized;
    bool databaseInitialized;
    bool securityInitialized;
    bool loggingInitialized;
    bool userInterfaceInitialized;
    uint8_t errorCount;
    String errorMessages[16];
};

class PowerUpInitialization {
private:
    // Current initialization state
    InitializationState currentState;
    InitializationPhase currentPhase;
    
    // Initialization management
    static const uint8_t MAX_INITIALIZATIONS = 8;
    InitializationData initializations[MAX_INITIALIZATIONS];
    uint8_t initCount;
    uint8_t currentInitId;
    
    // Initialization steps
    static const uint8_t MAX_STEPS = 64;
    InitializationStep steps[MAX_STEPS];
    uint8_t stepCount;
    uint8_t currentStepId;
    
    // Event management
    static const uint8_t MAX_EVENTS = 32;
    InitializationEvent events[MAX_EVENTS];
    uint8_t eventCount;
    uint8_t eventIndex;
    
    // Timing
    unsigned long lastUpdate;
    unsigned long lastCheck;
    unsigned long initStartTime;
    unsigned long stepStartTime;
    
    // Error tracking
    uint8_t errorCount;
    uint8_t warningCount;
    uint8_t criticalErrorCount;
    
    // Initialization configuration
    InitializationConfig config;
    bool initializationInProgress;
    bool operatorConfirmationRequired;
    bool operatorConfirmed;
    
    // Hardware and software initialization data
    HardwareInitData hardwareData;
    SoftwareInitData softwareData;
    
    // Recovery management
    bool recoveryInProgress;
    uint8_t recoveryAttempts;
    unsigned long recoveryStartTime;
    
    // Component interfaces
    StateEvents* stateEvents;

public:
    PowerUpInitialization();
    ~PowerUpInitialization();
    
    // Initialization and control
    bool initialize();
    void startup();
    void shutdown();
    void reset();
    
    // Initialization management
    bool startInitialization(const String& description = "");
    bool stopInitialization();
    bool pauseInitialization();
    bool resumeInitialization();
    bool abortInitialization();
    bool completeInitialization();
    
    // Initialization state management
    InitializationState getCurrentState() const { return currentState; }
    InitializationPhase getCurrentPhase() const { return currentPhase; }
    bool isInitializationInProgress() const { return initializationInProgress; }
    bool isInitializationCompleted() const;
    
    // Initialization configuration
    void setInitializationConfiguration(const InitializationConfig& config);
    InitializationConfig getInitializationConfiguration() const { return config; }
    void setInitializationTimeout(uint16_t timeout);
    void setRetryCount(uint8_t retryCount);
    void setAutoRecovery(bool enabled);
    void setOperatorConfirmationRequired(bool required);
    
    // Initialization step management
    bool addInitializationStep(const String& description, InitializationPhase phase, InitializationPriority priority, bool required = true);
    bool startCurrentStep();
    bool completeCurrentStep(InitializationResult result, const String& notes = "");
    bool failCurrentStep(const String& reason);
    uint8_t getStepCount() const { return stepCount; }
    uint8_t getCurrentStepId() const { return currentStepId; }
    void getStep(uint8_t index, InitializationStep& step) const;
    
    // Initialization data access
    uint8_t getInitializationCount() const { return initCount; }
    void getInitialization(uint8_t index, InitializationData& init) const;
    InitializationData getCurrentInitialization() const;
    InitializationResult getInitializationResult() const;
    uint16_t getInitializationDuration() const;
    uint8_t getCompletedSteps() const;
    uint8_t getFailedSteps() const;
    uint8_t getWarningSteps() const;
    
    // Specific initialization phases
    bool startPowerOnSequence();
    bool startHardwareDetection();
    bool startFirmwareLoad();
    bool startSystemConfiguration();
    bool startSensorCalibration();
    bool startCommunicationSetup();
    bool startSafetyCheck();
    bool startOperationalReady();
    
    // Hardware initialization
    bool initializeCPU();
    bool initializeMemory();
    bool initializePeripherals();
    bool initializeSensors();
    bool initializeActuators();
    bool initializeCommunication();
    bool initializePowerSystem();
    bool initializeSafetySystem();
    bool checkHardwareHealth();
    void getHardwareInitData(HardwareInitData& data) const;
    
    // Software initialization
    bool loadFirmware();
    bool loadConfiguration();
    bool initializeModules();
    bool initializeCommunicationStack();
    bool initializeDatabase();
    bool initializeSecurity();
    bool initializeLogging();
    bool initializeUserInterface();
    bool checkSoftwareHealth();
    void getSoftwareInitData(SoftwareInitData& data) const;
    
    // Calibration
    bool startCalibration();
    bool stopCalibration();
    bool isCalibrating() const;
    bool addCalibrationStep(const String& description);
    bool completeCalibrationStep(uint8_t stepId, bool success);
    double getCalibrationProgress() const;
    
    // System checks
    bool startSystemCheck();
    bool stopSystemCheck();
    bool isSystemChecking() const;
    bool addSystemCheckStep(const String& description);
    bool completeSystemCheckStep(uint8_t stepId, bool success);
    double getSystemCheckProgress() const;
    
    // Recovery
    bool startRecovery();
    bool stopRecovery();
    bool isRecovering() const { return recoveryInProgress; }
    uint8_t getRecoveryAttempts() const { return recoveryAttempts; }
    bool attemptRecovery();
    bool validateRecovery();
    
    // Operator interface
    bool requireOperatorConfirmation(bool required);
    bool confirmInitialization();
    bool isOperatorConfirmationRequired() const { return operatorConfirmationRequired; }
    bool isOperatorConfirmed() const { return operatorConfirmed; }
    
    // Error handling
    void addEvent(InitializationEventType type, const String& description, uint8_t severity = 128);
    uint8_t getErrorCount() const { return errorCount; }
    uint8_t getWarningCount() const { return warningCount; }
    uint8_t getCriticalErrorCount() const { return criticalErrorCount; }
    
    // Event management
    uint8_t getEventCount() const { return eventCount; }
    void getEvent(uint8_t index, InitializationEvent& event) const;
    void acknowledgeEvent(uint8_t index);
    void clearEvents();
    
    // Component interfaces
    void setStateEvents(StateEvents* events) { stateEvents = events; }
    
    // Main update loop
    void update();
    
    // Diagnostics and testing
    void performDiagnostics();
    bool isPowerUpInitializationHealthy() const;
    void runInitializationTest();
    void simulateInitializationFailure();
    void simulateInitializationTimeout();
    
    // Utility functions
    bool validateInitializationConfiguration(const InitializationConfig& config) const;
    bool checkInitializationTimeout() const;
    bool checkStepTimeout() const;
    void updateInitializationProgress();
    void logInitializationData();
    
    // Status and information
    String getInitializationStatus() const;
    double getInitializationProgress() const;
    double getPhaseProgress() const;
    double getStepProgress() const;
    void printInitializationStatus();
    void printInitializationResults();
    void printEventLog();
    void exportInitializationResults(const String& filename);
    
    // Recovery and maintenance
    void performRecovery();
    void enterMaintenanceMode();
    void exitMaintenanceMode();
    void clearInitializationHistory();
    void resetInitializationCounters();
    
    // Validation
    bool validateInitializationSequence() const;
    bool validateHardwareInitialization() const;
    bool validateSoftwareInitialization() const;
    bool validateSystemConfiguration() const;
    bool validateSafetySystems() const;
    
    // Reporting
    bool generateInitializationReport(String& report);
    bool generateHardwareReport(String& report);
    bool generateSoftwareReport(String& report);
    bool generateCalibrationReport(String& report);
    bool generateSystemCheckReport(String& report);
};

#endif // POWER_UP_INITIALIZATION_H 