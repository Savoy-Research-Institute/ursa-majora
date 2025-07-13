#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <Arduino.h>
#include <SD.h>

namespace Velma {
namespace Logger {

    /**
     * @brief Initialize the SD card logging system.
     * 
     * @param cs_pin Chip Select pin connected to the SD card module.
     * @return true if initialization succeeds.
     * @return false if SD card initialization fails.
     */
    bool init(uint8_t cs_pin);

    /**
     * @brief Log a string message to the SD card log file.
     * 
     * @param message The message to log.
     * @return true if write succeeds.
     * @return false if write fails.
     */
    bool log(const String& message);

    /**
     * @brief Log sensor data with label and timestamp.
     * 
     * @param sensor_name Name of the sensor.
     * @param data Sensor reading or formatted string.
     * @return true if write succeeds.
     * @return false if write fails.
     */
    bool log_sensor(const String& sensor_name, const String& data);

    /**
     * @brief Close the log file safely (e.g. before shutdown).
     */
    void close();

    /**
     * @brief Check if the SD card is initialized and ready.
     * 
     * @return true if SD card is initialized.
     * @return false if SD card is not initialized.
     */
    bool is_initialized();

    /**
     * @brief Check if the log file is open.
     * 
     * @return true if log file is open.
     * @return false if log file is not open.
     */
    bool is_log_open();

    /**
     * @brief Get the name of the current log file.
     * 
     * @return String containing the log file name.
     */
    String get_log_filename();

    /**
     * @brief Get the size of the current log file.
     * 
     * @return size_t containing the size of the log file in bytes.
     */
    size_t get_log_size();

    /**
     * @brief Clear the log file.
     * 
     * @return true if clear succeeds.
     * @return false if clear fails.
     */
    bool clear_log();

    /**
     * @brief Get the current timestamp in a human-readable format.
     * 
     * @return String containing the current timestamp.
     */
    String get_timestamp();

    /**
     * @brief Get the current timestamp in a machine-readable format (e.g. for filenames).
     * 
     * @return String containing the timestamp in YYYYMMDD_HHMMSS format.
     */
    String get_timestamp_machine();

} // namespace Logger
} // namespace Velma

#endif // DATA_LOGGER_H
// End of include/modules/data_logger.h