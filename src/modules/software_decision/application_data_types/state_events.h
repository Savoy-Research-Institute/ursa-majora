#ifndef STATE_EVENTS_H
#define STATE_EVENTS_H

#include <Arduino.h>

// Event types
enum class EventType {
    SYSTEM_EVENT,
    MODE_CHANGE,
    STATE_TRANSITION,
    ERROR_EVENT,
    WARNING_EVENT,
    CALIBRATION_EVENT,
    SENSOR_EVENT,
    COMMUNICATION_EVENT,
    USER_INPUT_EVENT,
    TIMER_EVENT,
    INTERRUPT_EVENT,
    EMERGENCY_EVENT,
    MAINTENANCE_EVENT,
    TEST_EVENT,
    LOGGING_EVENT,
    CONFIGURATION_EVENT
};

// Event severity levels
enum class EventSeverity {
    INFO,
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL,
    EMERGENCY
};

// Event categories
enum class EventCategory {
    FLIGHT_CONTROL,
    NAVIGATION,
    COMMUNICATION,
    WEAPONS,
    SENSORS,
    POWER,
    SAFETY,
    MAINTENANCE,
    SYSTEM,
    USER_INTERFACE
};

// Event source types
enum class EventSource {
    SYSTEM,
    SENSOR,
    USER,
    EXTERNAL,
    TIMER,
    INTERRUPT,
    CALIBRATION,
    TEST,
    MAINTENANCE,
    EMERGENCY
};

// Event status
enum class EventStatus {
    PENDING,
    ACTIVE,
    ACKNOWLEDGED,
    RESOLVED,
    CANCELLED,
    EXPIRED,
    ERROR
};

// Event priority
enum class EventPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL,
    EMERGENCY
};

// State transition types
enum class StateTransitionType {
    AUTOMATIC,
    MANUAL,
    TIMED,
    CONDITIONAL,
    EMERGENCY,
    MAINTENANCE,
    TEST
};

// State transition triggers
enum class StateTransitionTrigger {
    TIME_ELAPSED,
    CONDITION_MET,
    USER_COMMAND,
    SYSTEM_COMMAND,
    ERROR_DETECTED,
    CALIBRATION_COMPLETE,
    TEST_COMPLETE,
    MAINTENANCE_COMPLETE,
    EMERGENCY_ACTIVATED,
    EXTERNAL_SIGNAL
};

// Event data structure
struct EventData {
    EventType type;
    EventSeverity severity;
    EventCategory category;
    EventSource source;
    EventStatus status;
    EventPriority priority;
    
    String description;
    String details;
    uint8_t sourceId;
    uint8_t targetId;
    
    unsigned long timestamp;
    unsigned long duration;
    uint8_t retryCount;
    uint8_t maxRetries;
    
    bool acknowledged;
    bool logged;
    bool persistent;
    
    // Additional data fields
    float numericValue;
    String stringValue;
    bool booleanValue;
    uint8_t dataArray[8];
};

// State transition data
struct StateTransitionData {
    String fromState;
    String toState;
    StateTransitionType type;
    StateTransitionTrigger trigger;
    
    unsigned long timestamp;
    unsigned long duration;
    bool successful;
    
    String reason;
    String conditions;
    uint8_t priority;
    
    bool automatic;
    bool reversible;
    bool logged;
};

// Event filter criteria
struct EventFilter {
    EventType type;
    EventSeverity minSeverity;
    EventCategory category;
    EventSource source;
    EventStatus status;
    EventPriority minPriority;
    
    unsigned long startTime;
    unsigned long endTime;
    String searchText;
    
    bool includeAcknowledged;
    bool includeResolved;
    bool includeExpired;
};

// Event statistics
struct EventStatistics {
    uint32_t totalEvents;
    uint32_t eventsByType[16];      // Assuming max 16 event types
    uint32_t eventsBySeverity[6];   // 6 severity levels
    uint32_t eventsByCategory[10];  // 10 categories
    uint32_t eventsBySource[10];    // 10 source types
    
    uint32_t pendingEvents;
    uint32_t activeEvents;
    uint32_t acknowledgedEvents;
    uint32_t resolvedEvents;
    
    unsigned long lastEventTime;
    unsigned long averageEventDuration;
    uint8_t maxEventDuration;
    uint8_t minEventDuration;
};

// Event callback function type
typedef void (*EventCallback)(const EventData& event, void* userData);

// Event listener structure
struct EventListener {
    EventCallback callback;
    void* userData;
    EventFilter filter;
    bool active;
    uint8_t priority;
    String name;
};

// Event queue item
struct EventQueueItem {
    EventData event;
    unsigned long queueTime;
    uint8_t priority;
    bool processed;
    EventListener* listener;
};

// Event management class
class EventManager {
private:
    static const uint8_t MAX_EVENTS = 128;
    static const uint8_t MAX_LISTENERS = 32;
    static const uint8_t MAX_QUEUE_ITEMS = 64;
    
    // Event storage
    EventData events[MAX_EVENTS];
    uint8_t eventCount;
    uint8_t eventIndex;
    
    // Event listeners
    EventListener listeners[MAX_LISTENERS];
    uint8_t listenerCount;
    
    // Event queue
    EventQueueItem eventQueue[MAX_QUEUE_ITEMS];
    uint8_t queueHead;
    uint8_t queueTail;
    uint8_t queueLength;
    
    // Statistics
    EventStatistics statistics;
    
    // System state
    bool initialized;
    bool eventLoggingEnabled;
    unsigned long lastUpdateTime;
    
    // Private methods
    void addEventToHistory(const EventData& event);
    void processEventQueue();
    void notifyListeners(const EventData& event);
    bool matchesFilter(const EventData& event, const EventFilter& filter);
    void updateStatistics(const EventData& event);
    void cleanupExpiredEvents();
    
public:
    EventManager();
    
    // Event creation and management
    uint8_t createEvent(EventType type, EventSeverity severity, EventCategory category, 
                       EventSource source, const String& description);
    bool updateEvent(uint8_t eventId, const EventData& updates);
    bool acknowledgeEvent(uint8_t eventId);
    bool resolveEvent(uint8_t eventId);
    bool cancelEvent(uint8_t eventId);
    
    // Event querying
    uint8_t getEventCount() const { return eventCount; }
    bool getEvent(uint8_t eventId, EventData& event) const;
    uint8_t findEvents(const EventFilter& filter, EventData* results, uint8_t maxResults) const;
    uint8_t getPendingEvents(EventData* results, uint8_t maxResults) const;
    uint8_t getActiveEvents(EventData* results, uint8_t maxResults) const;
    
    // Event listener management
    uint8_t addEventListener(EventCallback callback, void* userData, const EventFilter& filter);
    bool removeEventListener(uint8_t listenerId);
    bool enableEventListener(uint8_t listenerId);
    bool disableEventListener(uint8_t listenerId);
    bool updateEventListenerFilter(uint8_t listenerId, const EventFilter& filter);
    
    // State transition management
    bool recordStateTransition(const String& fromState, const String& toState, 
                             StateTransitionType type, StateTransitionTrigger trigger);
    bool getStateTransitionHistory(String* transitions, uint8_t maxTransitions) const;
    
    // Statistics and monitoring
    EventStatistics getStatistics() const { return statistics; }
    void resetStatistics();
    void printStatistics();
    
    // System control
    bool initialize();
    void update();
    void shutdown();
    bool performSelfTest();
    
    // Configuration
    bool enableEventLogging(bool enabled);
    bool setEventRetentionPeriod(unsigned long period);
    bool setMaxEventCount(uint8_t maxCount);
    
    // Utility methods
    bool hasPendingEvents() const;
    bool hasActiveEvents() const;
    bool hasCriticalEvents() const;
    bool hasEmergencyEvents() const;
    
    // Error handling
    uint8_t getErrorCount() const;
    void clearErrors();
    
    // Diagnostics
    void printStatus();
    void printEvents();
    void printListeners();
    void printQueue();
};

#endif // STATE_EVENTS_H 