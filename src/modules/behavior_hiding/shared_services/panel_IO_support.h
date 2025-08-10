#ifndef PANEL_IO_SUPPORT_H
#define PANEL_IO_SUPPORT_H

#include <Arduino.h>
#include "../../hardware_hiding/device_interface/panel_interface.h"
#include "../../software_decision/application_data_types/state_events.h"

// IO states
enum class IOState {
    IDLE,
    ACTIVE,
    BUSY,
    ERROR,
    MAINTENANCE
};

// IO types
enum class IOType {
    DIGITAL_INPUT,
    DIGITAL_OUTPUT,
    ANALOG_INPUT,
    ANALOG_OUTPUT,
    PWM_OUTPUT,
    SERIAL_INPUT,
    SERIAL_OUTPUT,
    I2C_INPUT,
    I2C_OUTPUT,
    SPI_INPUT,
    SPI_OUTPUT
};

// IO priority levels
enum class IOPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL,
    EMERGENCY
};

// IO configuration
struct IOConfig {
    uint8_t pinNumber;
    IOType type;
    IOPriority priority;
    bool enabled;
    bool inverted;
    uint16_t debounceTime;    // ms
    uint16_t timeout;         // ms
    uint8_t pullupResistor;   // Internal pullup
    uint16_t sampleRate;      // Hz
    uint8_t resolution;       // bits
};

// IO data structure
struct IOData {
    uint8_t ioId;
    IOType type;
    uint16_t value;
    uint16_t rawValue;
    bool state;
    bool valid;
    bool changed;
    unsigned long timestamp;
    unsigned long lastUpdate;
};

// IO event types
enum class IOEventType {
    INPUT_RECEIVED,
    OUTPUT_UPDATED,
    INPUT_ERROR,
    OUTPUT_ERROR,
    TIMEOUT_ERROR,
    COMMUNICATION_ERROR,
    CALIBRATION_COMPLETE,
    MAINTENANCE_REQUIRED
};

// IO event structure
struct IOEvent {
    IOEventType type;
    String description;
    unsigned long timestamp;
    uint8_t severity;
    bool acknowledged;
    uint8_t ioId;
};

// Digital IO configuration
struct DigitalIOConfig {
    uint8_t pinNumber;
    bool isInput;
    bool pullupEnabled;
    bool inverted;
    uint16_t debounceTime;
    IOPriority priority;
};

// Analog IO configuration
struct AnalogIOConfig {
    uint8_t pinNumber;
    bool isInput;
    uint8_t resolution;
    uint16_t referenceVoltage;
    uint16_t sampleRate;
    uint8_t averaging;
    IOPriority priority;
};

// PWM configuration
struct PWMConfig {
    uint8_t pinNumber;
    uint16_t frequency;
    uint8_t resolution;
    uint16_t dutyCycle;
    bool enabled;
    IOPriority priority;
};

// Serial configuration
struct SerialConfig {
    uint8_t serialPort;
    uint32_t baudRate;
    uint8_t dataBits;
    uint8_t stopBits;
    uint8_t parity;
    bool enabled;
    IOPriority priority;
};

class PanelIOSupport {
private:
    // Current state
    IOState currentState;
    
    // IO management
    static const uint8_t MAX_IO_CHANNELS = 64;
    IOData ioChannels[MAX_IO_CHANNELS];
    IOConfig ioConfigs[MAX_IO_CHANNELS];
    uint8_t ioCount;
    
    // Event management
    static const uint8_t MAX_EVENTS = 32;
    IOEvent events[MAX_EVENTS];
    uint8_t eventCount;
    uint8_t eventIndex;
    
    // Timing
    unsigned long lastUpdate;
    unsigned long lastCheck;
    unsigned long ioStartTime;
    
    // Error tracking
    uint8_t errorCount;
    uint8_t warningCount;
    uint8_t criticalErrorCount;
    
    // IO processing
    bool ioProcessingEnabled;
    uint8_t processedIOCount;
    uint8_t pendingIOCount;
    
    // Debouncing
    unsigned long lastInputTime[MAX_IO_CHANNELS];
    bool inputDebounced[MAX_IO_CHANNELS];
    uint16_t debounceCounters[MAX_IO_CHANNELS];
    
    // Calibration
    bool calibrationInProgress;
    unsigned long calibrationStartTime;
    uint16_t calibrationSamples;
    double calibrationData[MAX_IO_CHANNELS][100];
    
    // Component interfaces
    PanelInterface* panelInterface;
    StateEvents* stateEvents;

public:
    PanelIOSupport();
    ~PanelIOSupport();
    
    // Initialization and control
    bool initialize();
    void startup();
    void shutdown();
    void reset();
    
    // State management
    IOState getCurrentState() const { return currentState; }
    void changeState(IOState newState);
    
    // IO channel management
    uint8_t addIOChannel(const IOConfig& config);
    bool removeIOChannel(uint8_t ioId);
    bool enableIOChannel(uint8_t ioId, bool enabled);
    uint8_t getIOChannelCount() const { return ioCount; }
    void getIOChannel(uint8_t index, IOData& data) const;
    void getIOConfig(uint8_t index, IOConfig& config) const;
    
    // Digital IO
    uint8_t addDigitalInput(uint8_t pin, bool pullup = false, bool inverted = false);
    uint8_t addDigitalOutput(uint8_t pin, bool inverted = false);
    bool readDigitalInput(uint8_t ioId);
    bool writeDigitalOutput(uint8_t ioId, bool state);
    bool getDigitalState(uint8_t ioId) const;
    
    // Analog IO
    uint8_t addAnalogInput(uint8_t pin, uint8_t resolution = 10);
    uint8_t addAnalogOutput(uint8_t pin, uint8_t resolution = 8);
    uint16_t readAnalogInput(uint8_t ioId);
    bool writeAnalogOutput(uint8_t ioId, uint16_t value);
    uint16_t getAnalogValue(uint8_t ioId) const;
    
    // PWM IO
    uint8_t addPWMOutput(uint8_t pin, uint16_t frequency = 1000);
    bool setPWMDutyCycle(uint8_t ioId, uint16_t dutyCycle);
    bool setPWMFrequency(uint8_t ioId, uint16_t frequency);
    uint16_t getPWMDutyCycle(uint8_t ioId) const;
    uint16_t getPWMFrequency(uint8_t ioId) const;
    
    // Serial IO
    uint8_t addSerialInput(uint8_t port, uint32_t baudRate);
    uint8_t addSerialOutput(uint8_t port, uint32_t baudRate);
    bool writeSerialOutput(uint8_t ioId, const String& data);
    String readSerialInput(uint8_t ioId);
    bool isSerialAvailable(uint8_t ioId) const;
    
    // I2C IO
    uint8_t addI2CInput(uint8_t address);
    uint8_t addI2COutput(uint8_t address);
    bool writeI2COutput(uint8_t ioId, const uint8_t* data, uint8_t length);
    bool readI2CInput(uint8_t ioId, uint8_t* data, uint8_t length);
    
    // SPI IO
    uint8_t addSPIInput(uint8_t chipSelect);
    uint8_t addSPIOutput(uint8_t chipSelect);
    bool writeSPIOutput(uint8_t ioId, const uint8_t* data, uint8_t length);
    bool readSPIInput(uint8_t ioId, uint8_t* data, uint8_t length);
    
    // IO data access
    bool getIOData(uint8_t ioId, IOData& data) const;
    bool setIOData(uint8_t ioId, const IOData& data);
    bool isIOValid(uint8_t ioId) const;
    bool hasIOChanged(uint8_t ioId) const;
    void clearIOChange(uint8_t ioId);
    
    // IO configuration
    bool setIOConfig(uint8_t ioId, const IOConfig& config);
    bool getIOConfig(uint8_t ioId, IOConfig& config) const;
    bool setIOPriority(uint8_t ioId, IOPriority priority);
    bool setIODebounce(uint8_t ioId, uint16_t debounceTime);
    bool setIOTimeout(uint8_t ioId, uint16_t timeout);
    
    // Input processing
    void processInputs();
    void processDigitalInputs();
    void processAnalogInputs();
    void processSerialInputs();
    void processI2CInputs();
    void processSPIInputs();
    
    // Output processing
    void processOutputs();
    void processDigitalOutputs();
    void processAnalogOutputs();
    void processPWMOutputs();
    void processSerialOutputs();
    void processI2COutputs();
    void processSPIOutputs();
    
    // Debouncing
    void enableDebouncing(uint8_t ioId, bool enabled);
    void setDebounceTime(uint8_t ioId, uint16_t time);
    bool isInputDebounced(uint8_t ioId) const;
    void resetDebouncing(uint8_t ioId);
    
    // Error handling
    void addEvent(IOEventType type, const String& description, uint8_t severity = 128);
    uint8_t getErrorCount() const { return errorCount; }
    uint8_t getWarningCount() const { return warningCount; }
    uint8_t getCriticalErrorCount() const { return criticalErrorCount; }
    
    // Event management
    uint8_t getEventCount() const { return eventCount; }
    void getEvent(uint8_t index, IOEvent& event) const;
    void acknowledgeEvent(uint8_t index);
    void clearEvents();
    
    // Component interfaces
    void setPanelInterface(PanelInterface* interface) { panelInterface = interface; }
    void setStateEvents(StateEvents* events) { stateEvents = events; }
    
    // Main update loop
    void update();
    
    // Diagnostics and testing
    void performDiagnostics();
    bool isIOSupportHealthy() const;
    void runIOTest();
    void testDigitalIO();
    void testAnalogIO();
    void testPWMIO();
    void testSerialIO();
    void testI2CIO();
    void testSPIIO();
    
    // Calibration
    void startCalibration();
    void stopCalibration();
    bool isCalibrating() const { return calibrationInProgress; }
    bool addCalibrationSample(uint8_t ioId, double value);
    double getCalibrationProgress() const;
    
    // Utility functions
    bool validateIOConfig(const IOConfig& config) const;
    bool checkIOTimeout(uint8_t ioId) const;
    void updateIOStatus();
    void logIOData();
    
    // Status and information
    String getIOStatus() const;
    void printIOStatus();
    void printIOConfig();
    void printEventLog();
    void exportIOData(const String& filename);
    
    // Recovery and maintenance
    void performRecovery();
    void enterMaintenanceMode();
    void exitMaintenanceMode();
    void resetAllIO();
    void clearIOHistory();
};

#endif // PANEL_IO_SUPPORT_H 