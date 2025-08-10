#ifndef DATA_LOGGER_MODULE_H
#define DATA_LOGGER_MODULE_H

#include <Arduino.h>
#include <SD.h>

// Logger States
enum class LoggerState {
    DISABLED,
    INITIALIZING,
    READY,
    LOGGING,
    ERROR
};

// Log Entry Types
enum class LogEntryType {
    FLIGHT_DATA,
    SENSOR_DATA,
    SYSTEM_EVENT,
    ERROR_EVENT,
    USER_ACTION,
    CALIBRATION_DATA
};

// Sensor data types for structured logging
enum class SensorType {
    SENSOR_IMU = 0,         ///< MPU6050 IMU data
    SENSOR_GPS = 1,         ///< NEO-6M GPS data
    SENSOR_BAROMETER = 2,   ///< BMP180/BMP280 pressure
    SENSOR_ULTRASONIC = 3,  ///< HC-SR04 distance
    SENSOR_MAGNETOMETER = 4, ///< HMC5883L compass
    SENSOR_RADIO = 5        ///< PPM radio receiver
};

// Log levels for categorizing log entries
enum class LogLevel {
    LOG_DEBUG = 0,      ///< Debug information
    LOG_INFO = 1,       ///< General information
    LOG_WARNING = 2,    ///< Warning messages
    LOG_ERROR = 3,      ///< Error messages
    LOG_CRITICAL = 4    ///< Critical system errors
};

// Log Entry Structure
struct LogEntry {
    LogEntryType type;
    uint32_t timestamp;
    String data;
    bool is_valid;
};

// Logger Configuration
struct LoggerConfig {
    uint16_t max_file_size_kb;
    uint8_t max_files;
    bool enable_compression;
    bool enable_timestamp;
    uint16_t flush_interval_ms;
    String log_directory;
};

// Logger Error Information
struct LoggerError {
    bool sd_card_error;
    bool file_system_error;
    bool write_error;
    bool space_error;
    String error_message;
};

class DataLoggerModule {
private:
    // Hardware Configuration
    int chipSelectPin;
    String currentFileName;
    
    // State Management
    LoggerState currentState;
    LoggerConfig config;
    LoggerError errorInfo;
    
    // File Management
    File currentFile;
    uint32_t currentFileSize;
    uint16_t fileCounter;
    
    // Timing
    unsigned long lastFlushTime;
    unsigned long lastWriteTime;
    
    // Buffer Management
    String writeBuffer;
    uint16_t bufferSize;
    
    // Private Methods
    bool initializeSDCard();
    bool createLogFile();
    bool openLogFile();
    void closeLogFile();
    bool rotateLogFile();
    void flushBuffer();
    void updateErrorInfo();
    String generateFileName();
    String formatTimestamp(uint32_t timestamp);
    
public:
    DataLoggerModule(int cs_pin = 4);
    ~DataLoggerModule();
    
    // Initialization and Control
    bool initialize();
    void enable();
    void disable();
    bool isEnabled() const;
    void reset();
    
    // Configuration
    void setConfiguration(const LoggerConfig& cfg);
    void setChipSelectPin(int pin);
    void setLogDirectory(const String& directory);
    void setMaxFileSize(uint16_t size_kb);
    void setMaxFiles(uint8_t max_files);
    void setFlushInterval(uint16_t interval_ms);
    
    // Logging Operations
    bool logData(LogEntryType type, const String& data);
    bool logFlightData(const String& data);
    bool logSensorData(const String& data);
    bool logSystemEvent(const String& event);
    bool logError(const String& error);
    bool logUserAction(const String& action);
    bool logCalibrationData(const String& data);
    
    // File Operations
    bool createLogFile(const String& filename);
    bool writeLogData(const String& filename, const String& data);
    bool appendLogData(const String& filename, const String& data);
    bool readLogFile(const String& filename, String& content);
    bool deleteLogFile(const String& filename);
    bool listLogFiles(String& fileList);
    
    // Specialized Logging
    bool logBlackboxData(const String& data);
    bool storeCheatCodeScript(const String& scriptName, const String& script);
    bool retrieveCheatCodeScript(const String& scriptName, String& script);
    bool logTelemetryData(const String& data);
    
    // Sensor-specific logging methods
    bool logSensorData(SensorType sensorType, const String& sensorData, uint32_t timestamp = 0);
    bool logSystemEvent(const String& component, LogLevel level, const String& message, uint32_t timestamp = 0);
    bool logFlightData(const String& logData, const String& sessionId = "");
    
    // Non-blocking operations for flight loops
    bool writeNonBlocking(const String& data);
    bool processBufferedWrites();
    
    // Data Access
    LoggerState getState() const { return currentState; }
    bool isReady() const { return currentState == LoggerState::READY; }
    bool isLogging() const { return currentState == LoggerState::LOGGING; }
    String getCurrentFileName() const { return currentFileName; }
    uint32_t getCurrentFileSize() const { return currentFileSize; }
    
    // Error Handling
    bool hasError() const;
    LoggerError getError() const { return errorInfo; }
    void clearErrors();
    
    // Diagnostics
    bool performSelfTest();
    void getDiagnostics(String& diagnostics);
    uint32_t getFreeSpace();
    uint16_t getFileCount();
    
    // Maintenance
    void cleanup();
    void defragment();
    void backupLogs();
    void archiveOldLogs();
    
    // Advanced Features
    void enableCompression(bool enable);
    void setBufferSize(uint16_t size);
    void enableTimestamp(bool enable);
    void setLogLevel(uint8_t level);
    
    // Utility Functions
    String formatLogEntry(LogEntryType type, const String& data);
    bool validateFileName(const String& filename);
    void printFileInfo();
};

#endif // DATA_LOGGER_MODULE_H 