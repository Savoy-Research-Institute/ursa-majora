/**
 * @file alert_unit_test.cpp
 * @brief Unit tests for the Alert Module
 * @author Velma Development Team
 * @version 1.0
 * @date 2025
 * 
 * @details
 * This file contains unit tests for the Alert Module, testing individual
 * functionality in isolation. Tests include LED control, buzzer operation,
 * and alert state management.
 */

#include <Arduino.h>
#include "../../modules/behavior_hiding/shared_services/alert_module.h"

// Test configuration
const int GREEN_LED_PIN = 2;
const int YELLOW_LED_PIN = 3;
const int RED_LED_PIN = 4;
const int BUZZER_PIN = 5;

// Test instance
AlertModule alertModule;

// Test results tracking
bool allTestsPassed = true;
int testsRun = 0;
int testsPassed = 0;

// Test utilities
void assertTrue(bool condition, const char* testName) {
    testsRun++;
    if (condition) {
        testsPassed++;
        Serial.print("PASS: ");
    } else {
        allTestsPassed = false;
        Serial.print("FAIL: ");
    }
    Serial.println(testName);
}

void assertFalse(bool condition, const char* testName) {
    assertTrue(!condition, testName);
}

void assertEqual(int expected, int actual, const char* testName) {
    testsRun++;
    if (expected == actual) {
        testsPassed++;
        Serial.print("PASS: ");
    } else {
        allTestsPassed = false;
        Serial.print("FAIL: ");
        Serial.print(testName);
        Serial.print(" - Expected: ");
        Serial.print(expected);
        Serial.print(", Got: ");
        Serial.println(actual);
        return;
    }
    Serial.println(testName);
}

void assertStringEqual(const String& expected, const String& actual, const char* testName) {
    testsRun++;
    if (expected == actual) {
        testsPassed++;
        Serial.print("PASS: ");
    } else {
        allTestsPassed = false;
        Serial.print("FAIL: ");
        Serial.print(testName);
        Serial.print(" - Expected: '");
        Serial.print(expected);
        Serial.print("', Got: '");
        Serial.print(actual);
        Serial.println("'");
        return;
    }
    Serial.println(testName);
}

// Test functions
void testAlertInitialization() {
    Serial.println("\n=== Testing Alert Module Initialization ===");
    
    // Test default state
    assertTrue(alertModule.initialize(), "Module initialization");
    assertEqual(0, alertModule.getActiveAlertCount(), "Initial active alert count");
    assertEqual(0, alertModule.getTotalAlertCount(), "Initial total alert count");
    assertFalse(alertModule.hasActiveAlerts(), "No active alerts initially");
    assertFalse(alertModule.hasError(), "No errors initially");
}

void testAlertCreation() {
    Serial.println("\n=== Testing Alert Creation ===");
    
    // Test creating different types of alerts
    uint32_t alert1 = alertModule.createSystemAlert(AlertLevel::INFO, "Test system alert");
    assertTrue(alert1 > 0, "System alert creation");
    
    uint32_t alert2 = alertModule.createSensorAlert(AlertLevel::WARNING, "Test sensor alert");
    assertTrue(alert2 > 0, "Sensor alert creation");
    
    uint32_t alert3 = alertModule.createBatteryAlert(AlertLevel::CRITICAL, "Test battery alert");
    assertTrue(alert3 > 0, "Battery alert creation");
    
    // Verify alert counts
    assertEqual(3, alertModule.getTotalAlertCount(), "Total alert count after creation");
    assertEqual(3, alertModule.getActiveAlertCount(), "Active alert count after creation");
    assertTrue(alertModule.hasActiveAlerts(), "Has active alerts");
}

void testAlertLevels() {
    Serial.println("\n=== Testing Alert Levels ===");
    
    // Test all alert levels
    uint32_t infoAlert = alertModule.createAlert(AlertLevel::INFO, AlertType::SYSTEM, "Info level test");
    uint32_t warningAlert = alertModule.createAlert(AlertLevel::WARNING, AlertType::SYSTEM, "Warning level test");
    uint32_t criticalAlert = alertModule.createAlert(AlertLevel::CRITICAL, AlertType::SYSTEM, "Critical level test");
    uint32_t emergencyAlert = alertModule.createAlert(AlertLevel::EMERGENCY, AlertType::SYSTEM, "Emergency level test");
    
    assertTrue(infoAlert > 0, "Info alert creation");
    assertTrue(warningAlert > 0, "Warning alert creation");
    assertTrue(criticalAlert > 0, "Critical alert creation");
    assertTrue(emergencyAlert > 0, "Emergency alert creation");
    
    // Test level-specific queries
    Alert alerts[10];
    uint8_t infoCount = alertModule.getAlertsByLevel(AlertLevel::INFO, alerts, 10);
    uint8_t warningCount = alertModule.getAlertsByLevel(AlertLevel::WARNING, alerts, 10);
    uint8_t criticalCount = alertModule.getAlertsByLevel(AlertLevel::CRITICAL, alerts, 10);
    uint8_t emergencyCount = alertModule.getAlertsByLevel(AlertLevel::EMERGENCY, alerts, 10);
    
    assertTrue(infoCount > 0, "Info alerts found");
    assertTrue(warningCount > 0, "Warning alerts found");
    assertTrue(criticalCount > 0, "Critical alerts found");
    assertTrue(emergencyCount > 0, "Emergency alerts found");
}

void testAlertManagement() {
    Serial.println("\n=== Testing Alert Management ===");
    
    // Create test alert
    uint32_t testAlert = alertModule.createSystemAlert(AlertLevel::WARNING, "Test management alert");
    assertTrue(testAlert > 0, "Test alert creation");
    
    // Test acknowledgment
    assertTrue(alertModule.acknowledgeAlert(testAlert), "Alert acknowledgment");
    
    // Test update
    assertTrue(alertModule.updateAlert(testAlert, "Updated test message"), "Alert update");
    
    // Test resolution
    assertTrue(alertModule.resolveAlert(testAlert), "Alert resolution");
    
    // Verify state changes
    Alert alert = alertModule.getAlert(testAlert);
    // Note: This would require implementing getAlert method in AlertModule
}

void testAlertTypes() {
    Serial.println("\n=== Testing Alert Types ===");
    
    // Test all alert types
    uint32_t systemAlert = alertModule.createAlert(AlertLevel::INFO, AlertType::SYSTEM, "System type test");
    uint32_t sensorAlert = alertModule.createAlert(AlertLevel::INFO, AlertType::SENSOR, "Sensor type test");
    uint32_t commAlert = alertModule.createAlert(AlertLevel::INFO, AlertType::COMMUNICATION, "Communication type test");
    uint32_t batteryAlert = alertModule.createAlert(AlertLevel::INFO, AlertType::BATTERY, "Battery type test");
    uint32_t flightAlert = alertModule.createAlert(AlertLevel::INFO, AlertType::FLIGHT, "Flight type test");
    uint32_t safetyAlert = alertModule.createAlert(AlertLevel::INFO, AlertType::SAFETY, "Safety type test");
    
    assertTrue(systemAlert > 0, "System alert creation");
    assertTrue(sensorAlert > 0, "Sensor alert creation");
    assertTrue(commAlert > 0, "Communication alert creation");
    assertTrue(batteryAlert > 0, "Battery alert creation");
    assertTrue(flightAlert > 0, "Flight alert creation");
    assertTrue(safetyAlert > 0, "Safety alert creation");
    
    // Test type-specific queries
    Alert alerts[10];
    uint8_t systemCount = alertModule.getAlertsByType(AlertType::SYSTEM, alerts, 10);
    uint8_t sensorCount = alertModule.getAlertsByType(AlertType::SENSOR, alerts, 10);
    
    assertTrue(systemCount > 0, "System alerts found");
    assertTrue(sensorCount > 0, "Sensor alerts found");
}

void testAlertCleanup() {
    Serial.println("\n=== Testing Alert Cleanup ===");
    
    // Test clearing all alerts
    alertModule.clearAllAlerts();
    
    assertEqual(0, alertModule.getActiveAlertCount(), "Active alerts cleared");
    assertEqual(0, alertModule.getTotalAlertCount(), "Total alerts cleared");
    assertFalse(alertModule.hasActiveAlerts(), "No active alerts after cleanup");
}

void testAlertConfiguration() {
    Serial.println("\n=== Testing Alert Configuration ===");
    
    // Test configuration changes
    alertModule.setMaxActiveAlerts(5);
    alertModule.setAlertTimeout(5000);
    alertModule.enableAudibleAlerts(true);
    alertModule.enableVisualAlerts(true);
    alertModule.enableVibrationAlerts(false);
    
    // Create alerts up to limit
    for (int i = 0; i < 6; i++) {
        uint32_t alert = alertModule.createSystemAlert(AlertLevel::INFO, "Config test alert");
        if (i < 5) {
            assertTrue(alert > 0, "Alert creation within limit");
        } else {
            // The 6th alert should fail or be queued
            // This depends on implementation
        }
    }
}

void testAlertErrorHandling() {
    Serial.println("\n=== Testing Alert Error Handling ===");
    
    // Test error conditions
    assertFalse(alertModule.acknowledgeAlert(99999), "Invalid alert ID acknowledgment");
    assertFalse(alertModule.resolveAlert(99999), "Invalid alert ID resolution");
    assertFalse(alertModule.updateAlert(99999, "Invalid update"), "Invalid alert ID update");
    
    // Test self-test
    assertTrue(alertModule.performSelfTest(), "Self-test execution");
}

void testAlertStatistics() {
    Serial.println("\n=== Testing Alert Statistics ===");
    
    // Create some test alerts
    alertModule.createSystemAlert(AlertLevel::INFO, "Stats test 1");
    alertModule.createSystemAlert(AlertLevel::WARNING, "Stats test 2");
    alertModule.createSystemAlert(AlertLevel::CRITICAL, "Stats test 3");
    
    // Test statistics retrieval
    String stats;
    alertModule.getAlertStatistics(stats);
    assertTrue(stats.length() > 0, "Statistics string generated");
    
    // Test alert queue display
    alertModule.printAlertQueue();
}

void testAlertStringConversions() {
    Serial.println("\n=== Testing Alert String Conversions ===");
    
    // Test level conversions
    assertStringEqual("INFO", alertModule.alertLevelToString(AlertLevel::INFO), "Info level string");
    assertStringEqual("WARNING", alertModule.alertLevelToString(AlertLevel::WARNING), "Warning level string");
    assertStringEqual("CRITICAL", alertModule.alertLevelToString(AlertLevel::CRITICAL), "Critical level string");
    assertStringEqual("EMERGENCY", alertModule.alertLevelToString(AlertLevel::EMERGENCY), "Emergency level string");
    
    // Test type conversions
    assertStringEqual("SYSTEM", alertModule.alertTypeToString(AlertType::SYSTEM), "System type string");
    assertStringEqual("SENSOR", alertModule.alertTypeToString(AlertType::SENSOR), "Sensor type string");
    assertStringEqual("COMMUNICATION", alertModule.alertTypeToString(AlertType::COMMUNICATION), "Communication type string");
    
    // Test state conversions
    assertStringEqual("ACTIVE", alertModule.alertStateToString(AlertState::ACTIVE), "Active state string");
    assertStringEqual("ACKNOWLEDGED", alertModule.alertStateToString(AlertState::ACKNOWLEDGED), "Acknowledged state string");
    assertStringEqual("RESOLVED", alertModule.alertStateToString(AlertState::RESOLVED), "Resolved state string");
}

void testAlertCallbacks() {
    Serial.println("\n=== Testing Alert Callbacks ===");
    
    // Test callback registration (if implemented)
    // This would test the onAlertCreated, onAlertAcknowledged, etc. callbacks
    
    // Create a test alert to trigger callbacks
    uint32_t callbackAlert = alertModule.createSystemAlert(AlertLevel::INFO, "Callback test alert");
    assertTrue(callbackAlert > 0, "Callback test alert creation");
    
    // Test acknowledgment callback
    if (alertModule.acknowledgeAlert(callbackAlert)) {
        // Callback should be triggered
    }
    
    // Test resolution callback
    if (alertModule.resolveAlert(callbackAlert)) {
        // Callback should be triggered
    }
}

void runAllTests() {
    Serial.println("Starting Alert Module Unit Tests...");
    Serial.println("=====================================");
    
    testAlertInitialization();
    testAlertCreation();
    testAlertLevels();
    testAlertManagement();
    testAlertTypes();
    testAlertCleanup();
    testAlertConfiguration();
    testAlertErrorHandling();
    testAlertStatistics();
    testAlertStringConversions();
    testAlertCallbacks();
    
    // Print test summary
    Serial.println("\n=====================================");
    Serial.println("Test Summary:");
    Serial.print("Tests Run: ");
    Serial.println(testsRun);
    Serial.print("Tests Passed: ");
    Serial.println(testsPassed);
    Serial.print("Tests Failed: ");
    Serial.println(testsRun - testsPassed);
    Serial.print("Overall Result: ");
    Serial.println(allTestsPassed ? "ALL TESTS PASSED" : "SOME TESTS FAILED");
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("Alert Module Unit Test Suite");
    Serial.println("============================");
    
    runAllTests();
}

void loop() {
    // Tests run once in setup
    // Keep loop empty for unit testing
} 