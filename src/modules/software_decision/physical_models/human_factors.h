#ifndef HUMAN_FACTORS_H
#define HUMAN_FACTORS_H

#include <Arduino.h>
#include "../application_data_types/numeric_types.h"

// Human factors states
enum class HumanFactorsState {
    IDLE,
    ACTIVE,
    ANALYZING,
    OPTIMIZING,
    ERROR
};

// Pilot experience levels
enum class PilotExperienceLevel {
    NOVICE,
    BEGINNER,
    INTERMEDIATE,
    ADVANCED,
    EXPERT
};

// Workload levels
enum class WorkloadLevel {
    VERY_LOW,
    LOW,
    MODERATE,
    HIGH,
    VERY_HIGH,
    CRITICAL
};

// Human performance metrics
enum class PerformanceMetric {
    RESPONSE_TIME,
    ACCURACY,
    WORKLOAD,
    FATIGUE,
    STRESS,
    SITUATION_AWARENESS,
    DECISION_QUALITY,
    ERROR_RATE
};

// Human factors configuration
struct HumanFactorsConfig {
    PilotExperienceLevel experienceLevel;
    uint8_t age;
    bool hasVisionCorrection;
    bool hasHearingCorrection;
    uint8_t fatigueLevel;        // 0-100
    uint8_t stressLevel;         // 0-100
    uint8_t workloadLevel;       // 0-100
    bool isNightOperation;
    bool isEmergencySituation;
    uint16_t operationDuration;  // minutes
    uint8_t breakIntervals;      // minutes
};

// Human performance data
struct HumanPerformanceData {
    uint8_t pilotId;
    PilotExperienceLevel experienceLevel;
    WorkloadLevel currentWorkload;
    uint16_t responseTime;       // ms
    double accuracy;             // 0.0-1.0
    uint8_t fatigueLevel;        // 0-100
    uint8_t stressLevel;         // 0-100
    double situationAwareness;   // 0.0-1.0
    double decisionQuality;      // 0.0-1.0
    uint8_t errorRate;           // errors per hour
    unsigned long timestamp;
    String notes;
};

// Interface characteristics
struct InterfaceCharacteristics {
    uint8_t interfaceId;
    String interfaceType;
    double complexity;           // 0.0-1.0
    double usability;            // 0.0-1.0
    double accessibility;        // 0.0-1.0
    double learnability;         // 0.0-1.0
    double efficiency;           // 0.0-1.0
    double satisfaction;         // 0.0-1.0
    uint16_t responseTime;       // ms
    uint8_t errorRate;           // errors per hour
    String recommendations;
};

// Human factors event types
enum class HumanFactorsEventType {
    WORKLOAD_INCREASED,
    WORKLOAD_DECREASED,
    FATIGUE_DETECTED,
    STRESS_DETECTED,
    ERROR_OCCURRED,
    PERFORMANCE_DEGRADED,
    INTERFACE_OPTIMIZED,
    TRAINING_REQUIRED,
    BREAK_RECOMMENDED,
    EMERGENCY_SITUATION
};

// Human factors event structure
struct HumanFactorsEvent {
    HumanFactorsEventType type;
    String description;
    unsigned long timestamp;
    uint8_t severity;
    bool acknowledged;
    uint8_t pilotId;
    String recommendation;
};

// Response time model
struct ResponseTimeModel {
    uint16_t baselineResponseTime;   // ms
    uint16_t complexityMultiplier;   // ms per complexity unit
    uint16_t fatigueMultiplier;      // ms per fatigue unit
    uint16_t stressMultiplier;       // ms per stress unit
    uint16_t experienceMultiplier;   // ms per experience level
    uint16_t workloadMultiplier;     // ms per workload unit
};

// Workload model
struct WorkloadModel {
    uint8_t baselineWorkload;        // 0-100
    uint8_t taskComplexityWeight;    // 0-100
    uint8_t timePressureWeight;      // 0-100
    uint8_t environmentalStressWeight; // 0-100
    uint8_t fatigueWeight;           // 0-100
    uint8_t experienceWeight;        // 0-100
};

// Error probability model
struct ErrorProbabilityModel {
    double baselineErrorRate;        // errors per hour
    double complexityMultiplier;     // multiplier per complexity unit
    double fatigueMultiplier;        // multiplier per fatigue unit
    double stressMultiplier;         // multiplier per stress unit
    double experienceMultiplier;     // multiplier per experience level
    double workloadMultiplier;       // multiplier per workload unit
    double timeMultiplier;           // multiplier per hour of operation
};

class HumanFactors {
private:
    // Current state
    HumanFactorsState currentState;
    
    // Human performance data
    static const uint8_t MAX_PILOTS = 8;
    HumanPerformanceData pilots[MAX_PILOTS];
    uint8_t pilotCount;
    
    // Interface characteristics
    static const uint8_t MAX_INTERFACES = 16;
    InterfaceCharacteristics interfaces[MAX_INTERFACES];
    uint8_t interfaceCount;
    
    // Event management
    static const uint8_t MAX_EVENTS = 32;
    HumanFactorsEvent events[MAX_EVENTS];
    uint8_t eventCount;
    uint8_t eventIndex;
    
    // Timing
    unsigned long lastUpdate;
    unsigned long lastAnalysis;
    unsigned long humanFactorsStartTime;
    
    // Error tracking
    uint8_t errorCount;
    uint8_t warningCount;
    uint8_t criticalErrorCount;
    
    // Models
    ResponseTimeModel responseTimeModel;
    WorkloadModel workloadModel;
    ErrorProbabilityModel errorProbabilityModel;
    
    // Analysis data
    bool analysisInProgress;
    unsigned long analysisStartTime;
    uint16_t analysisSamples;
    double analysisData[100][8]; // [samples][response_time, accuracy, workload, fatigue, stress, awareness, decision, error_rate]
    
    // Component interfaces
    NumericTypes* numericTypes;

public:
    HumanFactors();
    ~HumanFactors();
    
    // Initialization and control
    bool initialize();
    void startup();
    void shutdown();
    void reset();
    
    // State management
    HumanFactorsState getCurrentState() const { return currentState; }
    void changeState(HumanFactorsState newState);
    
    // Pilot management
    uint8_t addPilot(const HumanFactorsConfig& config);
    bool removePilot(uint8_t pilotId);
    bool updatePilotData(uint8_t pilotId, const HumanPerformanceData& data);
    uint8_t getPilotCount() const { return pilotCount; }
    void getPilotData(uint8_t index, HumanPerformanceData& data) const;
    bool isPilotValid(uint8_t pilotId) const;
    
    // Interface management
    uint8_t addInterface(const String& interfaceType);
    bool removeInterface(uint8_t interfaceId);
    bool updateInterfaceCharacteristics(uint8_t interfaceId, const InterfaceCharacteristics& characteristics);
    uint8_t getInterfaceCount() const { return interfaceCount; }
    void getInterfaceCharacteristics(uint8_t index, InterfaceCharacteristics& characteristics) const;
    bool isInterfaceValid(uint8_t interfaceId) const;
    
    // Human performance analysis
    uint16_t calculateResponseTime(uint8_t pilotId, double taskComplexity);
    WorkloadLevel calculateWorkload(uint8_t pilotId, double taskComplexity, double timePressure);
    double calculateErrorProbability(uint8_t pilotId, double taskComplexity, uint16_t operationTime);
    double calculateSituationAwareness(uint8_t pilotId, double informationLoad);
    double calculateDecisionQuality(uint8_t pilotId, double decisionComplexity, double timePressure);
    
    // Performance prediction
    bool predictPerformance(uint8_t pilotId, double taskComplexity, uint16_t duration, HumanPerformanceData& prediction);
    bool predictWorkload(uint8_t pilotId, double taskComplexity, double timePressure, WorkloadLevel& predictedWorkload);
    bool predictErrorRate(uint8_t pilotId, double taskComplexity, uint16_t operationTime, double& predictedErrorRate);
    
    // Fatigue and stress analysis
    uint8_t calculateFatigueLevel(uint8_t pilotId, uint16_t operationTime, uint8_t sleepQuality);
    uint8_t calculateStressLevel(uint8_t pilotId, double taskComplexity, bool emergencySituation);
    bool detectFatigue(uint8_t pilotId);
    bool detectStress(uint8_t pilotId);
    bool recommendBreak(uint8_t pilotId);
    
    // Interface optimization
    bool optimizeInterface(uint8_t interfaceId, uint8_t pilotId);
    bool evaluateInterfaceUsability(uint8_t interfaceId, uint8_t pilotId);
    bool generateInterfaceRecommendations(uint8_t interfaceId, String& recommendations);
    bool validateInterfaceDesign(uint8_t interfaceId, bool& isValid);
    
    // Training and experience
    bool assessTrainingNeeds(uint8_t pilotId, String& trainingRecommendations);
    bool calculateExperienceMultiplier(PilotExperienceLevel level, double& multiplier);
    bool recommendExperienceLevel(uint8_t pilotId, PilotExperienceLevel& recommendedLevel);
    
    // Model configuration
    void setResponseTimeModel(const ResponseTimeModel& model);
    void setWorkloadModel(const WorkloadModel& model);
    void setErrorProbabilityModel(const ErrorProbabilityModel& model);
    ResponseTimeModel getResponseTimeModel() const { return responseTimeModel; }
    WorkloadModel getWorkloadModel() const { return workloadModel; }
    ErrorProbabilityModel getErrorProbabilityModel() const { return errorProbabilityModel; }
    
    // Error handling
    void addEvent(HumanFactorsEventType type, const String& description, uint8_t severity = 128);
    uint8_t getErrorCount() const { return errorCount; }
    uint8_t getWarningCount() const { return warningCount; }
    uint8_t getCriticalErrorCount() const { return criticalErrorCount; }
    
    // Event management
    uint8_t getEventCount() const { return eventCount; }
    void getEvent(uint8_t index, HumanFactorsEvent& event) const;
    void acknowledgeEvent(uint8_t index);
    void clearEvents();
    
    // Component interfaces
    void setNumericTypes(NumericTypes* types) { numericTypes = types; }
    
    // Main update loop
    void update();
    
    // Diagnostics and testing
    void performDiagnostics();
    bool isHumanFactorsHealthy() const;
    void runHumanFactorsTest();
    void testResponseTimeModel();
    void testWorkloadModel();
    void testErrorProbabilityModel();
    
    // Analysis and optimization
    void startAnalysis();
    void stopAnalysis();
    bool isAnalyzing() const { return analysisInProgress; }
    bool addAnalysisSample(uint8_t pilotId, const HumanPerformanceData& data);
    double getAnalysisProgress() const;
    bool generateOptimizationReport(String& report);
    
    // Utility functions
    bool validatePilotConfig(const HumanFactorsConfig& config) const;
    bool validateInterfaceCharacteristics(const InterfaceCharacteristics& characteristics) const;
    void updatePerformanceMetrics();
    void logHumanFactorsData();
    
    // Status and information
    String getHumanFactorsStatus() const;
    void printHumanFactorsStatus();
    void printPilotData();
    void printInterfaceCharacteristics();
    void printEventLog();
    void exportHumanFactorsData(const String& filename);
    
    // Recovery and maintenance
    void performRecovery();
    void enterMaintenanceMode();
    void exitMaintenanceMode();
    void clearHumanFactorsHistory();
    void resetHumanFactorsCounters();
    
    // Statistical analysis
    double calculateAverageResponseTime() const;
    double calculateAverageWorkload() const;
    double calculateAverageErrorRate() const;
    double calculatePerformanceCorrelation(uint8_t pilotId1, uint8_t pilotId2) const;
    bool generatePerformanceReport(String& report);
};

#endif // HUMAN_FACTORS_H 