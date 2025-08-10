#ifndef DISPLAY_INTERFACE_H
#define DISPLAY_INTERFACE_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// Display States
enum class DisplayState {
    OFF,
    INITIALIZING,
    READY,
    UPDATING,
    ERROR
};

// Display Types
enum class DisplayType {
    LCD_I2C,
    LCD_PARALLEL,
    OLED_I2C,
    OLED_SPI,
    TFT,
    LED_MATRIX
};

// Display Data Structure
struct DisplayData {
    String line1;
    String line2;
    String line3;
    String line4;
    uint8_t cursor_x;
    uint8_t cursor_y;
    bool cursor_visible;
    bool backlight_on;
    uint32_t timestamp;
};

// Display Configuration
struct DisplayConfig {
    DisplayType type;
    uint8_t address;
    uint8_t columns;
    uint8_t rows;
    uint8_t sda_pin;
    uint8_t scl_pin;
    bool enable_backlight;
    bool enable_cursor;
    uint8_t contrast;
    uint8_t brightness;
};

// Display Error Information
struct DisplayError {
    bool communication_error;
    bool hardware_error;
    bool initialization_error;
    bool memory_error;
    String error_message;
};

class DisplayInterface {
private:
    // Hardware Configuration
    DisplayConfig config;
    LiquidCrystal_I2C* lcd;
    
    // State Management
    DisplayState currentState;
    DisplayData currentData;
    DisplayError errorInfo;
    
    // Buffer Management
    String displayBuffer[4];
    bool bufferChanged;
    
    // Timing
    unsigned long lastUpdateTime;
    unsigned long lastBlinkTime;
    uint16_t updateInterval;
    
    // Display Features
    bool backlightEnabled;
    bool cursorEnabled;
    bool cursorBlinking;
    uint8_t contrastLevel;
    uint8_t brightnessLevel;
    
    // Private Methods
    bool initializeHardware();
    bool configureDisplay();
    void updateDisplay();
    void processBuffer();
    void updateErrorInfo();
    void clearBuffer();
    
public:
    DisplayInterface();
    ~DisplayInterface();
    
    // Initialization and Control
    bool initialize();
    void setConfiguration(const DisplayConfig& cfg);
    void enable();
    void disable();
    bool isEnabled() const;
    void reset();
    
    // Configuration
    void setDisplayType(DisplayType type);
    void setAddress(uint8_t address);
    void setDimensions(uint8_t columns, uint8_t rows);
    void setPins(uint8_t sda_pin, uint8_t scl_pin);
    void setUpdateInterval(uint16_t interval_ms);
    
    // Display Operations
    void clear();
    void setCursor(uint8_t x, uint8_t y);
    void home();
    void noDisplay();
    void display();
    void noCursor();
    void cursor();
    void noBlink();
    void blink();
    
    // Text Display
    void print(const String& text);
    void printAt(uint8_t x, uint8_t y, const String& text);
    void setLine(uint8_t line, const String& text);
    void setLines(const String& line1, const String& line2);
    void setLines(const String& line1, const String& line2, const String& line3, const String& line4);
    void scrollLeft();
    void scrollRight();
    void autoscroll();
    void noAutoscroll();
    
    // Backlight Control
    void setBacklight(bool enable);
    void setBrightness(uint8_t brightness);
    bool isBacklightOn() const { return backlightEnabled; }
    uint8_t getBrightness() const { return brightnessLevel; }
    
    // Cursor Control
    void setCursorVisible(bool visible);
    void setCursorBlinking(bool blinking);
    void setCursorPosition(uint8_t x, uint8_t y);
    bool isCursorVisible() const { return cursorEnabled; }
    bool isCursorBlinking() const { return cursorBlinking; }
    
    // Contrast Control
    void setContrast(uint8_t contrast);
    uint8_t getContrast() const { return contrastLevel; }
    
    // Data Access
    DisplayData getData() const { return currentData; }
    String getLine(uint8_t line) const;
    DisplayState getState() const { return currentState; }
    bool isReady() const { return currentState == DisplayState::READY; }
    
    // Buffer Management
    void clearBuffer();
    void setBufferLine(uint8_t line, const String& text);
    void updateBuffer();
    bool isBufferChanged() const { return bufferChanged; }
    
    // Error Handling
    bool hasError() const;
    DisplayError getError() const { return errorInfo; }
    void clearErrors();
    
    // Diagnostics
    bool performSelfTest();
    void getDiagnostics(String& diagnostics);
    void printStatus();
    void testDisplay();
    
    // Advanced Features
    void createCustomChar(uint8_t location, uint8_t charmap[]);
    void setScrollSpeed(uint16_t speed_ms);
    void enableAutoScroll(bool enable);
    void setDisplayMode(uint8_t mode);
    
    // Utility Functions
    String displayTypeToString(DisplayType type);
    DisplayType stringToDisplayType(const String& type);
    void printCentered(uint8_t line, const String& text);
    void printRightAligned(uint8_t line, const String& text);
    void printWithPadding(uint8_t line, const String& text, char padding = ' ');
    
    // Event Handlers
    void onDisplayUpdated();
    void onError(const String& error);
    void onStateChanged(DisplayState oldState, DisplayState newState);
};

#endif // DISPLAY_INTERFACE_H 