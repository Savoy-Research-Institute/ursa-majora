#ifndef COMPUTER_STATE_H
#define COMPUTER_STATE_H

#include <Arduino.h>

// Computer operating states
enum class ComputerState {
    POWER_OFF,
    POWERING_UP,
    BOOTING,
    INITIALIZING,
    OPERATIONAL,
    STANDBY,
    MAINTENANCE,
    DIAGNOSTIC,
    EMERGENCY,
    SHUTTING_DOWN,
    ERROR,
    RECOVERY
};

// Computer health status
enum class ComputerHealth {
    EXCELLENT,
    GOOD,
    FAIR,
    POOR,
    CRITICAL,
    FAILED
};

// Power states
enum class PowerState {
    OFF,
    STANDBY,
    LOW_POWER,
    NORMAL,
    HIGH_POWER,
    EMERGENCY
};

// Memory status
enum class MemoryStatus {
    NORMAL,
    WARNING,
    CRITICAL,
    FAILED
};

// CPU status
enum class CPUStatus {
    IDLE,
    LOW_LOAD,
    NORMAL_LOAD,
    HIGH_LOAD,
    OVERLOADED,
    THROTTLED
};

// Temperature status
enum class TemperatureStatus {
    COLD,
    NORMAL,
    WARM,
    HOT,
    CRITICAL,
    OVERHEATED
};

// Computer configuration
struct ComputerConfig {
    uint8_t maxTemperature;      // Maximum operating temperature in Celsius
    uint8_t maxCPUUsage;         // Maximum CPU usage percentage
    uint8_t maxMemoryUsage;      // Maximum memory usage percentage
    uint16_t watchdogTimeout;    // Watchdog timeout in milliseconds
    bool autoRecovery;           // Enable automatic recovery
    bool powerManagement;        // Enable power management
    bool thermalProtection;      // Enable thermal protection
    uint8_t emergencyMode;       // Emergency mode configuration
};

// Computer status
struct ComputerStatus {
    ComputerState state;
    ComputerHealth health;
    PowerState powerState;
    MemoryStatus memoryStatus;
    CPUStatus cpuStatus;
    TemperatureStatus temperatureStatus;
    
    uint8_t temperature;         // Current temperature in Celsius
    uint8_t cpuUsage;            // Current CPU usage percentage
    uint8_t memoryUsage;         // Current memory usage percentage
    uint8_t powerConsumption;    // Current power consumption percentage
    
    uint32_t uptime;             // System uptime in seconds
    uint32_t bootCount;          // Number of system boots
    uint8_t errorFlags;          // Error status flags
    String lastError;            // Last error message
    
    bool watchdogActive;         // Whether watchdog is active
    bool thermalProtectionActive; // Whether thermal protection is active
    bool powerManagementActive;   // Whether power management is active
};

// System resource information
struct SystemResources {
    uint32_t totalMemory;        // Total available memory in bytes
    uint32_t usedMemory;         // Currently used memory in bytes
    uint32_t freeMemory;         // Free memory in bytes
    
    uint32_t totalStorage;       // Total storage capacity in bytes
    uint32_t usedStorage;        // Used storage in bytes
    uint32_t freeStorage;        // Free storage in bytes
    
    uint8_t cpuCores;            // Number of CPU cores
    uint32_t cpuFrequency;       // CPU frequency in Hz
    uint8_t cpuTemperature;      // CPU temperature in Celsius
    
    uint8_t powerVoltage;        // Power supply voltage in volts
    uint8_t powerCurrent;        // Power supply current in amperes
    uint8_t powerEfficiency;     // Power efficiency percentage
};

// Performance metrics
struct PerformanceMetrics {
    uint32_t averageCPUUsage;    // Average CPU usage over time
    uint32_t peakCPUUsage;       // Peak CPU usage
    uint32_t averageMemoryUsage; // Average memory usage over time
    uint32_t peakMemoryUsage;    // Peak memory usage
    
    uint32_t averageTemperature; // Average temperature over time
    uint32_t peakTemperature;    // Peak temperature
    uint32_t averagePowerUsage;  // Average power usage over time
    uint32_t peakPowerUsage;     // Peak power usage
    
    uint32_t responseTime;       // Average system response time
    uint32_t throughput;         // System throughput
    uint32_t errorRate;          // Error rate percentage
    uint32_t availability;       // System availability percentage
};

class ComputerState {
private:
    // Current state
    ComputerStatus status;
    ComputerConfig config;
    SystemResources resources;
    PerformanceMetrics metrics;
    
    // State history
    static const uint8_t MAX_STATE_HISTORY = 32;
    ComputerState stateHistory[MAX_STATE_HISTORY];
    unsigned long stateTimestamps[MAX_STATE_HISTORY];
    uint8_t stateHistoryIndex;
    uint8_t stateHistoryCount;
    
    // Performance monitoring
    static const uint8_t MAX_PERFORMANCE_SAMPLES = 64;
    uint8_t cpuUsageHistory[MAX_PERFORMANCE_SAMPLES];
    uint8_t memoryUsageHistory[MAX_PERFORMANCE_SAMPLES];
    uint8_t temperatureHistory[MAX_PERFORMANCE_SAMPLES];
    uint8_t powerUsageHistory[MAX_PERFORMANCE_SAMPLES];
    uint8_t performanceIndex;
    
    // System state
    bool initialized;
    bool monitoringEnabled;
    unsigned long lastUpdateTime;
    unsigned long lastMetricsUpdate;
    unsigned long lastHealthCheck;
    
    // Error handling
    uint8_t errorFlags;
    String lastError;
    
    // Private methods
    void updateSystemResources();
    void updatePerformanceMetrics();
    void updateSystemHealth();
    void recordStateChange(ComputerState newState);
    void updatePerformanceHistory();
    bool validateConfiguration(const ComputerConfig& config);
    void applyConfiguration(const ComputerConfig& config);
    void performHealthCheck();
    
public:
    ComputerState();
    
    // State management
    bool setState(ComputerState newState);
    ComputerState getState() const { return status.state; }
    bool isState(ComputerState state) const { return status.state == state; }
    bool canTransitionTo(ComputerState newState) const;
    
    // Configuration
    bool configure(const ComputerConfig& newConfig);
    ComputerConfig getConfiguration() const { return config; }
    bool setMaxTemperature(uint8_t maxTemp);
    bool setMaxCPUUsage(uint8_t maxUsage);
    bool setMaxMemoryUsage(uint8_t maxUsage);
    bool setWatchdogTimeout(uint16_t timeout);
    bool setAutoRecovery(bool enabled);
    bool setPowerManagement(bool enabled);
    bool setThermalProtection(bool enabled);
    
    // Status and health
    ComputerStatus getStatus() const { return status; }
    ComputerHealth getHealth() const { return status.health; }
    bool isHealthy() const { return status.health != ComputerHealth::FAILED; }
    bool isOperational() const { return status.state == ComputerState::OPERATIONAL; }
    bool isEmergency() const { return status.state == ComputerState::EMERGENCY; }
    
    // System resources
    SystemResources getResources() const { return resources; }
    uint32_t getTotalMemory() const { return resources.totalMemory; }
    uint32_t getUsedMemory() const { return resources.usedMemory; }
    uint32_t getFreeMemory() const { return resources.freeMemory; }
    uint8_t getCPUUsage() const { return status.cpuUsage; }
    uint8_t getMemoryUsage() const { return status.memoryUsage; }
    uint8_t getTemperature() const { return status.temperature; }
    
    // Performance monitoring
    PerformanceMetrics getMetrics() const { return metrics; }
    void resetMetrics();
    uint32_t getUptime() const { return status.uptime; }
    uint32_t getBootCount() const { return status.bootCount; }
    
    // Power management
    PowerState getPowerState() const { return status.powerState; }
    bool setPowerState(PowerState newState);
    bool enablePowerManagement(bool enabled);
    bool isPowerManagementActive() const { return status.powerManagementActive; }
    
    // Thermal management
    TemperatureStatus getTemperatureStatus() const { return status.temperatureStatus; }
    bool enableThermalProtection(bool enabled);
    bool isThermalProtectionActive() const { return status.thermalProtectionActive; }
    
    // Watchdog management
    bool enableWatchdog(bool enabled);
    bool isWatchdogActive() const { return status.watchdogActive; }
    void resetWatchdog();
    
    // System control
    bool initialize();
    void update();
    void shutdown();
    bool performSelfTest();
    bool performDiagnostics();
    
    // Recovery and maintenance
    bool enterMaintenanceMode();
    bool exitMaintenanceMode();
    bool performRecovery();
    bool resetSystem();
    bool emergencyShutdown();
    
    // Error handling
    uint8_t getErrorFlags() const { return errorFlags; }
    String getLastError() const { return lastError; }
    void clearErrors();
    bool hasErrors() const;
    
    // Monitoring and diagnostics
    bool enableMonitoring(bool enabled);
    bool isMonitoringEnabled() const { return monitoringEnabled; }
    void printStatus();
    void printResources();
    void printMetrics();
    void printConfiguration();
    void printStateHistory();
    
    // Utility methods
    bool isLowPower() const;
    bool isOverheating() const;
    bool isOverloaded() const;
    bool needsMaintenance() const;
    uint8_t getHealthScore() const;
    
    // Advanced features
    bool enablePerformanceProfiling(bool enabled);
    bool setPerformanceThresholds(uint8_t cpu, uint8_t memory, uint8_t temp);
    bool enablePredictiveMaintenance(bool enabled);
    bool setMaintenanceSchedule(uint32_t interval);
};

#endif // COMPUTER_STATE_H 