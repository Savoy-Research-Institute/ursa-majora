#ifndef INTERRUPT_HANDLER_H
#define INTERRUPT_HANDLER_H

#include <Arduino.h>

// Interrupt types
enum class InterruptType {
    EXTERNAL_INT0,
    EXTERNAL_INT1,
    TIMER0_OVF,
    TIMER1_OVF,
    TIMER2_OVF,
    TIMER0_COMPA,
    TIMER0_COMPB,
    TIMER1_COMPA,
    TIMER1_COMPB,
    TIMER2_COMPA,
    TIMER2_COMPB,
    SPI_STC,
    USART_RX,
    USART_UDRE,
    USART_TX,
    ANALOG_COMP,
    ADC_COMPLETE,
    EEPROM_READY,
    WATCHDOG_TIMER,
    CUSTOM_INTERRUPT
};

// Interrupt priority levels
enum class InterruptPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};

// Interrupt states
enum class InterruptState {
    DISABLED,
    ENABLED,
    PENDING,
    EXECUTING,
    COMPLETED,
    ERROR
};

// Interrupt configuration
struct InterruptConfig {
    InterruptType type;
    InterruptPriority priority;
    bool enabled;
    bool autoRestart;
    uint16_t timeout;        // Timeout in milliseconds
    uint8_t maxRetries;      // Maximum retry attempts
    bool loggingEnabled;      // Whether to log interrupt events
};

// Interrupt event
struct InterruptEvent {
    InterruptType type;
    unsigned long timestamp;
    uint8_t source;          // Source identifier
    String description;
    InterruptState state;
    uint8_t retryCount;
    bool handled;
};

// Interrupt statistics
struct InterruptStats {
    uint32_t totalInterrupts;
    uint32_t handledInterrupts;
    uint32_t timeoutInterrupts;
    uint32_t errorInterrupts;
    uint32_t retryInterrupts;
    unsigned long lastInterruptTime;
    unsigned long averageResponseTime;
    uint8_t maxResponseTime;
    uint8_t minResponseTime;
};

class InterruptHandler {
private:
    static const uint8_t MAX_INTERRUPTS = 32;
    static const uint8_t MAX_EVENTS = 64;
    
    // Interrupt configurations
    InterruptConfig interrupts[MAX_INTERRUPTS];
    uint8_t interruptCount;
    
    // Interrupt events
    InterruptEvent events[MAX_EVENTS];
    uint8_t eventCount;
    uint8_t eventIndex;
    
    // Interrupt handlers
    typedef void (*InterruptCallback)(uint8_t source, void* data);
    InterruptCallback callbacks[MAX_INTERRUPTS];
    void* callbackData[MAX_INTERRUPTS];
    
    // Statistics
    InterruptStats stats;
    
    // System state
    bool initialized;
    bool globalInterruptsEnabled;
    uint8_t errorFlags;
    String lastError;
    
    // Timing
    unsigned long lastUpdateTime;
    unsigned long lastStatsTime;
    
    // Private methods
    bool validateInterruptType(InterruptType type);
    bool validatePriority(InterruptPriority priority);
    void handleInterrupt(InterruptType type, uint8_t source);
    void logInterruptEvent(const InterruptEvent& event);
    void updateStatistics(const InterruptEvent& event);
    bool enableHardwareInterrupt(InterruptType type);
    bool disableHardwareInterrupt(InterruptType type);
    void processPendingInterrupts();
    
public:
    InterruptHandler();
    
    // Interrupt management
    bool registerInterrupt(InterruptType type, InterruptPriority priority = InterruptPriority::NORMAL);
    bool unregisterInterrupt(InterruptType type);
    bool enableInterrupt(InterruptType type);
    bool disableInterrupt(InterruptType type);
    bool isInterruptEnabled(InterruptType type) const;
    
    // Interrupt configuration
    bool configureInterrupt(InterruptType type, const InterruptConfig& config);
    InterruptConfig getInterruptConfig(InterruptType type) const;
    bool setInterruptPriority(InterruptType type, InterruptPriority priority);
    bool setInterruptTimeout(InterruptType type, uint16_t timeout);
    bool setInterruptRetries(InterruptType type, uint8_t maxRetries);
    bool setInterruptLogging(InterruptType type, bool enabled);
    
    // Callback management
    bool setInterruptCallback(InterruptType type, InterruptCallback callback, void* data = nullptr);
    bool removeInterruptCallback(InterruptType type);
    
    // Interrupt control
    bool enableGlobalInterrupts();
    bool disableGlobalInterrupts();
    bool areGlobalInterruptsEnabled() const { return globalInterruptsEnabled; }
    
    // Event management
    uint8_t getEventCount() const { return eventCount; }
    bool getEvent(uint8_t index, InterruptEvent& event) const;
    void clearEvents();
    void acknowledgeEvent(uint8_t index);
    
    // Statistics
    InterruptStats getStatistics() const { return stats; }
    void resetStatistics();
    void printStatistics();
    
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
    void printInterrupts();
    void printEvents();
    void printConfiguration();
    
    // Hardware-specific methods
    bool attachExternalInterrupt(uint8_t pin, int mode);
    bool detachExternalInterrupt(uint8_t pin);
    bool setTimerInterrupt(uint8_t timer, uint16_t period);
    bool clearTimerInterrupt(uint8_t timer);
    
    // Utility methods
    bool hasPendingInterrupts() const;
    uint8_t getActiveInterruptCount() const;
    bool isInterruptPending(InterruptType type) const;
    
    // Emergency handling
    void emergencyDisableAll();
    void emergencyRestore();
    bool isEmergencyMode() const;
};

#endif // INTERRUPT_HANDLER_H 