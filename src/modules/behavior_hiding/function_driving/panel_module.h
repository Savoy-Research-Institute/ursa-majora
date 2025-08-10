#ifndef PANEL_MODULE_H
#define PANEL_MODULE_H

#include <Arduino.h>
#include "../../hardware_hiding/device_interface/panel_interface.h"
#include "../../software_decision/application_data_types/state_events.h"

// Panel states
enum class PanelState {
    POWER_OFF,
    INITIALIZING,
    ACTIVE,
    MAINTENANCE,
    ERROR_STATE,
    CALIBRATING
};

// Panel modes
enum class PanelMode {
    NORMAL_OPERATION,
    EMERGENCY_MODE,
    MAINTENANCE_MODE,
    TEST_MODE,
    CALIBRATION_MODE
};

// Input types
enum class InputType {
    BUTTON_PRESS,
    BUTTON_RELEASE,
    SWITCH_ON,
    SWITCH_OFF,
    ROTARY_INCREASE,
    ROTARY_DECREASE,
    TOUCH_PRESS,
    TOUCH_RELEASE,
    TOUCH_SWIPE,
    KEY_PRESS,
    KEY_RELEASE
};

// Input priority levels
enum class InputPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL,
    EMERGENCY
};

// Panel input structure
struct PanelInput {
    uint8_t inputId;
    InputType type;
    InputPriority priority;
    uint16_t value;
    unsigned long timestamp;
    bool processed;
    bool valid;
};

// Panel output structure
struct PanelOutput {
    uint8_t outputId;
    bool state;
    uint8_t brightness;
    uint16_t value;
    String text;
    unsigned long timestamp;
};

// Panel configuration
struct PanelConfig {
    bool enableDebouncing;
    bool enableInputValidation;
    bool enableOutputFeedback;
    uint16_t debounceTime;        // ms
    uint16_t inputTimeout;        // ms
    uint16_t outputUpdateRate;    // Hz
    uint8_t maxInputs;            // Maximum simultaneous inputs
    bool enableTouchInterface;
    bool enableKeyboardInterface;
    bool enableRotaryControls;
};

// Panel event types
enum class PanelEventType {
    INPUT_RECEIVED,
    INPUT_PROCESSED,
    INPUT_ERROR,
    OUTPUT_UPDATED,
    OUTPUT_ERROR,
    PANEL_CALIBRATED,
    TOUCH_CALIBRATED,
    KEYBOARD_CALIBRATED,
    PANEL_ERROR,
    MAINTENANCE_REQUIRED
};

// Panel event structure
struct PanelEvent {
    PanelEventType type;
    String description;
    unsigned long timestamp;
    uint8_t severity;
    bool acknowledged;
};

// Button configuration
struct ButtonConfig {
    uint8_t buttonId;
    String label;
    bool enabled;
    bool momentary;
    bool latching;
    uint16_t debounceTime;
    InputPriority priority;
};

// Switch configuration
struct SwitchConfig {
    uint8_t switchId;
    String label;
    bool enabled;
    uint8_t positions;
    String positionLabels[8];
    InputPriority priority;
};

// Rotary control configuration
struct RotaryConfig {
    uint8_t rotaryId;
    String label;
    bool enabled;
    int16_t minValue;
    int16_t maxValue;
    int16_t stepSize;
    int16_t currentValue;
    InputPriority priority;
};

class PanelModule {
private:
    // Current state and mode
    PanelState currentState;
    PanelMode currentMode;
    
    // Configuration
    PanelConfig config;
    
    // Input management
    static const uint8_t MAX_INPUTS = 32;
    PanelInput inputs[MAX_INPUTS];
    uint8_t inputCount;
    uint8_t inputIndex;
    
    // Output management
    static const uint8_t MAX_OUTPUTS = 32;
    PanelOutput outputs[MAX_OUTPUTS];
    uint8_t outputCount;
    
    // Event management
    static const uint8_t MAX_EVENTS = 16;
    PanelEvent events[MAX_EVENTS];
    uint8_t eventCount;
    uint8_t eventIndex;
    
    // Timing
    unsigned long lastUpdate;
    unsigned long lastInputCheck;
    unsigned long lastOutputUpdate;
    unsigned long panelStartTime;
    
    // Error tracking
    uint8_t errorCount;
    uint8_t warningCount;
    uint8_t criticalErrorCount;
    
    // Input processing
    bool inputProcessingEnabled;
    uint8_t processedInputCount;
    uint8_t pendingInputCount;
    
    // Debouncing
    unsigned long lastInputTime[MAX_INPUTS];
    bool inputDebounced[MAX_INPUTS];
    
    // Calibration
    bool calibrationInProgress;
    unsigned long calibrationStartTime;
    uint16_t calibrationSamples;
    
    // Component interfaces
    PanelInterface* panelInterface;
    StateEvents* stateEvents;

public:
    PanelModule();
    ~PanelModule();
    
    // Initialization and control
    bool initialize();
    void startup();
    void shutdown();
    void reset();
    
    // State management
    PanelState getCurrentState() const { return currentState; }
    PanelMode getCurrentMode() const { return currentMode; }
    bool setPanelMode(PanelMode mode);
    void changeState(PanelState newState);
    
    // Input processing
    bool processInput(uint8_t inputId, InputType type, uint16_t value = 0);
    bool addInput(uint8_t inputId, InputType type, InputPriority priority, uint16_t value = 0);
    uint8_t getInputCount() const { return inputCount; }
    void getInput(uint8_t index, PanelInput& input) const;
    bool isInputValid(uint8_t inputId) const;
    void clearInputs();
    
    // Output management
    bool updateOutput(uint8_t outputId, bool state, uint8_t brightness = 255);
    bool setOutputValue(uint8_t outputId, uint16_t value);
    bool setOutputText(uint8_t outputId, const String& text);
    uint8_t getOutputCount() const { return outputCount; }
    void getOutput(uint8_t index, PanelOutput& output) const;
    void clearOutputs();
    
    // Button management
    bool configureButton(const ButtonConfig& config);
    bool pressButton(uint8_t buttonId);
    bool releaseButton(uint8_t buttonId);
    bool isButtonPressed(uint8_t buttonId) const;
    
    // Switch management
    bool configureSwitch(const SwitchConfig& config);
    bool setSwitchPosition(uint8_t switchId, uint8_t position);
    uint8_t getSwitchPosition(uint8_t switchId) const;
    
    // Rotary control management
    bool configureRotary(const RotaryConfig& config);
    bool setRotaryValue(uint8_t rotaryId, int16_t value);
    int16_t getRotaryValue(uint8_t rotaryId) const;
    bool incrementRotary(uint8_t rotaryId);
    bool decrementRotary(uint8_t rotaryId);
    
    // Touch interface
    bool processTouchPress(uint8_t x, uint8_t y);
    bool processTouchRelease(uint8_t x, uint8_t y);
    bool processTouchSwipe(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY);
    
    // Keyboard interface
    bool processKeyPress(char key);
    bool processKeyRelease(char key);
    bool processKeySequence(const String& sequence);
    
    // Input validation
    bool validateInput(const PanelInput& input) const;
    bool validateButtonInput(uint8_t buttonId, InputType type) const;
    bool validateSwitchInput(uint8_t switchId, uint8_t position) const;
    bool validateRotaryInput(uint8_t rotaryId, int16_t value) const;
    
    // Debouncing
    void enableDebouncing(bool enabled);
    void setDebounceTime(uint16_t time);
    bool isInputDebounced(uint8_t inputId) const;
    void resetDebouncing(uint8_t inputId);
    
    // Error handling
    void addEvent(PanelEventType type, const String& description, uint8_t severity = 128);
    uint8_t getErrorCount() const { return errorCount; }
    uint8_t getWarningCount() const { return warningCount; }
    uint8_t getCriticalErrorCount() const { return criticalErrorCount; }
    
    // Event management
    uint8_t getEventCount() const { return eventCount; }
    void getEvent(uint8_t index, PanelEvent& event) const;
    void acknowledgeEvent(uint8_t index);
    void clearEvents();
    
    // Configuration
    void setConfiguration(const PanelConfig& newConfig);
    PanelConfig getConfiguration() const { return config; }
    void setInputTimeout(uint16_t timeout);
    void setOutputUpdateRate(uint16_t rate);
    void setMaxInputs(uint8_t max);
    
    // Component interfaces
    void setPanelInterface(PanelInterface* interface) { panelInterface = interface; }
    void setStateEvents(StateEvents* events) { stateEvents = events; }
    
    // Main update loop
    void update();
    
    // Diagnostics and testing
    void performDiagnostics();
    bool isPanelHealthy() const;
    void runPanelTest();
    void testButton(uint8_t buttonId);
    void testSwitch(uint8_t switchId);
    void testRotary(uint8_t rotaryId);
    void testTouchInterface();
    void testKeyboardInterface();
    
    // Calibration
    void startCalibration();
    void stopCalibration();
    bool isCalibrating() const { return calibrationInProgress; }
    bool addCalibrationSample(uint8_t inputId, uint16_t value);
    double getCalibrationProgress() const;
    
    // Utility functions
    bool isInputPending() const { return pendingInputCount > 0; }
    bool isOutputPending() const;
    void processPendingInputs();
    void updatePendingOutputs();
    
    // Status and information
    unsigned long getPanelUptime() const;
    String getPanelStatus() const;
    void printPanelStatus();
    void printInputLog();
    void printOutputLog();
    void printEventLog();
    
    // Recovery and maintenance
    void performRecovery();
    void enterMaintenanceMode();
    void exitMaintenanceMode();
    void resetAllInputs();
    void resetAllOutputs();
};

#endif // PANEL_MODULE_H 