#ifndef GROUND_TEST_MODULE_H
#define GROUND_TEST_MODULE_H

#include <Arduino.h>
#include "../../hardware_hiding/device_interface/air_data_interface.h"
#include "../../hardware_hiding/device_interface/inertial_measurement_interface.h"
#include "../../software_decision/application_data_types/state_events.h"

// Test states
enum class TestState {
    IDLE,
    INITIALIZING,
    RUNNING,
    PAUSED,
    COMPLETED,
    FAILED,
    ABORTED
};

// Test types
enum class TestType {
    SYSTEM_CHECK,
    SENSOR_CALIBRATION,
    COMMUNICATION_TEST,
    POWER_TEST,
    MOTOR_TEST,
    CONTROL_TEST,
    NAVIGATION_TEST,
    INTEGRATION_TEST,
    STRESS_TEST,
    ENDURANCE_TEST
};

// Test priority levels
enum class TestPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};

// Test result
enum class TestResult {
    PASSED,
    FAILED,
    WARNING,
    INCONCLUSIVE,
    NOT_RUN
};

// Test configuration
struct TestConfig {
    TestType type;
    TestPriority priority;
    uint16_t duration;           // seconds
    uint16_t timeout;            // seconds
    bool autoStart;
    bool autoStop;
    bool requireOperator;
    bool logResults;
    uint16_t checkInterval;      // ms
    uint8_t retryCount;
};

// Test step structure
struct TestStep {
    uint8_t stepId;
    String description;
    uint16_t duration;           // ms
    bool completed;
    TestResult result;
    String notes;
    unsigned long startTime;
    unsigned long endTime;
};

// Test data structure
struct TestData {
    uint8_t testId;
    TestType type;
    TestState state;
    TestResult result;
    unsigned long startTime;
    unsigned long endTime;
    uint16_t duration;
    uint8_t stepCount;
    uint8_t completedSteps;
    uint8_t passedSteps;
    uint8_t failedSteps;
    String description;
    String operatorName;
};

// Test event types
enum class TestEventType {
    TEST_STARTED,
    TEST_STEP_STARTED,
    TEST_STEP_COMPLETED,
    TEST_STEP_FAILED,
    TEST_PAUSED,
    TEST_RESUMED,
    TEST_COMPLETED,
    TEST_FAILED,
    TEST_ABORTED,
    TEST_TIMEOUT,
    OPERATOR_INTERVENTION
};

// Test event structure
struct TestEvent {
    TestEventType type;
    String description;
    unsigned long timestamp;
    uint8_t severity;
    bool acknowledged;
    uint8_t testId;
    uint8_t stepId;
};

// Sensor test data
struct SensorTestData {
    double expectedValue;
    double actualValue;
    double tolerance;
    bool withinTolerance;
    uint8_t sampleCount;
    double samples[100];
};

// Communication test data
struct CommunicationTestData {
    uint8_t messageCount;
    uint8_t receivedCount;
    uint8_t errorCount;
    uint16_t averageLatency;
    uint16_t maxLatency;
    uint16_t minLatency;
    bool communicationHealthy;
};

class GroundTestModule {
private:
    // Current test state
    TestState currentState;
    TestType currentTestType;
    
    // Test management
    static const uint8_t MAX_TESTS = 16;
    TestData tests[MAX_TESTS];
    uint8_t testCount;
    uint8_t currentTestId;
    
    // Test steps
    static const uint8_t MAX_STEPS = 32;
    TestStep steps[MAX_STEPS];
    uint8_t stepCount;
    uint8_t currentStepId;
    
    // Event management
    static const uint8_t MAX_EVENTS = 32;
    TestEvent events[MAX_EVENTS];
    uint8_t eventCount;
    uint8_t eventIndex;
    
    // Timing
    unsigned long lastUpdate;
    unsigned long lastCheck;
    unsigned long testStartTime;
    unsigned long stepStartTime;
    
    // Error tracking
    uint8_t errorCount;
    uint8_t warningCount;
    uint8_t criticalErrorCount;
    
    // Test configuration
    TestConfig currentConfig;
    bool testInProgress;
    bool operatorRequired;
    bool operatorPresent;
    
    // Sensor test data
    SensorTestData sensorData;
    
    // Communication test data
    CommunicationTestData commData;
    
    // Component interfaces
    AirDataInterface* airDataInterface;
    InertialMeasurementInterface* imuInterface;
    StateEvents* stateEvents;

public:
    GroundTestModule();
    ~GroundTestModule();
    
    // Initialization and control
    bool initialize();
    void startup();
    void shutdown();
    void reset();
    
    // Test management
    bool startTest(TestType type, const String& description = "");
    bool stopTest();
    bool pauseTest();
    bool resumeTest();
    bool abortTest();
    
    // Test state management
    TestState getCurrentState() const { return currentState; }
    TestType getCurrentTestType() const { return currentTestType; }
    bool isTestInProgress() const { return testInProgress; }
    bool isTestCompleted() const;
    
    // Test configuration
    void setTestConfiguration(const TestConfig& config);
    TestConfig getTestConfiguration() const { return currentConfig; }
    void setTestDuration(uint16_t duration);
    void setTestTimeout(uint16_t timeout);
    void setTestPriority(TestPriority priority);
    void setAutoStart(bool enabled);
    void setAutoStop(bool enabled);
    void setOperatorRequired(bool required);
    
    // Test step management
    bool addTestStep(const String& description, uint16_t duration = 0);
    bool startCurrentStep();
    bool completeCurrentStep(TestResult result, const String& notes = "");
    bool failCurrentStep(const String& reason);
    uint8_t getStepCount() const { return stepCount; }
    uint8_t getCurrentStepId() const { return currentStepId; }
    void getStep(uint8_t index, TestStep& step) const;
    
    // Test data access
    uint8_t getTestCount() const { return testCount; }
    void getTest(uint8_t index, TestData& test) const;
    TestData getCurrentTest() const;
    TestResult getTestResult() const;
    uint16_t getTestDuration() const;
    uint8_t getCompletedSteps() const;
    uint8_t getPassedSteps() const;
    uint8_t getFailedSteps() const;
    
    // Specific test types
    bool runSystemCheck();
    bool runSensorCalibration();
    bool runCommunicationTest();
    bool runPowerTest();
    bool runMotorTest();
    bool runControlTest();
    bool runNavigationTest();
    bool runIntegrationTest();
    bool runStressTest();
    bool runEnduranceTest();
    
    // Sensor testing
    bool testAirDataSensors();
    bool testInertialSensors();
    bool testBarometricSensor();
    bool testTemperatureSensors();
    bool testHumiditySensors();
    bool testGPSReceiver();
    
    // Communication testing
    bool testSerialCommunication();
    bool testI2CCommunication();
    bool testSPICommunication();
    bool testRadioCommunication();
    bool testWiFiCommunication();
    bool testBluetoothCommunication();
    
    // Power testing
    bool testBatteryVoltage();
    bool testBatteryCurrent();
    bool testBatteryTemperature();
    bool testPowerConsumption();
    bool testVoltageRegulation();
    bool testCurrentLimiting();
    
    // Motor testing
    bool testMotorResponse();
    bool testMotorSpeed();
    bool testMotorTorque();
    bool testMotorEfficiency();
    bool testMotorTemperature();
    bool testMotorVibration();
    
    // Control testing
    bool testControlResponse();
    bool testControlStability();
    bool testControlAccuracy();
    bool testControlLatency();
    bool testControlBandwidth();
    bool testControlRobustness();
    
    // Navigation testing
    bool testGPSAccuracy();
    bool testIMUAccuracy();
    bool testNavigationAlgorithm();
    bool testWaypointNavigation();
    bool testReturnToHome();
    bool testObstacleAvoidance();
    
    // Error handling
    void addEvent(TestEventType type, const String& description, uint8_t severity = 128);
    uint8_t getErrorCount() const { return errorCount; }
    uint8_t getWarningCount() const { return warningCount; }
    uint8_t getCriticalErrorCount() const { return criticalErrorCount; }
    
    // Event management
    uint8_t getEventCount() const { return eventCount; }
    void getEvent(uint8_t index, TestEvent& event) const;
    void acknowledgeEvent(uint8_t index);
    void clearEvents();
    
    // Component interfaces
    void setAirDataInterface(AirDataInterface* interface) { airDataInterface = interface; }
    void setIMUInterface(InertialMeasurementInterface* interface) { imuInterface = interface; }
    void setStateEvents(StateEvents* events) { stateEvents = events; }
    
    // Main update loop
    void update();
    
    // Diagnostics and testing
    void performDiagnostics();
    bool isTestModuleHealthy() const;
    void runSelfTest();
    void simulateTestFailure();
    void simulateTestTimeout();
    
    // Operator interface
    bool requireOperator(bool required);
    bool operatorPresent(bool present);
    bool isOperatorRequired() const { return operatorRequired; }
    bool isOperatorPresent() const { return operatorPresent; }
    
    // Utility functions
    bool validateTestConfiguration(const TestConfig& config) const;
    bool checkTestTimeout() const;
    bool checkStepTimeout() const;
    void updateTestProgress();
    void logTestData();
    
    // Status and information
    String getTestStatus() const;
    double getTestProgress() const;
    double getStepProgress() const;
    void printTestStatus();
    void printTestResults();
    void printEventLog();
    void exportTestResults(const String& filename);
    
    // Recovery and maintenance
    void performRecovery();
    void enterMaintenanceMode();
    void exitMaintenanceMode();
    void clearTestHistory();
    void resetTestCounters();
};

#endif // GROUND_TEST_MODULE_H 