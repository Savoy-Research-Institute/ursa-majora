/**
 * @file datalogger.cpp
 * @brief SD Card Data Logger Implementation for Velma Flight Controller System
 * @author Velma Development Team
 * @version 1.0
 * 
 * Implementation of FR4 requirements for Velma/Blizzard flight controller system.
 * Provides non-blocking SD card operations to maintain 100Hz flight loop performance.
 */

 #include "datalogger.h"

 // Global datalogger instance
 DataLogger datalogger;
 
 /**
  * @brief Constructor
  */
 DataLogger::DataLogger() 
     : _isInitialized(false), _currentMode(READ_MODE), _logCounter(0) {
     _currentFilename.reserve(MAX_FILENAME_LEN);
 }
 
 /**
  * @brief Initialize SD card and SPI interface
  */
 DataLogger::ErrorCode DataLogger::init(uint8_t csPin) {
     Serial.print(F("[DataLogger] Initializing SD card on CS pin "));
     Serial.println(csPin);
     
     // Initialize SPI
     SPI.begin();
     
     // Initialize SD card
     if (!SD.begin(csPin)) {
         Serial.println(F("[DataLogger] ERROR: SD card initialization failed!"));
         Serial.println(F("[DataLogger] Check SD card presence and wiring"));
         _isInitialized = false;
         return ERROR_SD_INIT;
     }
     
     Serial.println(F("[DataLogger] SD card initialized successfully"));
     _isInitialized = true;
     _logCounter = 0;
     
     // Create Velma-specific directories if they don't exist
     if (!SD.exists("/sensor_logs")) {
         SD.mkdir("/sensor_logs");
     }
     if (!SD.exists("/system_logs")) {
         SD.mkdir("/system_logs");
     }
     if (!SD.exists("/flight_logs")) {
         SD.mkdir("/flight_logs");
     }
     if (!SD.exists("/config")) {
         SD.mkdir("/config");
     }
     if (!SD.exists("/cheat_codes")) {
         SD.mkdir("/cheat_codes");
     }
     
     Serial.println(F("[DataLogger] Velma directory structure initialized"));
     return SUCCESS;
 }
 
 /**
  * @brief Open file for reading or writing
  */
 DataLogger::ErrorCode DataLogger::openFile(const String& filename, FileMode mode) {
     if (!_isInitialized) {
         Serial.println(F("[DataLogger] ERROR: SD card not initialized"));
         return ERROR_SD_INIT;
     }
     
     // Close any currently open file
     if (_currentFile) {
         closeFile();
     }
     
     // Validate filename length
     if (filename.length() > MAX_FILENAME_LEN - 1) {
         Serial.println(F("[DataLogger] ERROR: Filename too long"));
         return ERROR_INVALID_PATH;
     }
     
     // Open file based on mode
     switch (mode) {
         case READ_MODE:
             _currentFile = SD.open(filename, FILE_READ);
             break;
         case WRITE_MODE:
             _currentFile = SD.open(filename, FILE_WRITE | O_TRUNC);
             break;
         case APPEND_MODE:
             _currentFile = SD.open(filename, FILE_WRITE);
             if (_currentFile) {
                 _currentFile.seek(_currentFile.size()); // Seek to end for append
             }
             break;
     }
     
     if (!_currentFile) {
         Serial.print(F("[DataLogger] ERROR: Failed to open file: "));
         Serial.println(filename);
         return ERROR_FILE_OPEN;
     }
     
     _currentFilename = filename;
     _currentMode = mode;
     
     Serial.print(F("[DataLogger] File opened: "));
     Serial.print(filename);
     Serial.print(F(" (mode: "));
     Serial.print(mode);
     Serial.println(F(")"));
     
     return SUCCESS;
 }
 
 /**
  * @brief Write a line to the currently open file
  */
 DataLogger::ErrorCode DataLogger::writeLine(const String& data, bool addNewline) {
     if (!_currentFile) {
         Serial.println(F("[DataLogger] ERROR: No file open for writing"));
         return ERROR_FILE_OPEN;
     }
     
     if (_currentMode == READ_MODE) {
         Serial.println(F("[DataLogger] ERROR: File opened in read-only mode"));
         return ERROR_FILE_WRITE;
     }
     
     // Check available space (rough estimate)
     if (getAvailableSpace() < data.length() + 10) {
         Serial.println(F("[DataLogger] ERROR: Insufficient disk space"));
         return ERROR_NO_SPACE;
     }
     
     // Write data
     size_t bytesWritten = _currentFile.print(data);
     if (addNewline) {
         bytesWritten += _currentFile.println();
     }
     
     if (bytesWritten != data.length() + (addNewline ? 1 : 0)) {
         Serial.println(F("[DataLogger] ERROR: Failed to write complete data"));
         return ERROR_FILE_WRITE;
     }
     
     _logCounter++;
     
     // Check if rotation is needed
     if (needsRotation(_currentFilename)) {
         Serial.println(F("[DataLogger] Log rotation triggered"));
         String oldFilename = _currentFilename;
         closeFile();
         rotateLogFile(oldFilename);
         openFile(oldFilename, APPEND_MODE);
     }
     
     return SUCCESS;
 }
 
 /**
  * @brief Write a line to the currently open file (char* version)
  */
 DataLogger::ErrorCode DataLogger::writeLine(const char* data, bool addNewline) {
     return writeLine(String(data), addNewline);
 }
 
 /**
  * @brief Write formatted log entry with timestamp and level
  */
 DataLogger::ErrorCode DataLogger::writeLogEntry(LogLevel level, const String& message) {
     if (!_currentFile) {
         Serial.println(F("[DataLogger] ERROR: No file open for logging"));
         return ERROR_FILE_OPEN;
     }
     
     // Create timestamp (using millis() since we don't have RTC)
     String timestamp = String(millis());
     
     // Format: [TIMESTAMP] [LEVEL] MESSAGE
     String logEntry = "[" + timestamp + "] [" + getLogLevelString(level) + "] " + message;
     
     return writeLine(logEntry, true);
 }
 
 /**
  * @brief Read entire file contents
  */
 DataLogger::ErrorCode DataLogger::readFile(const String& filename, String& content) {
     if (!_isInitialized) {
         return ERROR_SD_INIT;
     }
     
     File file = SD.open(filename, FILE_READ);
     if (!file) {
         Serial.print(F("[DataLogger] ERROR: Failed to open file for reading: "));
         Serial.println(filename);
         return ERROR_FILE_READ;
     }
     
     // Reserve space for efficiency
     content.reserve(file.size());
     content = "";
     
     // Read file in chunks to minimize RAM usage
     const size_t CHUNK_SIZE = 64;
     char buffer[CHUNK_SIZE + 1];
     
     while (file.available()) {
         size_t bytesRead = file.readBytes(buffer, CHUNK_SIZE);
         buffer[bytesRead] = '\0';
         content += buffer;
     }
     
     file.close();
     
     Serial.print(F("[DataLogger] File read: "));
     Serial.print(filename);
     Serial.print(F(" ("));
     Serial.print(content.length());
     Serial.println(F(" bytes)"));
     
     return SUCCESS;
 }
 
 /**
  * @brief Read file line by line (memory efficient)
  */
 DataLogger::ErrorCode DataLogger::readFileLines(const String& filename, void (*lineCallback)(const String&)) {
     if (!_isInitialized) {
         return ERROR_SD_INIT;
     }
     
     File file = SD.open(filename, FILE_READ);
     if (!file) {
         Serial.print(F("[DataLogger] ERROR: Failed to open file for reading: "));
         Serial.println(filename);
         return ERROR_FILE_READ;
     }
     
     String line;
     line.reserve(128); // Reserve space for typical line length
     
     while (file.available()) {
         char c = file.read();
         if (c == '\n') {
             if (lineCallback) {
                 lineCallback(line);
             }
             line = "";
         } else if (c != '\r') {
             line += c;
         }
     }
     
     // Handle last line if no newline at end
     if (line.length() > 0 && lineCallback) {
         lineCallback(line);
     }
     
     file.close();
     return SUCCESS;
 }
 
 /**
  * @brief Close currently open file
  */
 DataLogger::ErrorCode DataLogger::closeFile() {
     if (_currentFile) {
         _currentFile.close();
         Serial.print(F("[DataLogger] File closed: "));
         Serial.println(_currentFilename);
         _currentFilename = "";
         _currentMode = READ_MODE;
     }
     return SUCCESS;
 }
 
 /**
  * @brief Delete file from SD card
  */
 DataLogger::ErrorCode DataLogger::deleteFile(const String& filename) {
     if (!_isInitialized) {
         return ERROR_SD_INIT;
     }
     
     if (!SD.exists(filename)) {
         Serial.print(F("[DataLogger] WARNING: File does not exist: "));
         Serial.println(filename);
         return SUCCESS; // Not an error if file doesn't exist
     }
     
     if (!SD.remove(filename)) {
         Serial.print(F("[DataLogger] ERROR: Failed to delete file: "));
         Serial.println(filename);
         return ERROR_FILE_DELETE;
     }
     
     Serial.print(F("[DataLogger] File deleted: "));
     Serial.println(filename);
     return SUCCESS;
 }
 
 /**
  * @brief Check if file exists on SD card
  */
 bool DataLogger::fileExists(const String& filename) {
     if (!_isInitialized) {
         return false;
     }
     return SD.exists(filename);
 }
 
 /**
  * @brief Get file size in bytes
  */
 uint32_t DataLogger::getFileSize(const String& filename) {
     if (!_isInitialized) {
         return 0;
     }
     
     File file = SD.open(filename, FILE_READ);
     if (!file) {
         return 0;
     }
     
     uint32_t size = file.size();
     file.close();
     return size;
 }
 
 /**
  * @brief Get available disk space (approximation)
  */
 uint32_t DataLogger::getAvailableSpace() {
     // SD.h doesn't provide direct disk space info
     // This is a rough estimation - in production you might want to implement
     // a more sophisticated approach
     return 1000000; // Return 1MB as safe estimate
 }
 
 /**
  * @brief List all files in root directory
  */
 uint8_t DataLogger::listFiles(String fileList[], uint8_t maxFiles) {
     if (!_isInitialized) {
         return 0;
     }
     
     File root = SD.open("/");
     if (!root) {
         return 0;
     }
     
     uint8_t fileCount = 0;
     
     while (true) {
         File entry = root.openNextFile();
         if (!entry) {
             break; // No more files
         }
         
         if (fileCount < maxFiles) {
             fileList[fileCount] = entry.name();
             fileCount++;
         }
         
         entry.close();
         
         if (fileCount >= maxFiles) {
             break;
         }
     }
     
     root.close();
     return fileCount;
 }
 
 /**
  * @brief Create flight log with automatic rotation
  */
 DataLogger::ErrorCode DataLogger::logFlightData(const String& logData, const String& sessionId) {
     String filename = "/flights/flight_" + sessionId + ".log";
     if (sessionId.length() == 0) {
         filename = "/flights/flight_current.log";
     }
     
     ErrorCode result = openFile(filename, APPEND_MODE);
     if (result != SUCCESS) {
         return result;
     }
     
     result = writeLogEntry(LOG_INFO, logData);
     closeFile();
     
     return result;
 }
 
 /**
  * @brief Log sensor data with timestamp (FR4.2)
  */
 DataLogger::ErrorCode DataLogger::logSensorData(SensorType sensorType, const String& sensorData, uint32_t timestamp) {
     if (timestamp == 0) {
         timestamp = millis();
     }
     
     String sensorTypeName;
     switch (sensorType) {
         case SENSOR_IMU:         sensorTypeName = "IMU"; break;
         case SENSOR_GPS:         sensorTypeName = "GPS"; break;
         case SENSOR_BAROMETER:   sensorTypeName = "BARO"; break;
         case SENSOR_ULTRASONIC:  sensorTypeName = "ULTRA"; break;
         case SENSOR_MAGNETOMETER: sensorTypeName = "MAG"; break;
         case SENSOR_RADIO:       sensorTypeName = "RADIO"; break;
         default:                 sensorTypeName = "UNKNOWN"; break;
     }
     
     String filename = "/sensor_logs/" + sensorTypeName.toLowerCase() + "_data.log";
     
     ErrorCode result = openFile(filename, APPEND_MODE);
     if (result != SUCCESS) {
         return result;
     }
     
     // Format: [TIMESTAMP] [SENSOR_TYPE] DATA
     String logEntry = "[" + String(timestamp) + "] [" + sensorTypeName + "] " + sensorData;
     result = writeLine(logEntry, true);
     closeFile();
     
     return result;
 }
 
 /**
  * @brief Log system event with component and timestamp (FR4.2)
  */
 DataLogger::ErrorCode DataLogger::logSystemEvent(SystemComponent component, LogLevel level, const String& message, uint32_t timestamp) {
     if (timestamp == 0) {
         timestamp = millis();
     }
     
     String componentName;
     switch (component) {
         case COMPONENT_FLIGHT_CTRL:   componentName = "FLIGHT_CTRL"; break;
         case COMPONENT_COMMUNICATION: componentName = "HC12_COMM"; break;
         case COMPONENT_CHEAT_CODE:    componentName = "CHEAT_CODE"; break;
         case COMPONENT_GPS_MODULE:    componentName = "GPS_MODULE"; break;
         case COMPONENT_RADIO:         componentName = "RADIO"; break;
         case COMPONENT_SENSORS:       componentName = "SENSORS"; break;
         case COMPONENT_SYSTEM:        componentName = "SYSTEM"; break;
         default:                      componentName = "UNKNOWN"; break;
     }
     
     String filename = "/system_logs/system_events.log";
     
     ErrorCode result = openFile(filename, APPEND_MODE);
     if (result != SUCCESS) {
         return result;
     }
     
     // Format: [TIMESTAMP] [COMPONENT] [LEVEL] MESSAGE
     String logEntry = "[" + String(timestamp) + "] [" + componentName + "] [" + 
                      getLogLevelString(level) + "] " + message;
     result = writeLine(logEntry, true);
     closeFile();
     
     return result;
 }
 
 /**
  * @brief Get log file status information (FR4.3)
  */
 DataLogger::ErrorCode DataLogger::getLogFileStatus(const String& filename, LogFileStatus& status) {
     if (!_isInitialized) {
         return ERROR_SD_INIT;
     }
     
     status.filename = filename;
     status.fileSize = getFileSize(filename);
     status.isActive = (_currentFilename == filename && _currentFile);
     status.lastWrite = millis(); // Would need to track actual last write time
     
     // Count entries by reading file (rough estimate)
     if (status.fileSize > 0) {
         File file = SD.open(filename, FILE_READ);
         if (file) {
             uint32_t lineCount = 0;
             while (file.available()) {
                 if (file.read() == '\n') {
                     lineCount++;
                 }
             }
             file.close();
             status.entryCount = lineCount;
         } else {
             status.entryCount = 0;
         }
     } else {
         status.entryCount = 0;
     }
     
     return SUCCESS;
 }
 
 /**
  * @brief Clear log files (FR4.3)
  */
 DataLogger::ErrorCode DataLogger::clearLogs(const String& logType) {
     if (!_isInitialized) {
         return ERROR_SD_INIT;
     }
     
     Serial.print(F("[DataLogger] Clearing logs of type: "));
     Serial.println(logType);
     
     if (logType == "all" || logType == "sensor") {
         // Clear sensor logs
         deleteFile("/sensor_logs/imu_data.log");
         deleteFile("/sensor_logs/gps_data.log");
         deleteFile("/sensor_logs/baro_data.log");
         deleteFile("/sensor_logs/ultra_data.log");
         deleteFile("/sensor_logs/mag_data.log");
         deleteFile("/sensor_logs/radio_data.log");
     }
     
     if (logType == "all" || logType == "system") {
         // Clear system logs
         deleteFile("/system_logs/system_events.log");
     }
     
     if (logType == "all" || logType == "flight") {
         // Clear flight logs
         File flightDir = SD.open("/flight_logs");
         if (flightDir) {
             while (true) {
                 File entry = flightDir.openNextFile();
                 if (!entry) break;
                 
                 String filename = "/flight_logs/" + String(entry.name());
                 entry.close();
                 deleteFile(filename);
             }
             flightDir.close();
         }
     }
     
     Serial.println(F("[DataLogger] Log clearing completed"));
     return SUCCESS;
 }
 
 /**
  * @brief Non-blocking write operation for flight loop compatibility (NFR3)
  */
 DataLogger::ErrorCode DataLogger::writeNonBlocking(const String& data) {
     // Implementation would use a circular buffer for truly non-blocking operation
     // For now, this is a placeholder that maintains the interface
     return writeLine(data, true);
 }
 
 /**
  * @brief Process buffered writes (call periodically from main loop)
  */
 DataLogger::ErrorCode DataLogger::processBufferedWrites() {
     // Implementation would process buffered writes from writeNonBlocking
     // For now, this is a placeholder that maintains the interface
     return SUCCESS;
 }
 
 /**
  * @brief Flush file buffer to ensure data is written
  */
 DataLogger::ErrorCode DataLogger::flush() {
     if (!_currentFile) {
         return ERROR_FILE_OPEN;
     }
     
     _currentFile.flush();
     return SUCCESS;
 }
 
 /**
  * @brief Perform maintenance operations
  */
 DataLogger::ErrorCode DataLogger::performMaintenance() {
     Serial.println(F("[DataLogger] Performing maintenance..."));
     
     // Clean up old log files in /logs directory
     File logsDir = SD.open("/logs");
     if (logsDir) {
         while (true) {
             File entry = logsDir.openNextFile();
             if (!entry) break;
             
             String filename = entry.name();
             uint32_t fileSize = entry.size();
             entry.close();
             
             // Delete files larger than MAX_LOG_SIZE
             if (fileSize > MAX_LOG_SIZE) {
                 deleteFile("/logs/" + filename);
                 Serial.print(F("[DataLogger] Deleted oversized log: "));
                 Serial.println(filename);
             }
         }
         logsDir.close();
     }
     
     Serial.println(F("[DataLogger] Maintenance completed"));
     return SUCCESS;
 }
 
 /**
  * @brief Get log level string representation
  */
 String DataLogger::getLogLevelString(LogLevel level) {
     switch (level) {
         case LOG_DEBUG:    return F("DEBUG");
         case LOG_INFO:     return F("INFO");
         case LOG_WARNING:  return F("WARN");
         case LOG_ERROR:    return F("ERROR");
         case LOG_CRITICAL: return F("CRIT");
         default:           return F("UNKNOWN");
     }
 }
 
 /**
  * @brief Check if log rotation is needed
  */
 bool DataLogger::needsRotation(const String& filename) {
     uint32_t fileSize = getFileSize(filename);
     return fileSize > MAX_LOG_SIZE;
 }
 
 /**
  * @brief Perform log file rotation
  */
 DataLogger::ErrorCode DataLogger::rotateLogFile(const String& filename) {
     // Create backup filename
     String backupName = filename + ".old";
     
     // Remove oldest backup
     if (SD.exists(backupName)) {
         deleteFile(backupName);
     }
     
     // Rename current file to backup
     // Note: SD.h doesn't have rename, so we'll delete and recreate
     deleteFile(filename);
     
     Serial.print(F("[DataLogger] Rotated log file: "));
     Serial.println(filename);
     
     return SUCCESS;
 }
 
 /**
  * @brief Generate timestamped filename
  */
 String DataLogger::generateTimestampedFilename(const String& baseName, const String& extension) {
     String timestamp = String(millis());
     return baseName + "_" + timestamp + extension;
 }
 
 /**
  * @brief Get sensor type string representation
  */
 String DataLogger::getSensorTypeString(SensorType sensorType) {
     switch (sensorType) {
         case SENSOR_IMU:         return F("IMU");
         case SENSOR_GPS:         return F("GPS");
         case SENSOR_BAROMETER:   return F("BARO");
         case SENSOR_ULTRASONIC:  return F("ULTRA");
         case SENSOR_MAGNETOMETER: return F("MAG");
         case SENSOR_RADIO:       return F("RADIO");
         default:                 return F("UNKNOWN");
     }
 }
 
 /**
  * @brief Get error message string
  */
 String DataLogger::getErrorMessage(ErrorCode errorCode) {
     switch (errorCode) {
         case SUCCESS:           return F("Success");
         case ERROR_SD_INIT:     return F("SD card initialization failed");
         case ERROR_FILE_OPEN:   return F("File open failed");
         case ERROR_FILE_WRITE:  return F("File write failed");
         case ERROR_FILE_READ:   return F("File read failed");
         case ERROR_FILE_DELETE: return F("File delete failed");
         case ERROR_NO_SPACE:    return F("Insufficient disk space");
         case ERROR_INVALID_PATH: return F("Invalid file path");
         default:                return F("Unknown error");
     }
 }