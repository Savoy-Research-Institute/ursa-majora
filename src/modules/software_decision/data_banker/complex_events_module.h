#ifndef COMPLEX_EVENTS_MODULE_H
#define COMPLEX_EVENTS_MODULE_H

#include <Arduino.h>
#include "../application_data_types/state_events.h"
#include "../application_data_types/numeric_types.h"

// Event correlation types
enum class CorrelationType {
    SEQUENTIAL,
    PARALLEL,
    CONDITIONAL,
    TIMED,
    PATTERN,
    THRESHOLD,
    TREND,
    ANOMALY
};

// Event priority levels
enum class EventPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL,
    EMERGENCY
};

// Event status
enum class EventStatus {
    PENDING,
    ACTIVE,
    COMPLETED,
    CANCELLED,
    FAILED,
    TIMEOUT
};

// Event correlation rule
struct CorrelationRule {
    String ruleId;
    String name;
    String description;
    CorrelationType type;
    EventPriority priority;
    bool enabled;
    
    // Rule conditions
    String* triggerEvents;
    uint16_t triggerCount;
    String* requiredEvents;
    uint16_t requiredCount;
    String* excludedEvents;
    uint16_t excludedCount;
    
    // Timing constraints
    uint32_t minDelay;
    uint32_t maxDelay;
    uint32_t timeout;
    bool strictOrdering;
    
    // Action configuration
    String actionType;
    String actionParameters;
    uint8_t retryCount;
    uint32_t retryDelay;
    
    // Rule metadata
    unsigned long creationTime;
    unsigned long lastTriggered;
    uint16_t triggerCount;
    bool persistent;
};

// Complex event
struct ComplexEvent {
    String eventId;
    String ruleId;
    String name;
    String description;
    CorrelationType correlationType;
    EventPriority priority;
    EventStatus status;
    
    // Event data
    String* componentEvents;
    uint16_t componentCount;
    String eventData;
    uint32_t dataSize;
    
    // Timing information
    unsigned long creationTime;
    unsigned long startTime;
    unsigned long completionTime;
    uint32_t duration;
    
    // Event metadata
    String source;
    String category;
    uint8_t severity;
    bool acknowledged;
    String notes;
};

// Event pattern
struct EventPattern {
    String patternId;
    String name;
    String description;
    CorrelationType type;
    bool enabled;
    
    // Pattern definition
    String* eventSequence;
    uint16_t sequenceLength;
    uint32_t* timeConstraints;
    uint16_t constraintCount;
    
    // Pattern matching
    float confidence;
    uint16_t matchCount;
    unsigned long lastMatch;
    bool continuous;
};

// Event analysis result
struct EventAnalysisResult {
    String analysisId;
    String eventId;
    String analysisType;
    float confidence;
    String conclusion;
    
    // Analysis data
    String* supportingEvents;
    uint16_t supportCount;
    String* conflictingEvents;
    uint16_t conflictCount;
    
    // Analysis metadata
    unsigned long analysisTime;
    uint32_t processingTime;
    String analyst;
    bool verified;
};

// Complex events configuration
struct ComplexEventsConfig {
    uint16_t maxEvents;
    uint16_t maxRules;
    uint16_t maxPatterns;
    uint16_t maxAnalyses;
    uint32_t defaultTimeout;
    bool enablePersistence;
    bool enableLogging;
    uint16_t logInterval;
    bool enableRealTimeAnalysis;
    uint16_t analysisInterval;
};

class ComplexEventsModule {
private:
    static const uint16_t MAX_COMPLEX_EVENTS = 256;
    static const uint16_t MAX_CORRELATION_RULES = 128;
    static const uint16_t MAX_EVENT_PATTERNS = 64;
    static const uint16_t MAX_ANALYSIS_RESULTS = 128;
    
    // Event storage
    ComplexEvent events[MAX_COMPLEX_EVENTS];
    uint16_t eventCount;
    
    // Correlation rules
    CorrelationRule rules[MAX_CORRELATION_RULES];
    uint16_t ruleCount;
    
    // Event patterns
    EventPattern patterns[MAX_EVENT_PATTERNS];
    uint16_t patternCount;
    
    // Analysis results
    EventAnalysisResult analyses[MAX_ANALYSIS_RESULTS];
    uint16_t analysisCount;
    
    // Configuration
    ComplexEventsConfig config;
    
    // Timing and intervals
    unsigned long lastUpdateTime;
    unsigned long lastAnalysisTime;
    unsigned long lastLogTime;
    
    // Error tracking
    uint8_t errorFlags;
    String lastError;
    
    // Helper methods
    uint16_t findEventIndex(const String& eventId) const;
    uint16_t findRuleIndex(const String& ruleId) const;
    uint16_t findPatternIndex(const String& patternId) const;
    uint16_t findAnalysisIndex(const String& analysisId) const;
    
    bool evaluateCorrelationRule(const CorrelationRule& rule, const String& triggerEvent);
    bool checkEventConditions(const CorrelationRule& rule, const String* events, uint16_t count);
    bool checkTimingConstraints(const CorrelationRule& rule, unsigned long eventTime);
    
    void createComplexEvent(const CorrelationRule& rule, const String& triggerEvent);
    void updateEventStatus(const String& eventId, EventStatus status);
    void processEventPatterns(const String& componentEvent);
    
    bool persistEvent(const ComplexEvent& event);
    bool loadPersistedEvent(ComplexEvent& event);
    void logEventOperation(const String& operation, const String& eventId, const String& details);

public:
    ComplexEventsModule();
    ~ComplexEventsModule();
    
    // Initialization and configuration
    bool initialize(const ComplexEventsConfig& config = ComplexEventsConfig{});
    void setConfiguration(const ComplexEventsConfig& newConfig);
    ComplexEventsConfig getConfiguration() const { return config; }
    
    // Correlation rule management
    bool addCorrelationRule(const CorrelationRule& rule);
    bool updateCorrelationRule(const String& ruleId, const CorrelationRule& newRule);
    bool removeCorrelationRule(const String& ruleId);
    bool enableCorrelationRule(const String& ruleId, bool enable);
    
    CorrelationRule getCorrelationRule(const String& ruleId) const;
    bool isCorrelationRuleEnabled(const String& ruleId) const;
    uint16_t getCorrelationRuleCount() const { return ruleCount; }
    
    // Event pattern management
    bool addEventPattern(const EventPattern& pattern);
    bool updateEventPattern(const String& patternId, const EventPattern& newPattern);
    bool removeEventPattern(const String& patternId);
    bool enableEventPattern(const String& patternId, bool enable);
    
    EventPattern getEventPattern(const String& patternId) const;
    bool isEventPatternEnabled(const String& patternId) const;
    uint16_t getEventPatternCount() const { return patternCount; }
    
    // Complex event processing
    bool processComponentEvent(const String& componentEvent, const String& eventData = "");
    bool createComplexEvent(const String& ruleId, const String* componentEvents, uint16_t count);
    bool updateComplexEvent(const String& eventId, const String& field, const String& value);
    
    ComplexEvent getComplexEvent(const String& eventId) const;
    bool isComplexEventActive(const String& eventId) const;
    EventStatus getEventStatus(const String& eventId) const;
    uint16_t getComplexEventCount() const { return eventCount; }
    
    // Event correlation and analysis
    bool correlateEvents(const String* events, uint16_t count, String& correlationId);
    bool analyzeEventPattern(const String& eventId, String& analysisId);
    bool detectAnomalies(const String& eventId, String& anomalyId);
    bool predictEvents(const String& patternId, String& predictionId);
    
    // Event response and actions
    bool acknowledgeEvent(const String& eventId, const String& acknowledger);
    bool escalateEvent(const String& eventId, EventPriority newPriority);
    bool assignEvent(const String& eventId, const String& assignee);
    bool addEventNote(const String& eventId, const String& note);
    
    // Event search and filtering
    bool findEventsByStatus(EventStatus status, String* eventIds, uint16_t& count) const;
    bool findEventsByPriority(EventPriority priority, String* eventIds, uint16_t& count) const;
    bool findEventsBySource(const String& source, String* eventIds, uint16_t& count) const;
    bool findEventsByCategory(const String& category, String* eventIds, uint16_t& count) const;
    bool findEventsByTimeRange(unsigned long startTime, unsigned long endTime, String* eventIds, uint16_t& count) const;
    
    // Event statistics and monitoring
    uint16_t getEventCountByStatus(EventStatus status) const;
    uint16_t getEventCountByPriority(EventPriority priority) const;
    uint16_t getEventCountBySource(const String& source) const;
    uint16_t getEventCountByCategory(const String& category) const;
    
    float getAverageEventDuration() const;
    uint32_t getLongestEventDuration() const;
    uint32_t getShortestEventDuration() const;
    
    // Pattern matching and recognition
    bool matchEventPattern(const String& patternId, const String* events, uint16_t count, float& confidence);
    bool learnEventPattern(const String& patternId, const String* events, uint16_t count);
    bool updatePatternConfidence(const String& patternId, float confidence);
    
    // Real-time analysis
    bool enableRealTimeAnalysis(bool enable);
    bool setAnalysisInterval(uint16_t interval);
    bool performRealTimeAnalysis();
    
    // System operations
    void update();
    void shutdown();
    bool performSelfTest();
    
    // Status and monitoring
    bool isHealthy() const;
    uint8_t getErrorFlags() const { return errorFlags; }
    String getLastError() const { return lastError; }
    void clearErrors();
    
    // Utility methods
    void printAllEvents();
    void printEvent(const String& eventId);
    void printCorrelationRules();
    void printEventPatterns();
    void printAnalysisResults();
    void printConfiguration();
    void printStatistics();
    
    // Cleanup and maintenance
    bool cleanupCompletedEvents(uint16_t maxAge);
    bool cleanupExpiredEvents();
    bool cleanupOldAnalyses(uint16_t maxAge);
    bool validateAllRules();
    
    // Persistence
    bool saveAllEvents();
    bool loadAllEvents();
    bool exportEvents(const String& filename);
    bool importEvents(const String& filename);
    
    // Performance optimization
    bool optimizeEventProcessing();
    bool defragmentEventStorage();
    bool optimizePatternMatching();
};

#endif // COMPLEX_EVENTS_MODULE_H 