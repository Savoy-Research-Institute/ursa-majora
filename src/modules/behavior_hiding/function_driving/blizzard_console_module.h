#ifndef BLIZZARD_CONSOLE_MODULE_H
#define BLIZZARD_CONSOLE_MODULE_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include "../../hardware_hiding/device_interface/communication_interface.h"
#include "../../hardware_hiding/device_interface/display_interface.h"
#include "../../hardware_hiding/device_interface/input_interface.h"
#include "../../behavior_hiding/shared_services/alert_module.h"

// Console States
enum class ConsoleState {
    BOOTING,
    MAIN_MENU,
    FLIGHT_CONTROL,
    TELEMETRY_VIEW,
    SETTINGS,
    DIAGNOSTICS,
    ERROR
};

// Console Modes
enum class ConsoleMode {
    NORMAL,
    CHEAT_CODE,
    ACCESS_CONTROL,
    MAINTENANCE,
    DEBUG
};

// Console Events
enum class ConsoleEventType {
    KEY_PRESS,
    MENU_SELECTION,
    COMMUNICATION_RECEIVED,
    ALERT_RECEIVED,
    SYSTEM_ERROR,
    MODE_CHANGE
};

// Console Configuration
struct ConsoleConfig {
    uint8_t lcd_address;
    uint8_t lcd_columns;
    uint8_t lcd_rows;
    uint8_t hc12_tx_pin;
    uint8_t hc12_rx_pin;
    uint8_t volume_pin;
    uint8_t keypad_rows;
    uint8_t keypad_cols;
    uint16_t baud_rate;
    bool enable_sound;
    bool enable_backlight;
};

// Console Data Structure
struct ConsoleData {
    String current_menu;
    String display_line1;
    String display_line2;
    char last_key_pressed;
    String received_data;
    bool access_granted;
    bool cheat_code_mode;
    uint32_t timestamp;
};

// Cheat Code Structure
struct CheatCode {
    String code;
    String description;
    bool enabled;
    uint32_t activation_time;
};

class BlizzardConsoleModule {
private:
    // Hardware Configuration
    ConsoleConfig config;
    
    // State Management
    ConsoleState currentState;
    ConsoleMode currentMode;
    ConsoleData consoleData;
    
    // Hardware Interfaces
    LiquidCrystal_I2C* lcd;
    Keypad* keypad;
    SoftwareSerial* hc12;
    CommunicationInterface* commInterface;
    DisplayInterface* displayInterface;
    InputInterface* inputInterface;
    AlertModule* alertModule;
    
    // Menu System
    String menuItems[10];
    uint8_t menuItemCount;
    uint8_t currentMenuItem;
    
    // Cheat Code System
    CheatCode cheatCodes[20];
    uint8_t cheatCodeCount;
    bool cheatCodeMode;
    
    // Access Control
    String accessCode;
    bool accessGranted;
    uint32_t accessTimeout;
    
    // Timing
    unsigned long lastUpdateTime;
    unsigned long lastKeyTime;
    unsigned long lastDisplayUpdate;
    
    // Private Methods
    bool initializeHardware();
    bool initializeInterfaces();
    void processKeyInput();
    void updateDisplay();
    void processMenu();
    void handleCheatCodes();
    void checkAccessControl();
    void processCommunication();
    void updateState();
    
public:
    BlizzardConsoleModule();
    ~BlizzardConsoleModule();
    
    // Initialization and Control
    bool initialize();
    void setConfiguration(const ConsoleConfig& cfg);
    void reset();
    void shutdown();
    
    // State Management
    void setState(ConsoleState state);
    void setMode(ConsoleMode mode);
    ConsoleState getState() const { return currentState; }
    ConsoleMode getMode() const { return currentMode; }
    
    // Display Management
    void clearDisplay();
    void setDisplayLine(uint8_t line, const String& text);
    void setDisplayLines(const String& line1, const String& line2);
    void scrollDisplay(const String& text);
    void setBacklight(bool enable);
    void setContrast(uint8_t contrast);
    
    // Menu System
    void addMenuItem(const String& item);
    void removeMenuItem(uint8_t index);
    void setCurrentMenuItem(uint8_t index);
    void nextMenuItem();
    void previousMenuItem();
    void selectMenuItem();
    String getCurrentMenuItem() const;
    
    // Input Handling
    char getLastKeyPressed() const { return consoleData.last_key_pressed; }
    bool isKeyPressed() const;
    void clearKeyBuffer();
    void setKeyRepeat(bool enable);
    
    // Communication
    void sendData(const String& data);
    String getReceivedData() const { return consoleData.received_data; }
    bool hasReceivedData() const;
    void clearReceivedData();
    
    // Cheat Code System
    void addCheatCode(const String& code, const String& description);
    void removeCheatCode(const String& code);
    void enableCheatCode(const String& code, bool enable);
    bool isCheatCodeActive(const String& code) const;
    void activateCheatCode(const String& code);
    void deactivateCheatCode(const String& code);
    void listCheatCodes(String& list);
    
    // Access Control
    void setAccessCode(const String& code);
    bool checkAccessCode(const String& code);
    bool isAccessGranted() const { return accessGranted; }
    void grantAccess();
    void revokeAccess();
    void setAccessTimeout(uint32_t timeout_ms);
    
    // System Updates
    void update();
    void processEvents();
    void handleSystemEvents();
    
    // Configuration
    void setBaudRate(uint16_t baud);
    void setLCDAddress(uint8_t address);
    void setKeypadPins(const uint8_t* row_pins, const uint8_t* col_pins);
    void setHC12Pins(uint8_t tx_pin, uint8_t rx_pin);
    
    // Error Handling
    bool hasError() const;
    void clearErrors();
    
    // Diagnostics
    bool performSelfTest();
    void getDiagnostics(String& diagnostics);
    void printSystemInfo();
    void testHardware();
    
    // Advanced Features
    void enableSound(bool enable);
    void setVolume(uint8_t volume);
    void playTone(uint16_t frequency, uint16_t duration);
    void enableVibration(bool enable);
    void setBrightness(uint8_t brightness);
    
    // Utility Functions
    String stateToString(ConsoleState state);
    String modeToString(ConsoleMode mode);
    ConsoleState stringToState(const String& state);
    ConsoleMode stringToMode(const String& mode);
    
    // Event Handlers
    void onKeyPressed(char key);
    void onMenuSelected(uint8_t item);
    void onDataReceived(const String& data);
    void onAlertReceived(const String& alert);
    void onStateChanged(ConsoleState oldState, ConsoleState newState);
    void onModeChanged(ConsoleMode oldMode, ConsoleMode newMode);
};

#endif // BLIZZARD_CONSOLE_MODULE_H 