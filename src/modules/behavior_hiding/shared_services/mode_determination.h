#ifndef MODE_DETERMINATION_H
#define MODE_DETERMINATION_H

#include <Arduino.h>

// System modes
enum class SystemModeEnum {
    UNKNOWN,
    GROUND_TEST,
    PREFLIGHT,
    TAKEOFF,
    CRUISE,
    MANEUVER,
    APPROACH,
    LANDING,
    POSTFLIGHT,
    MAINTENANCE,
    EMERGENCY,
    RECOVERY
};

// Mode transition triggers
enum class ModeTransitionTrigger {
    AUTOMATIC,
    PILOT_INPUT,
    SYSTEM_DETECTION,
    EMERGENCY_SIGNAL,
    CALIBRATION_COMPLETE,
    ERROR_CONDITION,
    RECOVERY_ATTEMPT
};

// Mode validation result
enum class ModeValidationResult {
    VALID,
    INVALID_CONDITIONS,
    INVALID_STATE,
    INVALID_TRANSITION,
    REQUIRES_CALIBRATION,
    REQUIRES_MAINTENANCE
};

// Mode configuration
struct ModeConfig {
    SystemModeEnum mode;
    bool enabled;
    uint8_t priority;
    uint32_t minDuration;
    uint32_t maxDuration;
    bool requiresCalibration;
    bool requiresMaintenance;
    String description;
};

// Mode transition rule
struct ModeTransitionRule {
    SystemModeEnum fromMode;
    SystemModeEnum toMode;
    ModeTransitionTrigger trigger;
    bool automatic;
    uint32_t delayMs;
    String conditions;
    String description;
};

// Mode status
struct ModeStatus {
    SystemModeEnum currentMode;
    SystemModeEnum previousMode;
    unsigned long modeStartTime;
    unsigned long lastTransitionTime;
    uint32_t modeDuration;
    ModeTransitionTrigger lastTrigger;
    bool modeValid;
    String currentConditions;
};

class ModeDetermination {
private:
    // Current mode status
    ModeStatus status;
    
    // Mode configurations
    static const uint8_t MAX_MODES = 16;
    ModeConfig modeConfigs[MAX_MODES];
    uint8_t modeCount;
    
    // Transition rules
    static const uint8_t MAX_TRANSITIONS = 32;
    ModeTransitionRule transitions[MAX_TRANSITIONS];
    uint8_t transitionCount;
    
    // Mode validation
    bool autoModeSelection;
    bool modeValidationEnabled;
    uint32_t modeCheckInterval;
    unsigned long lastModeCheck;
    
    // Mode history
    static const uint8_t MAX_MODE_HISTORY = 16;
    SystemModeEnum modeHistory[MAX_MODE_HISTORY];
    uint8_t historyIndex;
    uint8_t historyCount;
    
    // Private methods
    bool validateModeTransition(SystemModeEnum fromMode, SystemModeEnum toMode, ModeTransitionTrigger trigger);
    void executeModeTransition(SystemModeEnum newMode, ModeTransitionTrigger trigger);
    void updateModeStatus();
    bool checkModeConditions(SystemModeEnum mode);
    void addModeToHistory(SystemModeEnum mode);
    ModeValidationResult validateMode(SystemModeEnum mode);
    bool isTransitionAllowed(SystemModeEnum fromMode, SystemModeEnum toMode);
    
public:
    ModeDetermination();
    
    // Mode management
    SystemModeEnum getCurrentMode() const { return status.currentMode; }
    SystemModeEnum getPreviousMode() const { return status.previousMode; }
    ModeStatus getModeStatus() const { return status; }
    
    // Mode transitions
    bool setMode(SystemModeEnum newMode, ModeTransitionTrigger trigger = ModeTransitionTrigger::PILOT_INPUT);
    bool requestModeChange(SystemModeEnum newMode);
    void processModeChanges();
    
    // Mode configuration
    bool addMode(SystemModeEnum mode, const String& description = "");
    bool removeMode(SystemModeEnum mode);
    bool enableMode(SystemModeEnum mode);
    bool disableMode(SystemModeEnum mode);
    void setModePriority(SystemModeEnum mode, uint8_t priority);
    void setModeDuration(SystemModeEnum mode, uint32_t minDuration, uint32_t maxDuration);
    
    // Transition rules
    bool addTransitionRule(SystemModeEnum fromMode, SystemModeEnum toMode, 
                          ModeTransitionTrigger trigger, bool automatic = false, 
                          uint32_t delayMs = 0, const String& conditions = "");
    bool removeTransitionRule(uint8_t ruleIndex);
    void setTransitionDelay(SystemModeEnum fromMode, SystemModeEnum toMode, uint32_t delayMs);
    
    // Mode validation
    ModeValidationResult validateCurrentMode();
    bool isModeValid(SystemModeEnum mode) const;
    bool canTransitionTo(SystemModeEnum mode) const;
    String getModeConditions(SystemModeEnum mode) const;
    
    // Automatic mode selection
    void enableAutoModeSelection(bool enabled);
    bool isAutoModeSelectionEnabled() const { return autoModeSelection; }
    SystemModeEnum determineOptimalMode();
    void performAutomaticModeSelection();
    
    // Mode timing
    uint32_t getModeDuration() const { return status.modeDuration; }
    unsigned long getModeStartTime() const { return status.modeStartTime; }
    bool isModeTimedOut(SystemModeEnum mode) const;
    
    // Mode history
    uint8_t getModeHistoryCount() const { return historyCount; }
    SystemModeEnum getModeFromHistory(uint8_t index) const;
    void clearModeHistory();
    
    // Mode information
    String getModeDescription(SystemModeEnum mode) const;
    void getModeInfo(SystemModeEnum mode, String& info);
    void printModeStatus();
    void printTransitionRules();
    
    // System interface
    bool initialize();
    void update();
    void shutdown();
    
    // Configuration
    void setModeValidation(bool enabled);
    void setModeCheckInterval(uint32_t interval);
    void setDefaultMode(SystemModeEnum mode);
    
    // Diagnostic functions
    void runSelfTest();
    void printDiagnostics();
    void validateAllModes();
    
    // Utility functions
    String modeToString(SystemModeEnum mode) const;
    String triggerToString(ModeTransitionTrigger trigger) const;
    String validationResultToString(ModeValidationResult result) const;
    
    // Mode analysis
    bool isGroundMode(SystemModeEnum mode) const;
    bool isFlightMode(SystemModeEnum mode) const;
    bool isEmergencyMode(SystemModeEnum mode) const;
    bool isMaintenanceMode(SystemModeEnum mode) const;
    
    // Testing functions
    void simulateModeTransition(SystemModeEnum newMode);
    void testModeValidation();
    void testTransitionRules();
};

#endif // MODE_DETERMINATION_H 