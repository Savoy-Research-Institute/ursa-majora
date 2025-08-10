#ifndef TIMER_MODULE_H
#define TIMER_MODULE_H

#include <Arduino.h>

// Timer types
enum class TimerType {
    TIMER0,
    TIMER1,
    TIMER2,
    TIMER3,
    TIMER4,
    TIMER5,
    SOFTWARE_TIMER,
    WATCHDOG_TIMER
};

// Timer modes
enum class TimerMode {
    NORMAL,
    CTC,           // Clear Timer on Compare Match
    FAST_PWM,
    PHASE_CORRECT_PWM,
    PHASE_FREQ_CORRECT_PWM,
    INPUT_CAPTURE
};

// Timer prescaler values
enum class TimerPrescaler {
    PRESCALER_1,
    PRESCALER_8,
    PRESCALER_64,
    PRESCALER_256,
    PRESCALER_1024,
    EXTERNAL_FALLING,
    EXTERNAL_RISING
};

// Timer configuration
struct TimerConfig {
    TimerType type;
    TimerMode mode;
    TimerPrescaler prescaler;
    uint16_t compareValue;   // Compare value for CTC mode
    uint16_t period;         // Period in microseconds
    bool interruptEnabled;    // Whether timer interrupt is enabled
    bool outputEnabled;       // Whether timer output is enabled
    uint8_t outputPin;       // Output pin for PWM
};

// Timer status
struct TimerStatus {
    bool running;
    bool overflow;
    bool compareMatch;
    uint16_t currentValue;
    uint16_t compareValue;
    uint32_t overflowCount;
    uint32_t compareCount;
    unsigned long lastUpdateTime;
    bool healthy;
};

// Software timer
struct SoftwareTimer {
    uint8_t id;
    uint32_t period;         // Period in milliseconds
    uint32_t lastTriggerTime;
    bool enabled;
    bool repeat;
    uint32_t repeatCount;
    uint32_t maxRepeats;
    void (*callback)(uint8_t id, void* data);
    void* callbackData;
};

class TimerModule {
private:
    static const uint8_t MAX_TIMERS = 8;
    static const uint8_t MAX_SOFTWARE_TIMERS = 16;
    
    // Hardware timers
    TimerConfig hardwareTimers[MAX_TIMERS];
    TimerStatus timerStatus[MAX_TIMERS];
    uint8_t hardwareTimerCount;
    
    // Software timers
    SoftwareTimer softwareTimers[MAX_SOFTWARE_TIMERS];
    uint8_t softwareTimerCount;
    
    // System state
    bool initialized;
    unsigned long systemStartTime;
    unsigned long lastUpdateTime;
    
    // Error handling
    uint8_t errorFlags;
    String lastError;
    
    // Private methods
    bool initializeHardwareTimer(TimerType type);
    bool configureHardwareTimer(TimerType type, const TimerConfig& config);
    bool setHardwareTimerPrescaler(TimerType type, TimerPrescaler prescaler);
    bool setHardwareTimerCompare(TimerType type, uint16_t compareValue);
    bool setHardwareTimerMode(TimerType type, TimerMode mode);
    void updateHardwareTimers();
    void updateSoftwareTimers();
    bool validateTimerConfig(const TimerConfig& config);
    bool validateSoftwareTimer(const SoftwareTimer& timer);
    uint32_t calculatePrescalerValue(TimerPrescaler prescaler);
    
public:
    TimerModule();
    
    // Hardware timer management
    bool addHardwareTimer(TimerType type);
    bool removeHardwareTimer(TimerType type);
    bool configureTimer(TimerType type, const TimerConfig& config);
    TimerConfig getTimerConfig(TimerType type) const;
    TimerStatus getTimerStatus(TimerType type) const;
    
    // Timer configuration
    bool setTimerMode(TimerType type, TimerMode mode);
    bool setTimerPrescaler(TimerType type, TimerPrescaler prescaler);
    bool setTimerCompare(TimerType type, uint16_t compareValue);
    bool setTimerPeriod(TimerType type, uint16_t period);
    bool setTimerOutput(TimerType type, uint8_t pin, bool enabled);
    
    // Timer control
    bool startTimer(TimerType type);
    bool stopTimer(TimerType type);
    bool resetTimer(TimerType type);
    bool isTimerRunning(TimerType type) const;
    
    // Software timer management
    uint8_t createSoftwareTimer(uint32_t period, void (*callback)(uint8_t id, void* data), void* data = nullptr);
    bool removeSoftwareTimer(uint8_t timerId);
    bool startSoftwareTimer(uint8_t timerId);
    bool stopSoftwareTimer(uint8_t timerId);
    bool resetSoftwareTimer(uint8_t timerId);
    bool setSoftwareTimerPeriod(uint8_t timerId, uint32_t period);
    bool setSoftwareTimerRepeat(uint8_t timerId, bool repeat, uint32_t maxRepeats = 0);
    
    // Timing functions
    unsigned long getMicroseconds() const;
    unsigned long getMilliseconds() const;
    unsigned long getSeconds() const;
    unsigned long getUptime() const;
    void delayMicroseconds(uint32_t microseconds);
    void delayMilliseconds(uint32_t milliseconds);
    
    // PWM control
    bool setPWMFrequency(TimerType type, uint32_t frequency);
    bool setPWMDutyCycle(TimerType type, uint8_t dutyCycle);
    bool setPWMPin(TimerType type, uint8_t pin);
    
    // Interrupt management
    bool enableTimerInterrupt(TimerType type);
    bool disableTimerInterrupt(TimerType type);
    bool isTimerInterruptEnabled(TimerType type) const;
    
    // Status and monitoring
    uint8_t getHardwareTimerCount() const { return hardwareTimerCount; }
    uint8_t getSoftwareTimerCount() const { return softwareTimerCount; }
    bool isTimerOverflow(TimerType type) const;
    bool isTimerCompareMatch(TimerType type) const;
    uint32_t getTimerOverflowCount(TimerType type) const;
    uint32_t getTimerCompareCount(TimerType type) const;
    
    // System control
    bool initialize();
    void update();
    void shutdown();
    bool performSelfTest();
    
    // Status and health
    bool isInitialized() const { return initialized; }
    uint8_t getErrorFlags() const { return errorFlags; }
    String getLastError() const { return lastError; }
    void clearErrors();
    
    // Diagnostics
    void printStatus();
    void printHardwareTimers();
    void printSoftwareTimers();
    void printConfiguration();
    
    // Hardware-specific methods
    bool setTimerPin(uint8_t timerId, uint8_t pin);
    bool setTimerFrequency(TimerType type, uint32_t frequency);
    bool setTimerDutyCycle(TimerType type, uint8_t dutyCycle);
    
    // Utility methods
    uint32_t frequencyToPeriod(uint32_t frequency) const;
    uint32_t periodToFrequency(uint32_t period) const;
    uint8_t calculateDutyCycle(uint16_t compareValue, uint16_t maxValue) const;
    uint16_t calculateCompareValue(uint8_t dutyCycle, uint16_t maxValue) const;
    
    // Advanced features
    bool synchronizeTimers(TimerType timer1, TimerType timer2);
    bool setTimerPhase(TimerType type, uint16_t phase);
    bool enableTimerOutput(TimerType type, bool enabled);
};

#endif // TIMER_MODULE_H 