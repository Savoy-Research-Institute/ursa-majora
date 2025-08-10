#ifndef AUDIBLE_SIGNAL_MODULE_H
#define AUDIBLE_SIGNAL_MODULE_H

#include <Arduino.h>
#include "../../hardware_hiding/device_interface/audible_signal_interface.h"
#include "../../shared_services/mode_determination.h"

// Audio signal types
enum class AudioSignalType {
    SYSTEM_STARTUP,
    SYSTEM_SHUTDOWN,
    WARNING_ALERT,
    CAUTION_ALERT,
    EMERGENCY_ALERT,
    STATUS_UPDATE,
    COMMUNICATION_ACTIVE,
    TEST_SIGNAL,
    CALIBRATION_COMPLETE,
    ERROR_DETECTED,
    MODE_CHANGE,
    TARGET_ACQUIRED,
    WEAPONS_ARMED,
    WEAPONS_SAFE,
    FUEL_LOW,
    ENGINE_START,
    ENGINE_SHUTDOWN,
    GEAR_UP,
    GEAR_DOWN,
    FLAPS_EXTEND,
    FLAPS_RETRACT
};

// Audio priority levels
enum class AudioPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL,
    EMERGENCY
};

// Audio signal configuration
struct AudioSignalConfig {
    AudioSignalType type;
    uint16_t frequency;      // Frequency in Hz
    uint16_t duration;       // Duration in milliseconds
    uint8_t volume;          // Volume level (0-100)
    AudioPriority priority;   // Priority level
    bool repeat;             // Whether to repeat the signal
    uint16_t repeatInterval; // Repeat interval in milliseconds
    uint8_t repeatCount;     // Number of times to repeat (0 = infinite)
    bool interruptible;      // Whether higher priority can interrupt
};

// Audio queue item
struct AudioQueueItem {
    AudioSignalType type;
    AudioPriority priority;
    unsigned long requestTime;
    bool active;
    uint8_t repeatCount;
    unsigned long lastPlayTime;
};

// Audio system status
struct AudioSystemStatus {
    bool enabled;
    bool muted;
    uint8_t masterVolume;
    uint8_t currentSignalType;
    AudioPriority currentPriority;
    uint8_t queueLength;
    bool playing;
    uint8_t errorFlags;
    bool healthy;
};

class AudibleSignalModule {
private:
    static const uint8_t MAX_QUEUE_ITEMS = 32;
    static const uint8_t MAX_SIGNAL_TYPES = 64;
    
    // Audio interface
    AudibleSignalInterface* audioInterface;
    ModeDetermination* modeDetermination;
    
    // Signal configurations
    AudioSignalConfig signalConfigs[MAX_SIGNAL_TYPES];
    uint8_t signalConfigCount;
    
    // Audio queue
    AudioQueueItem audioQueue[MAX_QUEUE_ITEMS];
    uint8_t queueHead;
    uint8_t queueTail;
    uint8_t queueLength;
    
    // Current audio state
    AudioSystemStatus status;
    AudioSignalType currentSignal;
    bool signalActive;
    
    // Timing and intervals
    unsigned long lastUpdateTime;
    unsigned long lastVolumeCheck;
    unsigned long lastStatusUpdate;
    
    // Error handling
    uint8_t errorFlags;
    String lastError;
    
    // Private methods
    bool initializeSignalConfigs();
    bool addSignalConfig(AudioSignalType type, uint16_t frequency, uint16_t duration, 
                        uint8_t volume, AudioPriority priority, bool repeat = false);
    bool queueAudioSignal(AudioSignalType type, AudioPriority priority);
    void processAudioQueue();
    void updateCurrentSignal();
    bool shouldInterruptCurrent(AudioPriority newPriority) const;
    void stopCurrentSignal();
    bool validateSignalConfig(const AudioSignalConfig& config);
    void updateSystemStatus();
    
public:
    AudibleSignalModule();
    
    // Configuration
    bool configureSignal(AudioSignalType type, uint16_t frequency, uint16_t duration, 
                        uint8_t volume, AudioPriority priority);
    bool setSignalRepeat(AudioSignalType type, bool repeat, uint16_t interval = 1000, uint8_t count = 0);
    bool setSignalInterruptible(AudioSignalType type, bool interruptible);
    AudioSignalConfig getSignalConfig(AudioSignalType type) const;
    
    // Audio control
    bool playSignal(AudioSignalType type);
    bool playSignal(AudioSignalType type, AudioPriority priority);
    bool playCustomSignal(uint16_t frequency, uint16_t duration, uint8_t volume = 50);
    bool stopSignal();
    bool stopAllSignals();
    bool pauseAudio();
    bool resumeAudio();
    
    // Volume control
    void setMasterVolume(uint8_t volume);
    uint8_t getMasterVolume() const { return status.masterVolume; }
    void setSignalVolume(AudioSignalType type, uint8_t volume);
    uint8_t getSignalVolume(AudioSignalType type) const;
    void mute();
    void unmute();
    bool isMuted() const { return status.muted; }
    
    // System control
    bool initialize();
    void update();
    void shutdown();
    bool performSelfTest();
    
    // Status and health
    AudioSystemStatus getStatus() const { return status; }
    bool isEnabled() const { return status.enabled; }
    bool isHealthy() const { return status.healthy; }
    bool isPlaying() const { return status.playing; }
    uint8_t getQueueLength() const { return status.queueLength; }
    AudioSignalType getCurrentSignal() const { return currentSignal; }
    
    // Queue management
    bool clearQueue();
    bool removeFromQueue(AudioSignalType type);
    bool getQueueItem(uint8_t index, AudioQueueItem& item) const;
    uint8_t getQueueLength() const { return queueLength; }
    
    // Mode-based audio
    bool enableModeBasedAudio(bool enabled);
    bool setModeAudio(AudioSignalType type, bool enabled);
    void updateModeAudio();
    
    // Emergency handling
    void playEmergencySequence();
    void playWarningSequence();
    void playCautionSequence();
    void playStatusSequence();
    
    // Error handling
    uint8_t getErrorFlags() const { return errorFlags; }
    String getLastError() const { return lastError; }
    void clearErrors();
    
    // Diagnostics
    void printStatus();
    void printSignalConfigs();
    void printAudioQueue();
    void printConfiguration();
    
    // Hardware interface
    void setAudioInterface(AudibleSignalInterface* interface) { audioInterface = interface; }
    void setModeDetermination(ModeDetermination* mode) { modeDetermination = mode; }
    
    // Utility methods
    bool isSignalConfigured(AudioSignalType type) const;
    bool hasActiveSignals() const;
    bool isHighPriorityActive() const;
    void updateVolumeBasedOnMode();
    
    // Advanced features
    bool enableAudioProfiles(bool enabled);
    bool setAudioProfile(uint8_t profile);
    bool saveAudioProfile(uint8_t profile);
    bool loadAudioProfile(uint8_t profile);
};

#endif // AUDIBLE_SIGNAL_MODULE_H 