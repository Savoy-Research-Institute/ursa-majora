#ifndef AUDIBLE_SIGNAL_INTERFACE_H
#define AUDIBLE_SIGNAL_INTERFACE_H

#include <Arduino.h>

// Audio signal types
enum class AudioSignalType {
    WARNING_TONE,
    CAUTION_TONE,
    STATUS_TONE,
    COMMUNICATION_TONE,
    TEST_TONE,
    EMERGENCY_TONE
};

// Audio device types
enum class AudioDeviceType {
    SPEAKER,
    BUZZER,
    AUDIO_AMPLIFIER,
    TONE_GENERATOR
};

// Audio signal configuration
struct AudioSignalConfig {
    AudioSignalType type;
    uint16_t frequency;      // Frequency in Hz
    uint16_t duration;       // Duration in milliseconds
    uint8_t volume;          // Volume level (0-100)
    uint8_t priority;        // Priority level (0-255, higher = more important)
    bool repeat;             // Whether to repeat the signal
    uint16_t repeatInterval; // Repeat interval in milliseconds
};

// Audio device configuration
struct AudioDeviceConfig {
    AudioDeviceType type;
    uint8_t pin;             // Digital pin for output
    uint8_t volumePin;       // Analog pin for volume control (if applicable)
    bool enabled;
    uint8_t maxVolume;       // Maximum volume for this device
};

class AudibleSignalInterface {
private:
    static const uint8_t MAX_DEVICES = 4;
    static const uint8_t MAX_SIGNALS = 16;
    
    AudioDeviceConfig devices[MAX_DEVICES];
    uint8_t deviceCount;
    
    AudioSignalConfig activeSignals[MAX_SIGNALS];
    uint8_t signalCount;
    
    // Audio state management
    bool audioEnabled;
    uint8_t masterVolume;
    unsigned long lastSignalTime;
    
    // Hardware-specific variables
    uint8_t currentTonePin;
    uint16_t currentFrequency;
    bool toneActive;
    
    // Error handling
    uint8_t errorFlags;
    String lastError;
    
    // Private methods
    bool generateTone(uint8_t pin, uint16_t frequency, uint16_t duration);
    void stopTone();
    bool setDeviceVolume(uint8_t deviceIndex, uint8_t volume);
    void updateActiveSignals();
    bool validateSignalConfig(const AudioSignalConfig& config);
    
public:
    AudibleSignalInterface();
    
    // Device management
    bool addDevice(AudioDeviceType type, uint8_t pin, uint8_t volumePin = 255, uint8_t maxVolume = 100);
    bool removeDevice(uint8_t deviceIndex);
    bool enableDevice(uint8_t deviceIndex);
    bool disableDevice(uint8_t deviceIndex);
    
    // Signal management
    bool addSignal(AudioSignalType type, uint16_t frequency, uint16_t duration, uint8_t volume = 50, uint8_t priority = 128);
    bool removeSignal(uint8_t signalIndex);
    bool playSignal(AudioSignalType type);
    bool playCustomSignal(uint16_t frequency, uint16_t duration, uint8_t volume = 50);
    
    // Volume control
    void setMasterVolume(uint8_t volume);
    uint8_t getMasterVolume() const { return masterVolume; }
    void setDeviceVolume(uint8_t deviceIndex, uint8_t volume);
    uint8_t getDeviceVolume(uint8_t deviceIndex) const;
    
    // System control
    void enableAudio();
    void disableAudio();
    bool isAudioEnabled() const { return audioEnabled; }
    void stopAllSignals();
    
    // Priority management
    void setSignalPriority(AudioSignalType type, uint8_t priority);
    uint8_t getSignalPriority(AudioSignalType type) const;
    
    // Error handling
    uint8_t getErrorFlags() const { return errorFlags; }
    String getLastError() const { return lastError; }
    void clearErrors();
    
    // System interface
    bool initialize();
    void update();
    void shutdown();
    
    // Diagnostic functions
    void runSelfTest();
    void printDiagnostics();
    
    // Test functions
    void playTestSequence();
    void playWarningSequence();
    void playStatusSequence();
};

#endif // AUDIBLE_SIGNAL_INTERFACE_H 