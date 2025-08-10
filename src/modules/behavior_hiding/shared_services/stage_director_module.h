#ifndef STAGE_DIRECTOR_MODULE_H
#define STAGE_DIRECTOR_MODULE_H

#include <Arduino.h>
#include "../../software_decision/application_data_types/state_events.h"

// Stage states
enum class StageState {
    IDLE,
    INITIALIZING,
    ACTIVE,
    TRANSITIONING,
    PAUSED,
    COMPLETED,
    ERROR,
    ABORTED
};

// Stage types
enum class StageType {
    SYSTEM_STARTUP,
    PREFLIGHT_CHECK,
    TAKEOFF_SEQUENCE,
    FLIGHT_OPERATION,
    LANDING_SEQUENCE,
    POSTFLIGHT_CHECK,
    MAINTENANCE_MODE,
    EMERGENCY_SEQUENCE,
    CALIBRATION_SEQUENCE,
    TEST_SEQUENCE
};

// Stage priority levels
enum class StagePriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL,
    EMERGENCY
};

// Stage result
enum class StageResult {
    SUCCESS,
    FAILURE,
    WARNING,
    INCOMPLETE,
    ABORTED
};

// Stage configuration
struct StageConfig {
    StageType type;
    StagePriority priority;
    uint16_t duration;           // seconds
    uint16_t timeout;            // seconds
    bool autoStart;
    bool autoComplete;
    bool requireConfirmation;
    bool allowInterruption;
    uint8_t retryCount;
    uint16_t checkInterval;      // ms
};

// Stage step structure
struct StageStep {
    uint8_t stepId;
    String description;
    uint16_t duration;           // ms
    bool completed;
    StageResult result;
    String notes;
    bool required;
    bool confirmed;
    unsigned long startTime;
    unsigned long endTime;
};

// Stage data structure
struct StageData {
    uint8_t stageId;
    StageType type;
    StageState state;
    StageResult result;
    unsigned long startTime;
    unsigned long endTime;
    uint16_t duration;
    uint8_t stepCount;
    uint8_t completedSteps;
    uint8_t requiredSteps;
    uint8_t confirmedSteps;
    String description;
    String operatorName;
};

// Stage event types
enum class StageEventType {
    STAGE_STARTED,
    STAGE_STEP_STARTED,
    STAGE_STEP_COMPLETED,
    STAGE_STEP_FAILED,
    STAGE_PAUSED,
    STAGE_RESUMED,
    STAGE_COMPLETED,
    STAGE_FAILED,
    STAGE_ABORTED,
    STAGE_TIMEOUT,
    OPERATOR_CONFIRMATION,
    INTERRUPTION_REQUESTED
};

// Stage event structure
struct StageEvent {
    StageEventType type;
    String description;
    unsigned long timestamp;
    uint8_t severity;
    bool acknowledged;
    uint8_t stageId;
    uint8_t stepId;
};

// Stage transition rules
struct StageTransition {
    StageType fromStage;
    StageType toStage;
    bool allowed;
    String condition;
    bool requireConfirmation;
    uint8_t priority;
};

class StageDirectorModule {
private:
    // Current stage state
    StageState currentState;
    StageType currentStageType;
    
    // Stage management
    static const uint8_t MAX_STAGES = 16;
    StageData stages[MAX_STAGES];
    uint8_t stageCount;
    uint8_t currentStageId;
    
    // Stage steps
    static const uint8_t MAX_STEPS = 32;
    StageStep steps[MAX_STEPS];
    uint8_t stepCount;
    uint8_t currentStepId;
    
    // Event management
    static const uint8_t MAX_EVENTS = 32;
    StageEvent events[MAX_EVENTS];
    uint8_t eventCount;
    uint8_t eventIndex;
    
    // Timing
    unsigned long lastUpdate;
    unsigned long lastCheck;
    unsigned long stageStartTime;
    unsigned long stepStartTime;
    
    // Error tracking
    uint8_t errorCount;
    uint8_t warningCount;
    uint8_t criticalErrorCount;
    
    // Stage configuration
    StageConfig currentConfig;
    bool stageInProgress;
    bool confirmationRequired;
    bool confirmationReceived;
    bool interruptionRequested;
    
    // Stage transitions
    static const uint8_t MAX_TRANSITIONS = 32;
    StageTransition transitions[MAX_TRANSITIONS];
    uint8_t transitionCount;
    
    // Component interfaces
    StateEvents* stateEvents;

public:
    StageDirectorModule();
    ~StageDirectorModule();
    
    // Initialization and control
    bool initialize();
    void startup();
    void shutdown();
    void reset();
    
    // Stage management
    bool startStage(StageType type, const String& description = "");
    bool stopStage();
    bool pauseStage();
    bool resumeStage();
    bool abortStage();
    bool completeStage();
    
    // Stage state management
    StageState getCurrentState() const { return currentState; }
    StageType getCurrentStageType() const { return currentStageType; }
    bool isStageInProgress() const { return stageInProgress; }
    bool isStageCompleted() const;
    
    // Stage configuration
    void setStageConfiguration(const StageConfig& config);
    StageConfig getStageConfiguration() const { return currentConfig; }
    void setStageDuration(uint16_t duration);
    void setStageTimeout(uint16_t timeout);
    void setStagePriority(StagePriority priority);
    void setAutoStart(bool enabled);
    void setAutoComplete(bool enabled);
    void setConfirmationRequired(bool required);
    void setAllowInterruption(bool allowed);
    
    // Stage step management
    bool addStageStep(const String& description, bool required = true, uint16_t duration = 0);
    bool startCurrentStep();
    bool completeCurrentStep(StageResult result, const String& notes = "");
    bool failCurrentStep(const String& reason);
    bool confirmCurrentStep();
    uint8_t getStepCount() const { return stepCount; }
    uint8_t getCurrentStepId() const { return currentStepId; }
    void getStep(uint8_t index, StageStep& step) const;
    
    // Stage data access
    uint8_t getStageCount() const { return stageCount; }
    void getStage(uint8_t index, StageData& stage) const;
    StageData getCurrentStage() const;
    StageResult getStageResult() const;
    uint16_t getStageDuration() const;
    uint8_t getCompletedSteps() const;
    uint8_t getRequiredSteps() const;
    uint8_t getConfirmedSteps() const;
    
    // Specific stage types
    bool startSystemStartup();
    bool startPreflightCheck();
    bool startTakeoffSequence();
    bool startFlightOperation();
    bool startLandingSequence();
    bool startPostflightCheck();
    bool startMaintenanceMode();
    bool startEmergencySequence();
    bool startCalibrationSequence();
    bool startTestSequence();
    
    // Stage transitions
    bool addStageTransition(StageType fromStage, StageType toStage, bool allowed = true);
    bool removeStageTransition(StageType fromStage, StageType toStage);
    bool isTransitionAllowed(StageType fromStage, StageType toStage) const;
    bool requestStageTransition(StageType toStage);
    bool validateStageTransition(StageType toStage) const;
    
    // Operator interface
    bool requireConfirmation(bool required);
    bool confirmStage();
    bool confirmStep();
    bool requestInterruption();
    bool isConfirmationRequired() const { return confirmationRequired; }
    bool isConfirmationReceived() const { return confirmationReceived; }
    bool isInterruptionRequested() const { return interruptionRequested; }
    
    // Error handling
    void addEvent(StageEventType type, const String& description, uint8_t severity = 128);
    uint8_t getErrorCount() const { return errorCount; }
    uint8_t getWarningCount() const { return warningCount; }
    uint8_t getCriticalErrorCount() const { return criticalErrorCount; }
    
    // Event management
    uint8_t getEventCount() const { return eventCount; }
    void getEvent(uint8_t index, StageEvent& event) const;
    void acknowledgeEvent(uint8_t index);
    void clearEvents();
    
    // Component interfaces
    void setStateEvents(StateEvents* events) { stateEvents = events; }
    
    // Main update loop
    void update();
    
    // Diagnostics and testing
    void performDiagnostics();
    bool isStageDirectorHealthy() const;
    void runStageTest();
    void simulateStageFailure();
    void simulateStageTimeout();
    
    // Utility functions
    bool validateStageConfiguration(const StageConfig& config) const;
    bool checkStageTimeout() const;
    bool checkStepTimeout() const;
    void updateStageProgress();
    void logStageData();
    
    // Status and information
    String getStageStatus() const;
    double getStageProgress() const;
    double getStepProgress() const;
    void printStageStatus();
    void printStageResults();
    void printEventLog();
    void exportStageResults(const String& filename);
    
    // Recovery and maintenance
    void performRecovery();
    void enterMaintenanceMode();
    void exitMaintenanceMode();
    void clearStageHistory();
    void resetStageCounters();
    
    // Stage scheduling
    bool scheduleStage(StageType type, uint16_t delay = 0);
    bool cancelScheduledStage(StageType type);
    bool isStageScheduled(StageType type) const;
    
    // Stage dependencies
    bool addStageDependency(StageType dependentStage, StageType prerequisiteStage);
    bool removeStageDependency(StageType dependentStage, StageType prerequisiteStage);
    bool checkStageDependencies(StageType stage) const;
    
    // Stage validation
    bool validateStageSequence() const;
    bool validateStageTransitions() const;
    bool validateStageDependencies() const;
};

#endif // STAGE_DIRECTOR_MODULE_H 