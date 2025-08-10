#ifndef PANEL_INTERFACE_H
#define PANEL_INTERFACE_H

#include <Arduino.h>

// Panel device types
enum class PanelDeviceType {
    SWITCH,
    BUTTON,
    ROTARY_ENCODER,
    POTENTIOMETER,
    DISPLAY,
    LED_INDICATOR,
    KEYPAD,
    TOUCH_SCREEN
};

// Switch types
enum class SwitchType {
    TOGGLE,
    MOMENTARY,
    ROCKER,
    ROTARY,
    SLIDE,
    PUSH_BUTTON
};

// Switch states
enum class SwitchState {
    OFF,
    ON,
    POSITION_1,
    POSITION_2,
    POSITION_3,
    POSITION_4,
    POSITION_5
};

// Display types
enum class DisplayType {
    LCD,
    OLED,
    LED_SEGMENT,
    VFD,
    TFT,
    E_PAPER
};

// Panel configuration
struct PanelConfig {
    uint8_t maxDevices;      // Maximum number of devices
    uint8_t maxSwitches;     // Maximum number of switches
    uint8_t maxDisplays;     // Maximum number of displays
    uint8_t maxIndicators;   // Maximum number of indicators
    bool backlightEnabled;    // Backlight control enabled
    uint8_t brightness;      // Display brightness (0-100)
    bool autoDim;            // Auto-dimming enabled
    uint16_t dimTimeout;     // Dimming timeout in milliseconds
};

// Device configuration
struct DeviceConfig {
    PanelDeviceType type;
    uint8_t pin;             // Hardware pin
    uint8_t id;              // Device identifier
    String label;             // Device label
    bool enabled;             // Whether device is enabled
    uint8_t position;        // Position on panel
    uint8_t group;           // Device group
};

// Switch configuration
struct SwitchConfig {
    SwitchType type;
    uint8_t pin;             // Hardware pin
    uint8_t id;              // Switch identifier
    String label;             // Switch label
    SwitchState initialState; // Initial state
    bool momentary;           // Whether switch is momentary
    bool inverted;            // Whether logic is inverted
    uint8_t debounceTime;    // Debounce time in milliseconds
};

// Display configuration
struct DisplayConfig {
    DisplayType type;
    uint8_t pin;             // Hardware pin
    uint8_t id;              // Display identifier
    String label;             // Display label
    uint8_t width;           // Display width in characters/pixels
    uint8_t height;          // Display height in characters/pixels
    bool backlight;           // Backlight enabled
    uint8_t contrast;        // Contrast level (0-100)
};

// Panel status
struct PanelStatus {
    bool initialized;         // Whether panel is initialized
    uint8_t deviceCount;     // Number of devices
    uint8_t switchCount;     // Number of switches
    uint8_t displayCount;    // Number of displays
    uint8_t errorFlags;      // Error status flags
    uint16_t uptime;         // Panel uptime in seconds
    bool healthy;            // Overall panel health
};

class PanelInterface {
private:
    static const uint8_t MAX_DEVICES = 32;
    static const uint8_t MAX_SWITCHES = 16;
    static const uint8_t MAX_DISPLAYS = 8;
    static const uint8_t MAX_INDICATORS = 16;
    
    PanelConfig config;
    PanelStatus status;
    
    // Device management
    DeviceConfig devices[MAX_DEVICES];
    uint8_t deviceCount;
    
    // Switch management
    SwitchConfig switches[MAX_SWITCHES];
    SwitchState switchStates[MAX_SWITCHES];
    uint8_t switchCount;
    
    // Display management
    DisplayConfig displays[MAX_DISPLAYS];
    uint8_t displayCount;
    
    // LED indicators
    uint8_t indicatorPins[MAX_INDICATORS];
    bool indicatorStates[MAX_INDICATORS];
    uint8_t indicatorCount;
    
    // Timing and intervals
    unsigned long lastUpdateTime;
    unsigned long lastDimTime;
    unsigned long lastDebounceTime;
    
    // Error handling
    uint8_t errorFlags;
    String lastError;
    
    // Private methods
    bool initializeHardware();
    bool initializeDevice(const DeviceConfig& device);
    bool initializeSwitch(const SwitchConfig& sw);
    bool initializeDisplay(const DisplayConfig& display);
    void updateSwitches();
    void updateDisplays();
    void updateIndicators();
    bool validateDeviceConfig(const DeviceConfig& device);
    bool validateSwitchConfig(const SwitchConfig& sw);
    bool validateDisplayConfig(const DisplayConfig& display);
    
public:
    PanelInterface();
    
    // Configuration
    bool configure(const PanelConfig& newConfig);
    PanelConfig getConfiguration() const { return config; }
    bool setMaxDevices(uint8_t maxDev);
    bool setMaxSwitches(uint8_t maxSw);
    bool setMaxDisplays(uint8_t maxDisp);
    bool setBacklightEnabled(bool enabled);
    bool setBrightness(uint8_t brightness);
    bool setAutoDim(bool enabled);
    bool setDimTimeout(uint16_t timeout);
    
    // Device management
    bool addDevice(const DeviceConfig& device);
    bool removeDevice(uint8_t deviceId);
    bool enableDevice(uint8_t deviceId);
    bool disableDevice(uint8_t deviceId);
    uint8_t getDeviceCount() const { return deviceCount; }
    bool getDevice(uint8_t deviceId, DeviceConfig& device) const;
    
    // Switch management
    bool addSwitch(const SwitchConfig& sw);
    bool removeSwitch(uint8_t switchId);
    SwitchState getSwitchState(uint8_t switchId) const;
    bool setSwitchState(uint8_t switchId, SwitchState state);
    bool isSwitchOn(uint8_t switchId) const;
    uint8_t getSwitchCount() const { return switchCount; }
    bool getSwitch(uint8_t switchId, SwitchConfig& sw) const;
    
    // Display management
    bool addDisplay(const DisplayConfig& display);
    bool removeDisplay(uint8_t displayId);
    bool clearDisplay(uint8_t displayId);
    bool setDisplayText(uint8_t displayId, const String& text);
    bool setDisplayText(uint8_t displayId, const String& text, uint8_t row, uint8_t col);
    bool setDisplayBrightness(uint8_t displayId, uint8_t brightness);
    uint8_t getDisplayCount() const { return displayCount; }
    bool getDisplay(uint8_t displayId, DisplayConfig& display) const;
    
    // LED indicators
    bool addIndicator(uint8_t pin, const String& label);
    bool removeIndicator(uint8_t indicatorId);
    bool setIndicatorState(uint8_t indicatorId, bool state);
    bool getIndicatorState(uint8_t indicatorId) const;
    bool toggleIndicator(uint8_t indicatorId);
    uint8_t getIndicatorCount() const { return indicatorCount; }
    
    // Panel control
    bool initialize();
    void update();
    void shutdown();
    bool performSelfTest();
    bool performCalibration();
    
    // Status and health
    PanelStatus getStatus() const { return status; }
    bool isInitialized() const { return status.initialized; }
    bool isHealthy() const { return status.healthy; }
    uint8_t getErrorFlags() const { return errorFlags; }
    
    // Error handling
    String getLastError() const { return lastError; }
    void clearErrors();
    
    // Diagnostics
    void printStatus();
    void printDevices();
    void printSwitches();
    void printDisplays();
    void printConfiguration();
    
    // Hardware-specific methods
    bool setDevicePin(uint8_t deviceId, uint8_t pin);
    bool setSwitchPin(uint8_t switchId, uint8_t pin);
    bool setDisplayPin(uint8_t displayId, uint8_t pin);
    
    // Utility methods
    bool findDeviceByLabel(const String& label, uint8_t& deviceId) const;
    bool findSwitchByLabel(const String& label, uint8_t& switchId) const;
    bool findDisplayByLabel(const String& label, uint8_t& displayId) const;
};

#endif // PANEL_INTERFACE_H 