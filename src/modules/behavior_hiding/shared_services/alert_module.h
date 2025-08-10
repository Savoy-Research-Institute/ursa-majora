#ifndef ALERT_MODULE_H
#define ALERT_MODULE_H

#include <Arduino.h>

// Alert Levels
enum class AlertLevel {
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    EMERGENCY
};

// Alert Types
enum class AlertType {
    SYSTEM_ALERT,
    SENSOR_ALERT,
    COMMUNICATION_ALERT,
    BATTERY_ALERT,
    FLIGHT_ALERT,
    SAFETY_ALERT,
    USER_ALERT
};

// Alert States
enum class AlertState {
    INACTIVE,
    ACTIVE,
    ACKNOWLEDGED,
    RESOLVED,
    EXPIRED
};

// Alert Structure
struct Alert {
    uint32_t id;
    AlertLevel level;
    AlertType type;
    AlertState state;
    uint32_t timestamp;
    uint32_t duration_ms;
    String message;
    String source;
    bool is_valid;
};

// Alert Configuration
struct AlertConfig {
    bool enable_audible_alerts;
    bool enable_visual_alerts;
    bool enable_vibration_alerts;
    uint16_t alert_timeout_ms;
    uint8_t max_active_alerts;
    bool enable_alert_logging;
};

// Alert Error Information
struct AlertError {
    bool queue_full_error;
    bool memory_error;
    bool display_error;
    bool audio_error;
    String error_message;
};

class AlertModule {
private:
    // Alert Management
    static const uint8_t MAX_ALERTS = 20;
    Alert alertQueue[MAX_ALERTS];
    uint8_t alertCount;
    uint32_t nextAlertId;
    
    // State Management
    AlertConfig config;
    AlertError errorInfo;
    
    // Timing
    unsigned long lastUpdateTime;
    unsigned long lastAlertTime;
    
    // Alert Statistics
    uint32_t totalAlerts;
    uint32_t activeAlerts;
    uint32_t resolvedAlerts;
    
    // Private Methods
    void addAlertToQueue(const Alert& alert);
    void removeAlertFromQueue(uint32_t alertId);
    void updateAlertStates();
    void processExpiredAlerts();
    void updateErrorInfo();
    String formatAlertMessage(const Alert& alert);
    void triggerAlertOutput(const Alert& alert);
    
public:
    AlertModule();
    ~AlertModule();
    
    // Initialization and Control
    bool initialize();
    void setConfiguration(const AlertConfig& cfg);
    void reset();
    
    // Alert Creation
    uint32_t createAlert(AlertLevel level, AlertType type, const String& message, const String& source = "");
    uint32_t createSystemAlert(AlertLevel level, const String& message);
    uint32_t createSensorAlert(AlertLevel level, const String& message);
    uint32_t createCommunicationAlert(AlertLevel level, const String& message);
    uint32_t createBatteryAlert(AlertLevel level, const String& message);
    uint32_t createFlightAlert(AlertLevel level, const String& message);
    uint32_t createSafetyAlert(AlertLevel level, const String& message);
    
    // Alert Management
    bool acknowledgeAlert(uint32_t alertId);
    bool resolveAlert(uint32_t alertId);
    bool dismissAlert(uint32_t alertId);
    bool updateAlert(uint32_t alertId, const String& newMessage);
    bool setAlertDuration(uint32_t alertId, uint32_t duration_ms);
    
    // Alert Queries
    Alert getAlert(uint32_t alertId) const;
    uint8_t getActiveAlertCount() const { return activeAlerts; }
    uint8_t getTotalAlertCount() const { return alertCount; }
    bool hasActiveAlerts() const { return activeAlerts > 0; }
    bool hasCriticalAlerts() const;
    bool hasEmergencyAlerts() const;
    
    // Alert Filtering
    uint8_t getAlertsByLevel(AlertLevel level, Alert* alerts, uint8_t maxCount) const;
    uint8_t getAlertsByType(AlertType type, Alert* alerts, uint8_t maxCount) const;
    uint8_t getActiveAlerts(Alert* alerts, uint8_t maxCount) const;
    uint8_t getUnacknowledgedAlerts(Alert* alerts, uint8_t maxCount) const;
    
    // System Updates
    void update();
    void processAlerts();
    void clearAllAlerts();
    void clearExpiredAlerts();
    
    // Configuration
    void enableAudibleAlerts(bool enable);
    void enableVisualAlerts(bool enable);
    void enableVibrationAlerts(bool enable);
    void setAlertTimeout(uint16_t timeout_ms);
    void setMaxActiveAlerts(uint8_t max_alerts);
    void enableAlertLogging(bool enable);
    
    // Error Handling
    bool hasError() const;
    AlertError getError() const { return errorInfo; }
    void clearErrors();
    
    // Diagnostics
    bool performSelfTest();
    void getDiagnostics(String& diagnostics);
    void printAlertQueue();
    void getAlertStatistics(String& stats);
    
    // Advanced Features
    void setAlertPriority(uint32_t alertId, uint8_t priority);
    void enableAlertGrouping(bool enable);
    void setAlertThreshold(AlertLevel level, uint8_t threshold);
    void enableAlertEscalation(bool enable);
    
    // Utility Functions
    String alertLevelToString(AlertLevel level);
    String alertTypeToString(AlertType type);
    String alertStateToString(AlertState state);
    AlertLevel stringToAlertLevel(const String& level);
    AlertType stringToAlertType(const String& type);
    
    // Event Handlers
    void onAlertCreated(uint32_t alertId);
    void onAlertAcknowledged(uint32_t alertId);
    void onAlertResolved(uint32_t alertId);
    void onAlertExpired(uint32_t alertId);
};

#endif // ALERT_MODULE_H 