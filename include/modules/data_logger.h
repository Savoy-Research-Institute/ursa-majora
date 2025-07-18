/**
 * @file datalogger.h
 * @brief SD Card Data Logger for Velma Flight Controller System
 * @author Velma Development Team
 * @version 1.0
 * 
 * This module implements FR4 requirements for the Velma/Blizzard flight controller system.
 * Provides comprehensive SD card logging functionality with sensor data timestamping,
 * system event logging, and non-blocking operation to maintain 100Hz flight loops.
 * 
 * Requirements Implemented:
 * - FR4.1: Initialize SD card module
 * - FR4.2: Log sensor data and system events with timestamps
 * - FR4.3: Provide APIs to read log file status and clear logs
 * - NFR3: Non-blocking operation for main flight loop
 */

 #ifndef DATALOGGER_H
 #define DATALOGGER_H
 
 #include <Arduino.h>
 #include <SD.h>
 #include <SPI.h>
 
 /**
  * @brief Data logger class for SD card operations in Velma flight controller
  * 
  * Provides non-blocking SD card operations with error handling and memory optimization
  * specifically designed for Velma/Blizzard drone flight controllers.
  * Maintains 100Hz+ flight loop performance while logging sensor data and system events.
  */
 class DataLogger {
 public:
     /**
      * @brief File operation modes
      */
     enum FileMode {
         READ_MODE = 0,      ///< Read-only mode
         WRITE_MODE = 1,     ///< Write mode (truncate existing)
         APPEND_MODE = 2     ///< Append mode (add to existing)
     };
     
     /**
      * @brief Sensor data types for structured logging
      */
     enum SensorType {
         SENSOR_IMU = 0,         ///< MPU6050 IMU data
         SENSOR_GPS = 1,         ///< NEO-6M GPS data
         SENSOR_BAROMETER = 2,   ///< BMP180/BMP280 pressure
         SENSOR_ULTRASONIC = 3,  ///< HC-SR04 distance
         SENSOR_MAGNETOMETER = 4, ///< HMC5883L compass
         SENSOR_RADIO = 5        ///< PPM radio receiver
     };
     
     /**
      * @brief Log levels for categorizing log entries
      */
     enum LogLevel {
         LOG_DEBUG = 0,      ///< Debug information
         LOG_INFO = 1,       ///< General information
         LOG_WARNING = 2,    ///< Warning messages
         LOG_ERROR = 3,      ///< Error messages
         LOG_CRITICAL = 4    ///< Critical system errors
     };
     
     /**
      * @brief Log file status information (FR4.3)
      */
     struct LogFileStatus {
         String filename;        ///< Current log filename
         uint32_t fileSize;      ///< File size in bytes
         uint32_t entryCount;    ///< Number of log entries
         uint32_t lastWrite;     ///< Last write timestamp (millis)
         bool isActive;          ///< File currently open for writing
     };
     
     /**
      * @brief Error codes for datalogger operations
      */
     enum ErrorCode {
         SUCCESS = 0,             ///< Operation successful
         ERROR_SD_INIT = 1,       ///< SD card initialization failed
         ERROR_FILE_OPEN = 2,     ///< File open operation failed
         ERROR_FILE_WRITE = 3,    ///< File write operation failed
         ERROR_FILE_READ = 4,     ///< File read operation failed
         ERROR_FILE_DELETE = 5,   ///< File delete operation failed
         ERROR_NO_SPACE = 6,      ///< Insufficient disk space
         ERROR_INVALID_PATH = 7   ///< Invalid file path
     };
 
 private:
     static const uint8_t SD_CS_PIN = 53;        ///< SD card chip select pin (Arduino Mega default)
     static const uint16_t MAX_FILENAME_LEN = 64; ///< Maximum filename length
     static const uint32_t MAX_LOG_SIZE = 1048576; ///< Maximum log file size (1MB)
     static const uint8_t MAX_LOG_FILES = 5;      ///< Maximum number of rotated log files
     
     bool _isInitialized;                         ///< SD card initialization status
     File _currentFile;                           ///< Currently open file handle
     String _currentFilename;                     ///< Currently open filename
     FileMode _currentMode;                       ///< Current file access mode
     uint32_t _logCounter;                        ///< Log entry counter for rotation
     
     /**
      * @brief Get sensor type string representation
      * @param sensorType Sensor type enumeration
      * @return String representation of sensor type
      */
     String getSensorTypeString(SensorType sensorType);
     
     /**
      * @brief Get system component string representation
      * @param component System component enumeration
      * @return String representation of component
      */
     String getSystemComponentString(SystemComponent component);
     
     /**
      * @brief Get log level string representation
      * @param level Log level enumeration
      * @return String representation of log level
      */
     String getLogLevelString(LogLevel level);
     
     /**
      * @brief Check if log rotation is needed
      * @param filename Current log filename
      * @return True if rotation is needed
      */
     bool needsRotation(const String& filename);
     
     /**
      * @brief Perform log file rotation
      * @param filename Base filename for rotation
      * @return Error code
      */
     ErrorCode rotateLogFile(const String& filename);
     
     /**
      * @brief Generate timestamped filename
      * @param baseName Base filename without extension
      * @param extension File extension (with dot)
      * @return Timestamped filename
      */
     String generateTimestampedFilename(const String& baseName, const String& extension);
 
 public:
     /**
      * @brief Constructor
      */
     DataLogger();
     
     /**
      * @brief Initialize SD card and SPI interface
      * @param csPin Chip select pin (default: 53 for Arduino Mega)
      * @return Error code indicating success or failure
      */
     ErrorCode init(uint8_t csPin = SD_CS_PIN);
     
     /**
      * @brief Open file for reading or writing
      * @param filename Name of file to open
      * @param mode File access mode (READ_MODE, WRITE_MODE, APPEND_MODE)
      * @return Error code indicating success or failure
      */
     ErrorCode openFile(const String& filename, FileMode mode = WRITE_MODE);
     
     /**
      * @brief Write a line to the currently open file
      * @param data String data to write
      * @param addNewline Add newline character (default: true)
      * @return Error code indicating success or failure
      */
     ErrorCode writeLine(const String& data, bool addNewline = true);
     
     /**
      * @brief Write a line to the currently open file (char* version)
      * @param data Character array to write
      * @param addNewline Add newline character (default: true)
      * @return Error code indicating success or failure
      */
     ErrorCode writeLine(const char* data, bool addNewline = true);
     
     /**
      * @brief Write formatted log entry with timestamp and level
      * @param level Log level
      * @param message Log message
      * @return Error code indicating success or failure
      */
     ErrorCode writeLogEntry(LogLevel level, const String& message);
     
     /**
      * @brief Read entire file contents
      * @param filename Name of file to read
      * @param content Reference to string to store file contents
      * @return Error code indicating success or failure
      */
     ErrorCode readFile(const String& filename, String& content);
     
     /**
      * @brief Read file line by line (memory efficient)
      * @param filename Name of file to read
      * @param lineCallback Callback function for each line
      * @return Error code indicating success or failure
      */
     ErrorCode readFileLines(const String& filename, void (*lineCallback)(const String&));
     
     /**
      * @brief Close currently open file
      * @return Error code indicating success or failure
      */
     ErrorCode closeFile();
     
     /**
      * @brief Delete file from SD card
      * @param filename Name of file to delete
      * @return Error code indicating success or failure
      */
     ErrorCode deleteFile(const String& filename);
     
     /**
      * @brief Check if file exists on SD card
      * @param filename Name of file to check
      * @return True if file exists
      */
     bool fileExists(const String& filename);
     
     /**
      * @brief Get file size in bytes
      * @param filename Name of file to check
      * @return File size in bytes (0 if file doesn't exist)
      */
     uint32_t getFileSize(const String& filename);
     
     /**
      * @brief Get available disk space
      * @return Available space in bytes
      */
     uint32_t getAvailableSpace();
     
     /**
      * @brief List all files in root directory
      * @param fileList Reference to string array to store filenames
      * @param maxFiles Maximum number of files to list
      * @return Number of files found
      */
     uint8_t listFiles(String fileList[], uint8_t maxFiles);
     
     /**
      * @brief Create flight log with automatic rotation
      * @param logData Flight data to log
      * @param sessionId Flight session identifier
      * @return Error code indicating success or failure
      */
     ErrorCode logFlightData(const String& logData, const String& sessionId = "");
     
     /**
      * @brief Log sensor data with timestamp (FR4.2)
      * @param sensorType Type of sensor data being logged
      * @param sensorData Formatted sensor data string
      * @param timestamp Custom timestamp (0 = use current millis)
      * @return Error code indicating success or failure
      */
     ErrorCode logSensorData(SensorType sensorType, const String& sensorData, uint32_t timestamp = 0);
     
     /**
      * @brief Log system event with component and timestamp (FR4.2)
      * @param component System component generating the event
      * @param level Log level
      * @param message Event message
      * @param timestamp Custom timestamp (0 = use current millis)
      * @return Error code indicating success or failure
      */
     ErrorCode logSystemEvent(SystemComponent component, LogLevel level, const String& message, uint32_t timestamp = 0);
     
     /**
      * @brief Get log file status information (FR4.3)
      * @param filename Log file to check
      * @param status Reference to LogFileStatus structure
      * @return Error code indicating success or failure
      */
     ErrorCode getLogFileStatus(const String& filename, LogFileStatus& status);
     
     /**
      * @brief Clear log files (FR4.3)
      * @param logType Type of logs to clear ("all", "sensor", "system", "flight")
      * @return Error code indicating success or failure
      */
     ErrorCode clearLogs(const String& logType = "all");
     
     /**
      * @brief Non-blocking write operation for flight loop compatibility (NFR3)
      * @param data Data to write to buffer
      * @return Error code indicating success or failure
      */
     ErrorCode writeNonBlocking(const String& data);
     
     /**
      * @brief Process buffered writes (call periodically from main loop)
      * @return Error code indicating success or failure
      */
     ErrorCode processBufferedWrites();
     
     /**
      * @brief Get error message string
      * @param errorCode Error code enumeration
      * @return Human-readable error message
      */
     String getErrorMessage(ErrorCode errorCode);
     
     /**
      * @brief Check if SD card is initialized and ready
      * @return True if initialized
      */
     bool isInitialized() const { return _isInitialized; }
     
     /**
      * @brief Get current file name
      * @return Currently open filename
      */
     String getCurrentFilename() const { return _currentFilename; }
     
     /**
      * @brief Flush file buffer to ensure data is written
      * @return Error code indicating success or failure
      */
     ErrorCode flush();
     
     /**
      * @brief Perform maintenance operations (cleanup old logs, defragmentation)
      * @return Error code indicating success or failure
      */
     ErrorCode performMaintenance();
 };
 
 // Global datalogger instance for easy access
 extern DataLogger datalogger;
 
 #endif // DATALOGGER_H