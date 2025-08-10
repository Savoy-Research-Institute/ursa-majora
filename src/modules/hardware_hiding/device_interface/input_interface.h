#ifndef INPUT_INTERFACE_H
#define INPUT_INTERFACE_H

#include <Arduino.h>
#include <Keypad.h>

// Input States
enum class InputState {
    DISABLED,
    INITIALIZING,
    READY,
    READING,
    ERROR
};

// Input Types
enum class InputType {
    KEYPAD,
    BUTTONS,
    SWITCHES,
    ROTARY_ENCODER,
    TOUCH_SCREEN,
    JOYSTICK,
    RADIO
};

// Radio channel enumeration for easy channel identification
enum class ChannelId {
    THROTTLE = 0,
    ROLL,
    PITCH,
    YAW,
    SWITCH3WAY_1,
    POT1,
    SWITCH3WAY_2,
    POT2,
    CHANNEL_COUNT = 8
};

// Radio system status enumeration
enum class RadioStatus {
    RADIO_OFF,
    RADIO_ON,
    RADIO_FAILSAFE
};

// Input Data Structure
struct InputData {
    char key_pressed;
    uint8_t button_state;
    uint8_t switch_state;
    int16_t encoder_value;
    uint16_t touch_x;
    uint16_t touch_y;
    uint32_t timestamp;
    bool is_valid;
    
    // Radio-specific data
    int radio_channels[8];           // Raw PWM values for each channel
    float normalized_channels[8];    // Normalized values (-1.0 to 1.0)
    RadioStatus radio_status;       // Current radio status
    unsigned long last_signal_time; // Time since last signal received
};

// Input Configuration
struct InputConfig {
    InputType type;
    uint8_t rows;
    uint8_t columns;
    uint8_t* row_pins;
    uint8_t* col_pins;
    char* keymap;
    bool enable_debounce;
    uint16_t debounce_time_ms;
    bool enable_repeat;
    uint16_t repeat_delay_ms;
    
    // Radio-specific configuration
    int transistor_pin;              // Pin controlling radio power transistor
    int ppm_pin;                    // Pin connected to PPM signal from receiver
    unsigned long failsafe_timeout_ms; // Timeout for failsafe detection
    int channel_center_us[8];       // Center position for each channel (default 1500)
    int channel_range_us[8];        // Range for each channel (default 500)
};

// Input Error Information
struct InputError {
    bool hardware_error;
    bool communication_error;
    bool debounce_error;
    bool memory_error;
    String error_message;
};

class InputInterface {
private:
    // Hardware Configuration
    InputConfig config;
    Keypad* keypad;
    
    // State Management
    InputState currentState;
    InputData currentData;
    InputData lastData;
    InputError errorInfo;
    
    // Input Processing
    char lastKeyPressed;
    char currentKeyPressed;
    bool keyChanged;
    bool keyHeld;
    
    // Debouncing
    unsigned long lastKeyTime;
    unsigned long lastDebounceTime;
    uint16_t debounceDelay;
    
    // Key Repeat
    bool repeatEnabled;
    unsigned long repeatStartTime;
    unsigned long repeatInterval;
    uint16_t repeatDelay;
    
    // Buffer Management
    char keyBuffer[32];
    uint8_t bufferIndex;
    uint8_t bufferSize;
    
    // Radio Management
    bool radioInitialized;
    unsigned long lastRadioCheck;
    unsigned long radioCheckInterval;
    
    // Private Methods
    bool initializeHardware();
    bool configureInput();
    void processKeyInput();
    void processButtonInput();
    void processSwitchInput();
    
    // Radio-specific private methods
    bool initializeRadio();
    void processRadioInput();
    void updateRadioStatus();
    void checkRadioFailsafe();
    int normalizeChannelValue(int raw_value, ChannelId channel);
    void processEncoderInput();
    void processTouchInput();
    void updateDebounce();
    void updateKeyRepeat();
    void updateErrorInfo();
    
public:
    InputInterface();
    ~InputInterface();
    
    // Initialization and Control
    bool initialize();
    void setConfiguration(const InputConfig& cfg);
    void enable();
    void disable();
    bool isEnabled() const;
    void reset();
    
    // Configuration
    void setInputType(InputType type);
    void setKeypadLayout(uint8_t rows, uint8_t cols, const uint8_t* row_pins, const uint8_t* col_pins, const char* keymap);
    void setDebounceTime(uint16_t time_ms);
    void setRepeatSettings(bool enable, uint16_t delay_ms, uint16_t interval_ms);
    void setBufferSize(uint8_t size);
    
    // Input Reading
    bool read();
    char getKeyPressed() const { return currentData.key_pressed; }
    char getLastKeyPressed() const { return lastKeyPressed; }
    bool isKeyPressed() const;
    bool isKeyHeld() const { return keyHeld; }
    bool hasKeyChanged() const { return keyChanged; }
    
    // Button and Switch Reading
    uint8_t getButtonState() const { return currentData.button_state; }
    uint8_t getSwitchState() const { return currentData.switch_state; }
    bool isButtonPressed(uint8_t button) const;
    bool isSwitchOn(uint8_t switch_num) const;
    
    // Encoder Reading
    int16_t getEncoderValue() const { return currentData.encoder_value; }
    int16_t getEncoderDelta() const;
    void resetEncoder();
    
    // Touch Reading
    uint16_t getTouchX() const { return currentData.touch_x; }
    uint16_t getTouchY() const { return currentData.touch_y; }
    bool isTouched() const;
    
    // Radio Reading
    int getRadioChannel(ChannelId channel) const;
    float getRadioChannelNormalized(ChannelId channel) const;
    RadioStatus getRadioStatus() const { return currentData.radio_status; }
    bool isRadioOn() const { return currentData.radio_status == RadioStatus::RADIO_ON; }
    bool isRadioFailsafe() const { return currentData.radio_status == RadioStatus::RADIO_FAILSAFE; }
    unsigned long getTimeSinceLastSignal() const { return currentData.last_signal_time; }
    
    // Radio Control
    void switchRadioOn();
    void switchRadioOff();
    void setChannelCalibration(ChannelId channel, int center_us = 1500, int range_us = 500);
    void resetRadioCalibration();
    
    // Buffer Operations
    void clearBuffer();
    String getBuffer() const;
    uint8_t getBufferLength() const { return bufferIndex; }
    bool isBufferFull() const { return bufferIndex >= bufferSize; }
    void addToBuffer(char key);
    void removeFromBuffer();
    
    // Data Access
    InputData getData() const { return currentData; }
    InputState getState() const { return currentState; }
    bool isReady() const { return currentState == InputState::READY; }
    
    // Input Validation
    bool isValidKey(char key) const;
    bool isNumericKey(char key) const;
    bool isAlphaKey(char key) const;
    bool isSpecialKey(char key) const;
    
    // Error Handling
    bool hasError() const;
    InputError getError() const { return errorInfo; }
    void clearErrors();
    
    // Diagnostics
    bool performSelfTest();
    void getDiagnostics(String& diagnostics);
    void printStatus();
    void testInput();
    
    // Advanced Features
    void enableKeyFiltering(bool enable);
    void setKeyFilter(const char* allowed_keys);
    void enableKeyMapping(bool enable);
    void setKeyMap(const char* key_map);
    void enableInputLogging(bool enable);
    
    // Utility Functions
    String inputTypeToString(InputType type);
    InputType stringToInputType(const String& type);
    char keyToChar(char key);
    char charToKey(char character);
    void printKeyMap();
    
    // Event Handlers
    void onKeyPressed(char key);
    void onKeyReleased(char key);
    void onKeyHeld(char key);
    void onButtonPressed(uint8_t button);
    void onSwitchChanged(uint8_t switch_num, bool state);
    void onEncoderChanged(int16_t value);
    void onTouchDetected(uint16_t x, uint16_t y);
    void onRadioChannelChanged(ChannelId channel, int value);
    void onRadioStatusChanged(RadioStatus oldStatus, RadioStatus newStatus);
    void onRadioFailsafe();
    void onError(const String& error);
    void onStateChanged(InputState oldState, InputState newState);
};

#endif // INPUT_INTERFACE_H 