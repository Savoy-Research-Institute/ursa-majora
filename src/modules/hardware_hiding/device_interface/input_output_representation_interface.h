#ifndef INPUT_OUTPUT_REPRESENTATION_INTERFACE_H
#define INPUT_OUTPUT_REPRESENTATION_INTERFACE_H

#include <Arduino.h>
#include "../../software_decision/application_data_types/numeric_types.h"

// IO states
enum class IORepresentationState {
    IDLE,
    ACTIVE,
    BUSY,
    ERROR,
    MAINTENANCE
};

// IO types
enum class IORepresentationType {
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
    SPI_OUTPUT,
    PULSE_INPUT,
    PULSE_OUTPUT,
    FREQUENCY_INPUT,
    FREQUENCY_OUTPUT
};

// IO representation modes
enum class IORepresentationMode {
    RAW,
    CALIBRATED,
    FILTERED,
    AVERAGED,
    SCALED,
    NORMALIZED
};

// IO configuration
struct IORepresentationConfig {
    uint8_t pinNumber;
    IORepresentationType type;
    IORepresentationMode mode;
    bool enabled;
    bool inverted;
    uint16_t debounceTime;    // ms
    uint16_t timeout;         // ms
    uint8_t resolution;       // bits
    uint16_t sampleRate;      // Hz
    uint8_t averaging;        // samples
    double scaleFactor;
    double offset;
    double minValue;
    double maxValue;
};

// IO data structure
struct IORepresentationData {
    uint8_t ioId;
    IORepresentationType type;
    IORepresentationMode mode;
    uint16_t rawValue;
    double processedValue;
    double scaledValue;
    bool state;
    bool valid;
    bool changed;
    unsigned long timestamp;
    unsigned long lastUpdate;
    uint8_t errorCount;
    String errorMessage;
};

// IO event types
enum class IORepresentationEventType {
    INPUT_RECEIVED,
    OUTPUT_UPDATED,
    CALIBRATION_COMPLETE,
    FILTER_UPDATED,
    SCALE_UPDATED,
    TIMEOUT_ERROR,
    COMMUNICATION_ERROR,
    VALIDATION_ERROR,
    MAINTENANCE_REQUIRED
};

// IO event structure
struct IORepresentationEvent {
    IORepresentationEventType type;
    String description;
    unsigned long timestamp;
    uint8_t severity;
    bool acknowledged;
    uint8_t ioId;
};

// Digital IO representation
struct DigitalIORepresentation {
    bool state;
    bool lastState;
    bool debouncedState;
    unsigned long lastChangeTime;
    uint16_t debounceCount;
    bool risingEdge;
    bool fallingEdge;
    uint32_t pulseCount;
    uint32_t pulseFrequency;
};

// Analog IO representation
struct AnalogIORepresentation {
    uint16_t rawValue;
    double voltage;
    double current;
    double resistance;
    double temperature;
    double pressure;
    double humidity;
    double calibratedValue;
    double filteredValue;
    double averagedValue;
    double minValue;
    double maxValue;
    double averageValue;
};

// PWM IO representation
struct PWMIORepresentation {
    uint16_t dutyCycle;
    uint16_t frequency;
    uint16_t period;
    double onTime;
    double offTime;
    bool enabled;
    uint8_t resolution;
    double voltage;
    double current;
    double power;
};

// Serial IO representation
struct SerialIORepresentation {
    String data;
    uint8_t buffer[256];
    uint8_t bufferIndex;
    bool dataAvailable;
    uint32_t baudRate;
    uint8_t dataBits;
    uint8_t stopBits;
    uint8_t parity;
    uint16_t timeout;
    uint8_t errorCount;
};

class InputOutputRepresentationInterface {
private:
    // Current state
    IORepresentationState currentState;
    
    // IO management
    static const uint8_t MAX_IO_CHANNELS = 64;
    IORepresentationData ioChannels[MAX_IO_CHANNELS];
    IORepresentationConfig ioConfigs[MAX_IO_CHANNELS];
    uint8_t ioCount;
    
    // Event management
    static const uint8_t MAX_EVENTS = 32;
    IORepresentationEvent events[MAX_EVENTS];
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
    
    // Digital IO representations
    DigitalIORepresentation digitalIO[MAX_IO_CHANNELS];
    
    // Analog IO representations
    AnalogIORepresentation analogIO[MAX_IO_CHANNELS];
    
    // PWM IO representations
    PWMIORepresentation pwmIO[MAX_IO_CHANNELS];
    
    // Serial IO representations
    SerialIORepresentation serialIO[MAX_IO_CHANNELS];
    
    // Calibration
    bool calibrationInProgress;
    unsigned long calibrationStartTime;
    uint16_t calibrationSamples;
    double calibrationData[MAX_IO_CHANNELS][100];
    
    // Component interfaces
    NumericTypes* numericTypes;

public:
    InputOutputRepresentationInterface();
    ~InputOutputRepresentationInterface();
    
    // Initialization and control
    bool initialize();
    void startup();
    void shutdown();
    void reset();
    
    // State management
    IORepresentationState getCurrentState() const { return currentState; }
    void changeState(IORepresentationState newState);
    
    // IO channel management
    uint8_t addIOChannel(const IORepresentationConfig& config);
    bool removeIOChannel(uint8_t ioId);
    bool enableIOChannel(uint8_t ioId, bool enabled);
    uint8_t getIOChannelCount() const { return ioCount; }
    void getIOChannel(uint8_t index, IORepresentationData& data) const;
    void getIOConfig(uint8_t index, IORepresentationConfig& config) const;
    
    // Digital IO representation
    uint8_t addDigitalInputRepresentation(uint8_t pin, bool pullup = false, bool inverted = false);
    uint8_t addDigitalOutputRepresentation(uint8_t pin, bool inverted = false);
    bool readDigitalInputRepresentation(uint8_t ioId);
    bool writeDigitalOutputRepresentation(uint8_t ioId, bool state);
    bool getDigitalStateRepresentation(uint8_t ioId) const;
    bool getDigitalRisingEdge(uint8_t ioId) const;
    bool getDigitalFallingEdge(uint8_t ioId) const;
    uint32_t getDigitalPulseCount(uint8_t ioId) const;
    uint32_t getDigitalPulseFrequency(uint8_t ioId) const;
    
    // Analog IO representation
    uint8_t addAnalogInputRepresentation(uint8_t pin, uint8_t resolution = 10);
    uint8_t addAnalogOutputRepresentation(uint8_t pin, uint8_t resolution = 8);
    uint16_t readAnalogInputRepresentation(uint8_t ioId);
    bool writeAnalogOutputRepresentation(uint8_t ioId, uint16_t value);
    uint16_t getAnalogRawValue(uint8_t ioId) const;
    double getAnalogVoltage(uint8_t ioId) const;
    double getAnalogCurrent(uint8_t ioId) const;
    double getAnalogResistance(uint8_t ioId) const;
    double getAnalogTemperature(uint8_t ioId) const;
    double getAnalogPressure(uint8_t ioId) const;
    double getAnalogHumidity(uint8_t ioId) const;
    double getAnalogCalibratedValue(uint8_t ioId) const;
    double getAnalogFilteredValue(uint8_t ioId) const;
    double getAnalogAveragedValue(uint8_t ioId) const;
    
    // PWM IO representation
    uint8_t addPWMOutputRepresentation(uint8_t pin, uint16_t frequency = 1000);
    bool setPWMDutyCycleRepresentation(uint8_t ioId, uint16_t dutyCycle);
    bool setPWMFrequencyRepresentation(uint8_t ioId, uint16_t frequency);
    uint16_t getPWMDutyCycleRepresentation(uint8_t ioId) const;
    uint16_t getPWMFrequencyRepresentation(uint8_t ioId) const;
    uint16_t getPWMPeriodRepresentation(uint8_t ioId) const;
    double getPWMOnTimeRepresentation(uint8_t ioId) const;
    double getPWMOffTimeRepresentation(uint8_t ioId) const;
    double getPWMVoltageRepresentation(uint8_t ioId) const;
    double getPWMCurrentRepresentation(uint8_t ioId) const;
    double getPWMPowerRepresentation(uint8_t ioId) const;
    
    // Serial IO representation
    uint8_t addSerialInputRepresentation(uint8_t port, uint32_t baudRate);
    uint8_t addSerialOutputRepresentation(uint8_t port, uint32_t baudRate);
    bool writeSerialOutputRepresentation(uint8_t ioId, const String& data);
    String readSerialInputRepresentation(uint8_t ioId);
    bool isSerialAvailableRepresentation(uint8_t ioId) const;
    uint8_t getSerialErrorCount(uint8_t ioId) const;
    
    // IO data access
    bool getIOData(uint8_t ioId, IORepresentationData& data) const;
    bool setIOData(uint8_t ioId, const IORepresentationData& data);
    bool isIOValid(uint8_t ioId) const;
    bool hasIOChanged(uint8_t ioId) const;
    void clearIOChange(uint8_t ioId);
    
    // IO configuration
    bool setIOConfig(uint8_t ioId, const IORepresentationConfig& config);
    bool getIOConfig(uint8_t ioId, IORepresentationConfig& config) const;
    bool setIOMode(uint8_t ioId, IORepresentationMode mode);
    bool setIOScaleFactor(uint8_t ioId, double scaleFactor);
    bool setIOOffset(uint8_t ioId, double offset);
    bool setIORange(uint8_t ioId, double minValue, double maxValue);
    
    // Data processing
    void processIOData();
    void processDigitalIO();
    void processAnalogIO();
    void processPWMIO();
    void processSerialIO();
    
    // Calibration
    void startCalibration(uint8_t ioId);
    void stopCalibration();
    bool isCalibrating() const { return calibrationInProgress; }
    bool addCalibrationSample(uint8_t ioId, double value);
    double getCalibrationProgress() const;
    bool calibrateIO(uint8_t ioId, double knownValue);
    
    // Filtering and averaging
    void enableAveraging(uint8_t ioId, bool enabled, uint8_t samples = 10);
    void enableFiltering(uint8_t ioId, bool enabled);
    double getAveragedValue(uint8_t ioId) const;
    double getFilteredValue(uint8_t ioId) const;
    
    // Scaling and normalization
    void setScaleFactor(uint8_t ioId, double scaleFactor);
    void setOffset(uint8_t ioId, double offset);
    double getScaledValue(uint8_t ioId) const;
    double getNormalizedValue(uint8_t ioId) const;
    
    // Error handling
    void addEvent(IORepresentationEventType type, const String& description, uint8_t severity = 128);
    uint8_t getErrorCount() const { return errorCount; }
    uint8_t getWarningCount() const { return warningCount; }
    uint8_t getCriticalErrorCount() const { return criticalErrorCount; }
    
    // Event management
    uint8_t getEventCount() const { return eventCount; }
    void getEvent(uint8_t index, IORepresentationEvent& event) const;
    void acknowledgeEvent(uint8_t index);
    void clearEvents();
    
    // Component interfaces
    void setNumericTypes(NumericTypes* types) { numericTypes = types; }
    
    // Main update loop
    void update();
    
    // Diagnostics and testing
    void performDiagnostics();
    bool isIORepresentationHealthy() const;
    void runIOTest();
    void testDigitalIORepresentation();
    void testAnalogIORepresentation();
    void testPWMIORepresentation();
    void testSerialIORepresentation();
    
    // Utility functions
    bool validateIOConfig(const IORepresentationConfig& config) const;
    bool checkIOTimeout(uint8_t ioId) const;
    void updateIOStatus();
    void logIOData();
    
    // Status and information
    String getIORepresentationStatus() const;
    void printIORepresentationStatus();
    void printIOConfig();
    void printEventLog();
    void exportIORepresentationData(const String& filename);
    
    // Recovery and maintenance
    void performRecovery();
    void enterMaintenanceMode();
    void exitMaintenanceMode();
    void resetAllIO();
    void clearIOHistory();
};

#endif // INPUT_OUTPUT_REPRESENTATION_INTERFACE_H 