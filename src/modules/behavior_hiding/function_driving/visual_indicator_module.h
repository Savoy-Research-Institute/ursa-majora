#ifndef VISUAL_INDICATOR_MODULE_H
#define VISUAL_INDICATOR_MODULE_H

#include <Arduino.h>
#include "../../hardware_hiding/device_interface/panel_interface.h"
#include "../../shared_services/mode_determination.h"

// Visual indicator types
enum class IndicatorType {
    LED,
    LCD_DISPLAY,
    OLED_DISPLAY,
    SEVEN_SEGMENT,
    BAR_GRAPH,
    ANALOG_METER,
    DIGITAL_READOUT,
    STATUS_LIGHT,
    WARNING_LIGHT,
    ERROR_LIGHT
};

// Indicator states
enum class IndicatorState {
    OFF,
    ON,
    BLINKING,
    FLASHING,
    PULSING,
    DIM,
    BRIGHT,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_WHITE
};

// Indicator priority levels
enum class IndicatorPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL,
    EMERGENCY
};

// Visual indicator configuration
struct VisualIndicatorConfig {
    IndicatorType type;
    uint8_t pin;             // Hardware pin
    uint8_t id;              // Indicator identifier
    String label;             // Display label
    IndicatorState defaultState; // Default state
    IndicatorPriority priority;   // Priority level
    bool enabled;             // Whether indicator is enabled
    uint8_t brightness;      // Brightness level (0-100)
    uint16_t blinkRate;      // Blink rate in milliseconds
    bool autoDim;            // Auto-dimming enabled
    uint16_t dimTimeout;     // Dimming timeout in milliseconds
};

// Display content
struct DisplayContent {
    String text;             // Display text
    uint8_t row;             // Row position (for multi-line displays)
    uint8_t column;          // Column position
    bool centered;            // Whether text should be centered
    bool scrolling;           // Whether text should scroll
    uint16_t scrollSpeed;    // Scroll speed in milliseconds
    bool autoClear;           // Auto-clear after timeout
    uint16_t clearTimeout;   // Clear timeout in milliseconds
};

// Visual indicator status
struct VisualIndicatorStatus {
    IndicatorState state;
    bool active;
    bool blinking;
    bool flashing;
    uint8_t brightness;
    unsigned long lastStateChange;
    unsigned long lastBlinkTime;
    bool healthy;
};

// Visual system status
struct VisualSystemStatus {
    bool enabled;
    bool autoDimEnabled;
    uint8_t masterBrightness;
    uint8_t activeIndicatorCount;
    uint8_t errorFlags;
    bool healthy;
};

class VisualIndicatorModule {
private:
    static const uint8_t MAX_INDICATORS = 32;
    static const uint8_t MAX_DISPLAYS = 8;
    static const uint8_t MAX_DISPLAY_CONTENT = 16;
    
    // Panel interface
    PanelInterface* panelInterface;
    ModeDetermination* modeDetermination;
    
    // Visual indicators
    VisualIndicatorConfig indicators[MAX_INDICATORS];
    VisualIndicatorStatus indicatorStatus[MAX_INDICATORS];
    uint8_t indicatorCount;
    
    // Displays
    uint8_t displayIds[MAX_DISPLAYS];
    DisplayContent displayContent[MAX_DISPLAY_CONTENT];
    uint8_t displayCount;
    uint8_t contentCount;
    
    // System status
    VisualSystemStatus status;
    
    // Timing and intervals
    unsigned long lastUpdateTime;
    unsigned long lastBlinkUpdate;
    unsigned long lastDimCheck;
    unsigned long lastStatusUpdate;
    
    // Error handling
    uint8_t errorFlags;
    String lastError;
    
    // Private methods
    bool initializeIndicators();
    bool initializeDisplays();
    bool addIndicator(const VisualIndicatorConfig& config);
    bool addDisplay(uint8_t displayId);
    void updateIndicators();
    void updateDisplays();
    void updateBlinking();
    void updateAutoDim();
    bool validateIndicatorConfig(const VisualIndicatorConfig& config);
    bool validateDisplayContent(const DisplayContent& content);
    void updateSystemStatus();
    
public:
    VisualIndicatorModule();
    
    // Indicator management
    bool createIndicator(IndicatorType type, uint8_t pin, const String& label);
    bool removeIndicator(uint8_t indicatorId);
    bool enableIndicator(uint8_t indicatorId);
    bool disableIndicator(uint8_t indicatorId);
    bool setIndicatorState(uint8_t indicatorId, IndicatorState state);
    bool setIndicatorBrightness(uint8_t indicatorId, uint8_t brightness);
    bool setIndicatorBlinkRate(uint8_t indicatorId, uint16_t rate);
    
    // Display management
    bool addDisplay(uint8_t displayId, const String& label);
    bool removeDisplay(uint8_t displayId);
    bool setDisplayText(uint8_t displayId, const String& text);
    bool setDisplayText(uint8_t displayId, const String& text, uint8_t row, uint8_t col);
    bool clearDisplay(uint8_t displayId);
    bool setDisplayBrightness(uint8_t displayId, uint8_t brightness);
    
    // Status indicators
    bool setSystemStatus(bool healthy);
    bool setWarningStatus(bool active);
    bool setErrorStatus(bool active);
    bool setModeIndicator(uint8_t mode);
    bool setPowerIndicator(bool active);
    bool setCommunicationIndicator(bool active);
    bool setTargetIndicator(bool active);
    bool setWeaponsIndicator(bool armed);
    
    // System control
    bool initialize();
    void update();
    void shutdown();
    bool performSelfTest();
    
    // Configuration
    bool setMasterBrightness(uint8_t brightness);
    bool enableAutoDim(bool enabled);
    bool setAutoDimTimeout(uint16_t timeout);
    bool setIndicatorPriority(uint8_t indicatorId, IndicatorPriority priority);
    bool setIndicatorAutoDim(uint8_t indicatorId, bool enabled);
    
    // Status and health
    VisualSystemStatus getStatus() const { return status; }
    bool isEnabled() const { return status.enabled; }
    bool isHealthy() const { return status.healthy; }
    uint8_t getActiveIndicatorCount() const { return status.activeIndicatorCount; }
    uint8_t getIndicatorCount() const { return indicatorCount; }
    uint8_t getDisplayCount() const { return displayCount; }
    
    // Indicator information
    bool getIndicator(uint8_t indicatorId, VisualIndicatorConfig& config) const;
    bool getIndicatorStatus(uint8_t indicatorId, VisualIndicatorStatus& status) const;
    IndicatorState getIndicatorState(uint8_t indicatorId) const;
    bool isIndicatorActive(uint8_t indicatorId) const;
    
    // Display information
    bool getDisplayContent(uint8_t displayId, DisplayContent& content) const;
    bool isDisplayActive(uint8_t displayId) const;
    
    // Mode-based indicators
    bool enableModeBasedIndicators(bool enabled);
    bool setModeIndicator(uint8_t mode, bool enabled);
    void updateModeIndicators();
    
    // Emergency handling
    void activateEmergencyIndicators();
    void deactivateEmergencyIndicators();
    void activateWarningIndicators();
    void deactivateWarningIndicators();
    void activateStatusIndicators();
    void deactivateStatusIndicators();
    
    // Error handling
    uint8_t getErrorFlags() const { return errorFlags; }
    String getLastError() const { return lastError; }
    void clearErrors();
    
    // Diagnostics
    void printStatus();
    void printIndicators();
    void printDisplays();
    void printConfiguration();
    
    // Hardware interface
    void setPanelInterface(PanelInterface* interface) { panelInterface = interface; }
    void setModeDetermination(ModeDetermination* mode) { modeDetermination = mode; }
    
    // Utility methods
    bool isIndicatorConfigured(uint8_t indicatorId) const;
    bool isDisplayConfigured(uint8_t displayId) const;
    bool hasActiveIndicators() const;
    bool hasActiveDisplays() const;
    void updateBrightnessBasedOnMode();
    
    // Advanced features
    bool enableIndicatorProfiles(bool enabled);
    bool setIndicatorProfile(uint8_t profile);
    bool saveIndicatorProfile(uint8_t profile);
    bool loadIndicatorProfile(uint8_t profile);
    bool enableDisplayProfiles(bool enabled);
    bool setDisplayProfile(uint8_t profile);
    bool saveDisplayProfile(uint8_t profile);
    bool loadDisplayProfile(uint8_t profile);
};

#endif // VISUAL_INDICATOR_MODULE_H 