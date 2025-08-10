#include "audible_signal_interface.h"

// Error flag definitions
#define ERROR_DEVICE_NOT_FOUND      0x01
#define ERROR_INVALID_SIGNAL        0x02
#define ERROR_DEVICE_DISABLED       0x04
#define ERROR_INVALID_VOLUME        0x08
#define ERROR_TONE_GENERATION       0x10

// Default signal configurations
const AudioSignalConfig DEFAULT_SIGNALS[] = {
    {AudioSignalType::WARNING_TONE, 800, 500, 70, 200, true, 2000},
    {AudioSignalType::CAUTION_TONE, 600, 300, 60, 150, true, 3000},
    {AudioSignalType::STATUS_TONE, 1000, 200, 50, 100, false, 0},
    {AudioSignalType::COMMUNICATION_TONE, 1200, 100, 80, 120, false, 0},
    {AudioSignalType::TEST_TONE, 440, 1000, 50, 50, false, 0},
    {AudioSignalType::EMERGENCY_TONE, 1500, 1000, 100, 255, true, 1000}
};

AudibleSignalInterface::AudibleSignalInterface() 
    : deviceCount(0), signalCount(0), audioEnabled(true), masterVolume(80),
      lastSignalTime(0), currentTonePin(255), currentFrequency(0), toneActive(false), errorFlags(0) {
    
    // Initialize device arrays
    for (uint8_t i = 0; i < MAX_DEVICES; i++) {
        devices[i].enabled = false;
        devices[i].maxVolume = 100;
    }
    
    // Initialize signal arrays
    for (uint8_t i = 0; i < MAX_SIGNALS; i++) {
        activeSignals[i].type = AudioSignalType::TEST_TONE;
        activeSignals[i].frequency = 0;
        activeSignals[i].duration = 0;
        activeSignals[i].volume = 0;
        activeSignals[i].priority = 0;
        activeSignals[i].repeat = false;
        activeSignals[i].repeatInterval = 0;
    }
    
    lastError = "";
}

bool AudibleSignalInterface::initialize() {
    // Add default signals
    for (uint8_t i = 0; i < sizeof(DEFAULT_SIGNALS) / sizeof(DEFAULT_SIGNALS[0]); i++) {
        addSignal(DEFAULT_SIGNALS[i].type, DEFAULT_SIGNALS[i].frequency, 
                 DEFAULT_SIGNALS[i].duration, DEFAULT_SIGNALS[i].volume, 
                 DEFAULT_SIGNALS[i].priority);
    }
    
    // Add a default buzzer device on pin 8 if no devices are configured
    if (deviceCount == 0) {
        addDevice(AudioDeviceType::BUZZER, 8);
    }
    
    return true;
}

bool AudibleSignalInterface::addDevice(AudioDeviceType type, uint8_t pin, uint8_t volumePin, uint8_t maxVolume) {
    if (deviceCount >= MAX_DEVICES) {
        lastError = "Maximum device count reached";
        return false;
    }
    
    // Check if pin is already in use
    for (uint8_t i = 0; i < deviceCount; i++) {
        if (devices[i].pin == pin) {
            lastError = "Pin already in use";
            return false;
        }
    }
    
    devices[deviceCount].type = type;
    devices[deviceCount].pin = pin;
    devices[deviceCount].volumePin = volumePin;
    devices[deviceCount].enabled = true;
    devices[deviceCount].maxVolume = maxVolume;
    
    // Configure pin
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    
    deviceCount++;
    return true;
}

bool AudibleSignalInterface::removeDevice(uint8_t deviceIndex) {
    if (deviceIndex >= deviceCount) {
        lastError = "Invalid device index";
        return false;
    }
    
    // Stop any active tone on this device
    if (currentTonePin == devices[deviceIndex].pin) {
        stopTone();
    }
    
    // Shift remaining devices
    for (uint8_t i = deviceIndex; i < deviceCount - 1; i++) {
        devices[i] = devices[i + 1];
    }
    
    deviceCount--;
    return true;
}

bool AudibleSignalInterface::enableDevice(uint8_t deviceIndex) {
    if (deviceIndex >= deviceCount) {
        lastError = "Invalid device index";
        return false;
    }
    
    devices[deviceIndex].enabled = true;
    return true;
}

bool AudibleSignalInterface::disableDevice(uint8_t deviceIndex) {
    if (deviceIndex >= deviceIndex) {
        lastError = "Invalid device index";
        return false;
    }
    
    devices[deviceIndex].enabled = false;
    
    // Stop any active tone on this device
    if (currentTonePin == devices[deviceIndex].pin) {
        stopTone();
    }
    
    return true;
}

bool AudibleSignalInterface::addSignal(AudioSignalType type, uint16_t frequency, uint16_t duration, uint8_t volume, uint8_t priority) {
    if (signalCount >= MAX_SIGNALS) {
        lastError = "Maximum signal count reached";
        return false;
    }
    
    AudioSignalConfig config;
    config.type = type;
    config.frequency = frequency;
    config.duration = duration;
    config.volume = volume;
    config.priority = priority;
    config.repeat = false;
    config.repeatInterval = 0;
    
    if (!validateSignalConfig(config)) {
        return false;
    }
    
    activeSignals[signalCount] = config;
    signalCount++;
    
    return true;
}

bool AudibleSignalInterface::removeSignal(uint8_t signalIndex) {
    if (signalIndex >= signalCount) {
        lastError = "Invalid signal index";
        return false;
    }
    
    // Shift remaining signals
    for (uint8_t i = signalIndex; i < signalCount - 1; i++) {
        activeSignals[i] = activeSignals[i + 1];
    }
    
    signalCount--;
    return true;
}

bool AudibleSignalInterface::playSignal(AudioSignalType type) {
    // Find the signal configuration
    for (uint8_t i = 0; i < signalCount; i++) {
        if (activeSignals[i].type == type) {
            return playCustomSignal(activeSignals[i].frequency, 
                                  activeSignals[i].duration, 
                                  activeSignals[i].volume);
        }
    }
    
    lastError = "Signal type not found";
    return false;
}

bool AudibleSignalInterface::playCustomSignal(uint16_t frequency, uint16_t duration, uint8_t volume) {
    if (!audioEnabled || deviceCount == 0) {
        lastError = "Audio system not available";
        return false;
    }
    
    // Find the best available device
    uint8_t bestDevice = 255;
    uint8_t bestPriority = 0;
    
    for (uint8_t i = 0; i < deviceCount; i++) {
        if (devices[i].enabled && devices[i].maxVolume >= volume) {
            if (bestDevice == 255 || devices[i].maxVolume > bestPriority) {
                bestDevice = i;
                bestPriority = devices[i].maxVolume;
            }
        }
    }
    
    if (bestDevice == 255) {
        lastError = "No suitable audio device available";
        return false;
    }
    
    // Stop any currently playing tone
    if (toneActive) {
        stopTone();
    }
    
    // Generate the tone
    if (generateTone(devices[bestDevice].pin, frequency, duration)) {
        currentTonePin = devices[bestDevice].pin;
        currentFrequency = frequency;
        toneActive = true;
        lastSignalTime = millis();
        return true;
    }
    
    return false;
}

void AudibleSignalInterface::setMasterVolume(uint8_t volume) {
    if (volume > 100) volume = 100;
    masterVolume = volume;
    
    // Update all device volumes proportionally
    for (uint8_t i = 0; i < deviceCount; i++) {
        if (devices[i].enabled) {
            uint8_t deviceVolume = (devices[i].maxVolume * masterVolume) / 100;
            setDeviceVolume(i, deviceVolume);
        }
    }
}

bool AudibleSignalInterface::setDeviceVolume(uint8_t deviceIndex, uint8_t volume) {
    if (deviceIndex >= deviceCount) {
        lastError = "Invalid device index";
        return false;
    }
    
    if (volume > 100) {
        lastError = "Invalid volume level";
        return false;
    }
    
    // For devices with volume control pins, set analog output
    if (devices[deviceIndex].volumePin != 255) {
        uint16_t analogValue = (volume * 255) / 100;
        analogWrite(devices[deviceIndex].volumePin, analogValue);
    }
    
    return true;
}

uint8_t AudibleSignalInterface::getDeviceVolume(uint8_t deviceIndex) const {
    if (deviceIndex >= deviceCount) return 0;
    
    // For devices with volume control pins, read analog input
    if (devices[deviceIndex].volumePin != 255) {
        uint16_t analogValue = analogRead(devices[deviceIndex].volumePin);
        return (analogValue * 100) / 255;
    }
    
    return devices[deviceIndex].maxVolume;
}

void AudibleSignalInterface::enableAudio() {
    audioEnabled = true;
}

void AudibleSignalInterface::disableAudio() {
    audioEnabled = false;
    stopAllSignals();
}

void AudibleSignalInterface::stopAllSignals() {
    if (toneActive) {
        stopTone();
    }
}

void AudibleSignalInterface::setSignalPriority(AudioSignalType type, uint8_t priority) {
    for (uint8_t i = 0; i < signalCount; i++) {
        if (activeSignals[i].type == type) {
            activeSignals[i].priority = priority;
            break;
        }
    }
}

uint8_t AudibleSignalInterface::getSignalPriority(AudioSignalType type) const {
    for (uint8_t i = 0; i < signalCount; i++) {
        if (activeSignals[i].type == type) {
            return activeSignals[i].priority;
        }
    }
    return 0;
}

void AudibleSignalInterface::clearErrors() {
    errorFlags = 0;
    lastError = "";
}

void AudibleSignalInterface::update() {
    // Check if current tone should be stopped
    if (toneActive && (millis() - lastSignalTime) >= 1000) {
        stopTone();
    }
    
    // Update active signals (handle repeating signals)
    updateActiveSignals();
}

void AudibleSignalInterface::shutdown() {
    stopAllSignals();
    audioEnabled = false;
    
    // Disable all devices
    for (uint8_t i = 0; i < deviceCount; i++) {
        devices[i].enabled = false;
        digitalWrite(devices[i].pin, LOW);
    }
}

bool AudibleSignalInterface::generateTone(uint8_t pin, uint16_t frequency, uint16_t duration) {
    if (frequency < 31 || frequency > 65535) {
        lastError = "Invalid frequency";
        errorFlags |= ERROR_TONE_GENERATION;
        return false;
    }
    
    // Use Arduino's tone function
    tone(pin, frequency, duration);
    return true;
}

void AudibleSignalInterface::stopTone() {
    if (toneActive && currentTonePin != 255) {
        noTone(currentTonePin);
        toneActive = false;
        currentTonePin = 255;
        currentFrequency = 0;
    }
}

void AudibleSignalInterface::updateActiveSignals() {
    // Handle repeating signals
    for (uint8_t i = 0; i < signalCount; i++) {
        if (activeSignals[i].repeat && activeSignals[i].repeatInterval > 0) {
            // Check if it's time to repeat this signal
            static unsigned long lastRepeatTimes[MAX_SIGNALS] = {0};
            if ((millis() - lastRepeatTimes[i]) >= activeSignals[i].repeatInterval) {
                playCustomSignal(activeSignals[i].frequency, 
                               activeSignals[i].duration, 
                               activeSignals[i].volume);
                lastRepeatTimes[i] = millis();
            }
        }
    }
}

bool AudibleSignalInterface::validateSignalConfig(const AudioSignalConfig& config) {
    if (config.frequency < 31 || config.frequency > 65535) {
        lastError = "Invalid frequency";
        errorFlags |= ERROR_INVALID_SIGNAL;
        return false;
    }
    
    if (config.duration == 0) {
        lastError = "Invalid duration";
        errorFlags |= ERROR_INVALID_SIGNAL;
        return false;
    }
    
    if (config.volume > 100) {
        lastError = "Invalid volume";
        errorFlags |= ERROR_INVALID_VOLUME;
        return false;
    }
    
    return true;
}

// Diagnostic functions
void AudibleSignalInterface::runSelfTest() {
    Serial.println("=== Audible Signal Interface Self Test ===");
    
    // Test device communication
    Serial.print("Total devices: "); Serial.println(deviceCount);
    for (uint8_t i = 0; i < deviceCount; i++) {
        Serial.print("Device ");
        Serial.print(i);
        Serial.print(": Type=");
        Serial.print((int)devices[i].type);
        Serial.print(", Pin=");
        Serial.print(devices[i].pin);
        Serial.print(", Enabled=");
        Serial.println(devices[i].enabled ? "YES" : "NO");
    }
    
    // Test signal generation
    Serial.print("Total signals: "); Serial.println(signalCount);
    for (uint8_t i = 0; i < signalCount; i++) {
        Serial.print("Signal ");
        Serial.print(i);
        Serial.print(": Type=");
        Serial.print((int)activeSignals[i].type);
        Serial.print(", Freq=");
        Serial.print(activeSignals[i].frequency);
        Serial.print("Hz, Duration=");
        Serial.print(activeSignals[i].duration);
        Serial.println("ms");
    }
    
    // Test audio output
    Serial.println("Testing audio output...");
    playTestSequence();
    
    Serial.println("=== Self Test Complete ===");
}

void AudibleSignalInterface::printDiagnostics() {
    Serial.println("=== Audible Signal Interface Diagnostics ===");
    Serial.print("Audio enabled: "); Serial.println(audioEnabled ? "YES" : "NO");
    Serial.print("Master volume: "); Serial.println(masterVolume);
    Serial.print("Active devices: "); Serial.println(deviceCount);
    Serial.print("Active signals: "); Serial.println(signalCount);
    Serial.print("Tone active: "); Serial.println(toneActive ? "YES" : "NO");
    Serial.print("Error flags: 0x"); Serial.println(errorFlags, HEX);
    
    if (toneActive) {
        Serial.print("Current tone: Pin=");
        Serial.print(currentTonePin);
        Serial.print(", Frequency=");
        Serial.print(currentFrequency);
        Serial.println("Hz");
    }
}

// Test functions
void AudibleSignalInterface::playTestSequence() {
    Serial.println("Playing test sequence...");
    
    // Play a sequence of test tones
    playCustomSignal(440, 200, 50);   // A4
    delay(300);
    playCustomSignal(523, 200, 50);   // C5
    delay(300);
    playCustomSignal(659, 200, 50);   // E5
    delay(300);
    playCustomSignal(880, 400, 50);   // A5
    delay(500);
}

void AudibleSignalInterface::playWarningSequence() {
    Serial.println("Playing warning sequence...");
    
    // Play warning tones
    for (int i = 0; i < 3; i++) {
        playCustomSignal(800, 300, 70);
        delay(400);
    }
}

void AudibleSignalInterface::playStatusSequence() {
    Serial.println("Playing status sequence...");
    
    // Play status tones
    playCustomSignal(1000, 100, 50);
    delay(200);
    playCustomSignal(1200, 100, 50);
    delay(200);
    playCustomSignal(1000, 100, 50);
} 