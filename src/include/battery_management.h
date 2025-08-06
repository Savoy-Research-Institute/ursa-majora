/**
 * @file battery_management.h
 * @brief Battery Management System for Velma Flight Controller
 * @author Velma Development Team
 * @version 1.0
 * @date 2025
 * 
 * @details
 * This module provides comprehensive battery monitoring and management for LiPo batteries
 * in the Velma flight control system. It includes voltage monitoring, State of Charge (SOC)
 * estimation, safety thresholds, and critical response handling.
 * 
 * Hardware Requirements:
 * - Arduino Mega with analog input pin
 * - LiPo battery (3S or 4S supported)
 * - Voltage divider circuit for safe voltage measurement
 * 
 * Safety Features:
 * - Configurable low voltage warning
 * - Critical voltage cut-off protection
 * - Non-blocking real-time operation
 * - Integration with flight control safety systems
 */

 #ifndef BATTERY_MANAGEMENT_H
 #define BATTERY_MANAGEMENT_H
 
 #include <Arduino.h>
 
 namespace Velma {
 namespace BatteryManagement {
 
 // =============================================================================
 // CONFIGURATION CONSTANTS
 // =============================================================================
 
 /**
  * @brief Default configuration values for battery management
  */
 struct BatteryConfig {
     // Hardware Configuration
     static constexpr uint8_t DEFAULT_ANALOG_PIN = A0;          ///< Default analog pin for voltage measurement
     static constexpr float DEFAULT_VOLTAGE_DIVIDER_RATIO = 5.0f; ///< Default voltage divider ratio (R1+R2)/R2
     static constexpr float ADC_REFERENCE_VOLTAGE = 5.0f;        ///< Arduino ADC reference voltage
     static constexpr uint16_t ADC_RESOLUTION = 1024;           ///< 10-bit ADC resolution
     
     // Battery Type Configuration
     static constexpr uint8_t DEFAULT_CELL_COUNT = 3;           ///< Default 3S LiPo configuration
     static constexpr float LIPO_CELL_NOMINAL_VOLTAGE = 3.7f;   ///< Nominal voltage per LiPo cell
     static constexpr float LIPO_CELL_FULL_VOLTAGE = 4.2f;      ///< Full charge voltage per LiPo cell
     static constexpr float LIPO_CELL_EMPTY_VOLTAGE = 3.0f;     ///< Empty voltage per LiPo cell
     
     // Safety Thresholds (3S LiPo defaults)
     static constexpr float DEFAULT_LOW_VOLTAGE_WARNING = 11.1f; ///< Warning threshold (3.7V per cell)
     static constexpr float DEFAULT_CRITICAL_VOLTAGE = 10.5f;    ///< Critical cut-off (3.5V per cell)
     
     // Measurement Configuration
     static constexpr uint8_t VOLTAGE_SAMPLES = 10;             ///< Number of samples for voltage averaging
     static constexpr uint16_t SAMPLE_DELAY_MS = 1;             ///< Delay between voltage samples (ms)
 };
 
 /**
  * @brief Battery status enumeration
  */
 enum class BatteryStatus {
     UNKNOWN = 0,        ///< Battery status unknown/not initialized
     NORMAL = 1,         ///< Battery voltage normal
     LOW_WARNING = 2,    ///< Battery voltage below warning threshold
     CRITICAL = 3,       ///< Battery voltage at critical level
     FAULT = 4          ///< Battery measurement fault detected
 };
 
 /**
  * @brief Battery information structure
  */
 struct BatteryInfo {
     float voltage;                  ///< Current battery voltage (V)
     float percentage;               ///< Battery charge percentage (0-100%)
     BatteryStatus status;           ///< Current battery status
     uint32_t last_update_ms;        ///< Last update timestamp (milliseconds)
     bool is_valid;                  ///< Data validity flag
     
     // Extended data (for future features)
     float temperature;              ///< Battery temperature (°C) - placeholder
     float power_consumed_wh;        ///< Total power consumed (Wh) - placeholder
     uint32_t runtime_seconds;       ///< Runtime since last full charge - placeholder
 };
 
 /**
  * @brief Critical response structure for safety systems
  */
 struct CriticalResponse {
     bool trigger_disarm;            ///< Flag to trigger system disarm
     bool zero_throttle;             ///< Flag to zero throttle output
     bool zero_control_channels;     ///< Flag to zero all control channels
     bool activate_alarm;            ///< Flag to activate audio/visual alarms
     uint32_t response_timestamp;    ///< Timestamp when response was triggered
 };
 
 // =============================================================================
 // BATTERY MANAGEMENT CLASS
 // =============================================================================
 
 /**
  * @brief Main Battery Management class for Velma system
  * 
  * This class provides comprehensive battery monitoring and management functionality
  * for LiPo batteries in the Velma flight control system. It includes voltage
  * measurement, SOC estimation, safety monitoring, and critical response handling.
  */
 class BatteryManager {
 private:
     // Hardware configuration
     uint8_t analog_pin_;                    ///< Analog pin for voltage measurement
     float voltage_divider_ratio_;           ///< Voltage divider scaling ratio
     uint8_t cell_count_;                    ///< Number of LiPo cells (3S, 4S, etc.)
     
     // Safety thresholds
     float low_voltage_warning_;             ///< Low voltage warning threshold
     float critical_voltage_;                ///< Critical voltage cut-off threshold
     
     // Runtime data
     BatteryInfo battery_info_;              ///< Current battery information
     CriticalResponse critical_response_;    ///< Critical response state
     bool is_initialized_;                   ///< Initialization status flag
     
     // Calibration and filtering
     float voltage_offset_;                  ///< Voltage measurement offset calibration
     float voltage_scale_;                   ///< Voltage measurement scale calibration
     
     // Internal helper methods
     float read_raw_voltage();
     float filter_voltage(float raw_voltage);
     float calculate_soc_from_voltage(float voltage);
     void update_battery_status();
     bool validate_voltage_reading(float voltage);
 
 public:
     /**
      * @brief Constructor for BatteryManager
      */
     BatteryManager();
     
     /**
      * @brief Destructor for BatteryManager
      */
     ~BatteryManager();
     
     // =============================================================================
     // INITIALIZATION AND CONFIGURATION
     // =============================================================================
     
     /**
      * @brief Initialize the battery management system
      * 
      * @param analog_pin Analog pin connected to voltage divider (default: A0)
      * @param voltage_divider_ratio Voltage divider ratio (R1+R2)/R2 (default: 5.0)
      * @param cell_count Number of LiPo cells in series (default: 3)
      * @return true if initialization successful, false otherwise
      */
     bool init(uint8_t analog_pin = BatteryConfig::DEFAULT_ANALOG_PIN,
               float voltage_divider_ratio = BatteryConfig::DEFAULT_VOLTAGE_DIVIDER_RATIO,
               uint8_t cell_count = BatteryConfig::DEFAULT_CELL_COUNT);
     
     /**
      * @brief Configure safety thresholds
      * 
      * @param low_voltage_warning Warning threshold voltage (V)
      * @param critical_voltage Critical cut-off voltage (V)
      * @return true if configuration successful, false otherwise
      */
     bool configure_safety_thresholds(float low_voltage_warning, float critical_voltage);
     
     /**
      * @brief Calibrate voltage measurement
      * 
      * @param known_voltage Known actual battery voltage for calibration
      * @return true if calibration successful, false otherwise
      */
     bool calibrate_voltage(float known_voltage);
     
     // =============================================================================
     // VOLTAGE MEASUREMENT
     // =============================================================================
     
     /**
      * @brief Get current battery voltage
      * 
      * @return Current battery voltage in volts, or -1.0 if error
      */
     float get_voltage();
     
     /**
      * @brief Get filtered and averaged battery voltage
      * 
      * @param sample_count Number of samples to average (default: 10)
      * @return Averaged battery voltage in volts, or -1.0 if error
      */
     float get_voltage_filtered(uint8_t sample_count = BatteryConfig::VOLTAGE_SAMPLES);
     
     // =============================================================================
     // STATE OF CHARGE ESTIMATION
     // =============================================================================
     
     /**
      * @brief Get estimated battery charge percentage
      * 
      * @return Battery charge percentage (0-100%), or -1.0 if error
      */
     float get_remaining_percentage();
     
     /**
      * @brief Get estimated runtime remaining
      * 
      * @return Estimated runtime in seconds, or -1 if cannot estimate
      */
     int32_t get_estimated_runtime_seconds();
     
     // =============================================================================
     // SAFETY MONITORING
     // =============================================================================
     
     /**
      * @brief Check if battery voltage is below warning threshold
      * 
      * @return true if voltage is below warning threshold, false otherwise
      */
     bool is_low_voltage_warning();
     
     /**
      * @brief Check if battery voltage is at critical level
      * 
      * @return true if voltage is at critical level, false otherwise
      */
     bool is_critical_voltage();
     
     /**
      * @brief Check if battery measurement is valid
      * 
      * @return true if measurement is valid, false if fault detected
      */
     bool is_measurement_valid();
     
     // =============================================================================
     // CRITICAL RESPONSE HANDLING
     // =============================================================================
     
     /**
      * @brief Trigger critical voltage response
      * 
      * This function handles critical voltage situations by:
      * - Setting throttle to zero
      * - Zeroing control channels
      * - Triggering system disarm
      * - Activating alarms
      * 
      * @return CriticalResponse structure with response actions
      */
     CriticalResponse trigger_critical_response();
     
     /**
      * @brief Reset critical response state
      * 
      * @return true if reset successful, false otherwise
      */
     bool reset_critical_response();
     
     // =============================================================================
     // DATA ACCESS AND INTEGRATION
     // =============================================================================
     
     /**
      * @brief Get comprehensive battery information
      * 
      * @return BatteryInfo structure with all battery data
      */
     BatteryInfo get_battery_info();
     
     /**
      * @brief Get battery status enumeration
      * 
      * @return Current BatteryStatus
      */
     BatteryStatus get_battery_status();
     
     /**
      * @brief Update battery measurements (call regularly in main loop)
      * 
      * @return true if update successful, false otherwise
      */
     bool update();
     
     // =============================================================================
     // EXTENDED FEATURES (PLACEHOLDERS)
     // =============================================================================
     
     /**
      * @brief Get battery temperature (placeholder for future implementation)
      * 
      * @return Battery temperature in Celsius, or -999.0 if not available
      */
     float get_temperature();
     
     /**
      * @brief Get total power consumed (placeholder for future current sensing)
      * 
      * @return Total power consumed in Watt-hours, or -1.0 if not available
      */
     float get_power_consumed_wh();
     
     /**
      * @brief Reset power consumption counter (placeholder)
      * 
      * @return true if reset successful, false otherwise
      */
     bool reset_power_consumption();
     
     // =============================================================================
     // UTILITY AND DEBUGGING
     // =============================================================================
     
     /**
      * @brief Get initialization status
      * 
      * @return true if battery manager is initialized, false otherwise
      */
     bool is_initialized() const { return is_initialized_; }
     
     /**
      * @brief Print battery information to Serial (for debugging)
      */
     void print_battery_info();
     
     /**
      * @brief Get version information
      * 
      * @return Version string
      */
     const char* get_version() const { return "1.0.0"; }
 };
 
 // =============================================================================
 // GLOBAL UTILITY FUNCTIONS
 // =============================================================================
 
 /**
  * @brief Convert cell count and per-cell voltage to total voltage
  * 
  * @param cell_count Number of cells in series
  * @param voltage_per_cell Voltage per individual cell
  * @return Total battery pack voltage
  */
 float cells_to_total_voltage(uint8_t cell_count, float voltage_per_cell);
 
 /**
  * @brief Convert total voltage to per-cell voltage
  * 
  * @param total_voltage Total battery pack voltage
  * @param cell_count Number of cells in series
  * @return Voltage per individual cell
  */
 float total_voltage_to_per_cell(float total_voltage, uint8_t cell_count);
 
 /**
  * @brief Validate LiPo cell count
  * 
  * @param cell_count Cell count to validate
  * @return true if valid (1-6S), false otherwise
  */
 bool validate_cell_count(uint8_t cell_count);
 
 /**
  * @brief Calculate recommended safety thresholds for given cell count
  * 
  * @param cell_count Number of cells
  * @param low_voltage_warning Output: recommended warning threshold
  * @param critical_voltage Output: recommended critical threshold
  * @return true if calculation successful, false otherwise
  */
 bool calculate_safety_thresholds(uint8_t cell_count, 
                                 float& low_voltage_warning, 
                                 float& critical_voltage);
 
 } // namespace BatteryManagement
 } // namespace Velma
 
 #endif // BATTERY_MANAGEMENT_H