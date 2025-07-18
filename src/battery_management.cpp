/**
 * @file battery_management.cpp
 * @brief Battery Management System Implementation for Velma Flight Controller
 * @author Velma Development Team
 * @version 1.0
 * @date 2025
 */

 #include "battery_management.h"

 namespace Velma {
 namespace BatteryManagement {
 
 // =============================================================================
 // LIPO DISCHARGE CURVE LOOKUP TABLE
 // =============================================================================
 
 /**
  * @brief LiPo discharge curve lookup table (per cell voltage vs percentage)
  * 
  * This table represents the typical discharge curve for LiPo batteries.
  * Values are per-cell voltages mapped to charge percentages.
  */
 static const struct {
     float voltage;
     float percentage;
 } lipo_discharge_curve[] = {
     {4.20f, 100.0f},  // Full charge
     {4.15f, 95.0f},
     {4.11f, 90.0f},
     {4.08f, 85.0f},
     {4.02f, 80.0f},
     {3.98f, 75.0f},
     {3.95f, 70.0f},
     {3.91f, 65.0f},
     {3.87f, 60.0f},
     {3.85f, 55.0f},
     {3.84f, 50.0f},
     {3.82f, 45.0f},
     {3.80f, 40.0f},
     {3.79f, 35.0f},
     {3.77f, 30.0f},
     {3.75f, 25.0f},
     {3.73f, 20.0f},
     {3.71f, 15.0f},
     {3.69f, 10.0f},
     {3.61f, 5.0f},
     {3.27f, 0.0f}     // Empty (conservative)
 };
 
 static const uint8_t lipo_curve_size = sizeof(lipo_discharge_curve) / sizeof(lipo_discharge_curve[0]);
 
 // =============================================================================
 // CONSTRUCTOR AND DESTRUCTOR
 // =============================================================================
 
 BatteryManager::BatteryManager()
     : analog_pin_(BatteryConfig::DEFAULT_ANALOG_PIN)
     , voltage_divider_ratio_(BatteryConfig::DEFAULT_VOLTAGE_DIVIDER_RATIO)
     , cell_count_(BatteryConfig::DEFAULT_CELL_COUNT)
     , low_voltage_warning_(BatteryConfig::DEFAULT_LOW_VOLTAGE_WARNING)
     , critical_voltage_(BatteryConfig::DEFAULT_CRITICAL_VOLTAGE)
     , is_initialized_(false)
     , voltage_offset_(0.0f)
     , voltage_scale_(1.0f)
 {
     // Initialize battery info structure
     battery_info_.voltage = 0.0f;
     battery_info_.percentage = 0.0f;
     battery_info_.status = BatteryStatus::UNKNOWN;
     battery_info_.last_update_ms = 0;
     battery_info_.is_valid = false;
     battery_info_.temperature = -999.0f;
     battery_info_.power_consumed_wh = 0.0f;
     battery_info_.runtime_seconds = 0;
     
     // Initialize critical response structure
     critical_response_.trigger_disarm = false;
     critical_response_.zero_throttle = false;
     critical_response_.zero_control_channels = false;
     critical_response_.activate_alarm = false;
     critical_response_.response_timestamp = 0;
 }
 
 BatteryManager::~BatteryManager() {
     // Cleanup if needed
 }
 
 // =============================================================================
 // INITIALIZATION AND CONFIGURATION
 // =============================================================================
 
 bool BatteryManager::init(uint8_t analog_pin, float voltage_divider_ratio, uint8_t cell_count) {
     // Validate input parameters
     if (analog_pin >= A0 && analog_pin <= A15) {
         analog_pin_ = analog_pin;
     } else {
         return false;
     }
     
     if (voltage_divider_ratio > 1.0f && voltage_divider_ratio <= 10.0f) {
         voltage_divider_ratio_ = voltage_divider_ratio;
     } else {
         return false;
     }
     
     if (!validate_cell_count(cell_count)) {
         return false;
     }
     cell_count_ = cell_count;
     
     // Configure ADC pin
     pinMode(analog_pin_, INPUT);
     
     // Calculate default safety thresholds based on cell count
     calculate_safety_thresholds(cell_count_, low_voltage_warning_, critical_voltage_);
     
     // Reset calibration
     voltage_offset_ = 0.0f;
     voltage_scale_ = 1.0f;
     
     // Initialize battery info
     battery_info_.status = BatteryStatus::UNKNOWN;
     battery_info_.is_valid = false;
     
     // Mark as initialized
     is_initialized_ = true;
     
     // Perform initial measurement
     update();
     
     return true;
 }
 
 bool BatteryManager::configure_safety_thresholds(float low_voltage_warning, float critical_voltage) {
     if (!is_initialized_) {
         return false;
     }
     
     // Validate thresholds
     float max_voltage = cells_to_total_voltage(cell_count_, BatteryConfig::LIPO_CELL_FULL_VOLTAGE);
     float min_voltage = cells_to_total_voltage(cell_count_, BatteryConfig::LIPO_CELL_EMPTY_VOLTAGE);
     
     if (critical_voltage < min_voltage || critical_voltage > max_voltage) {
         return false;
     }
     
     if (low_voltage_warning <= critical_voltage || low_voltage_warning > max_voltage) {
         return false;
     }
     
     low_voltage_warning_ = low_voltage_warning;
     critical_voltage_ = critical_voltage;
     
     return true;
 }
 
 bool BatteryManager::calibrate_voltage(float known_voltage) {
     if (!is_initialized_) {
         return false;
     }
     
     float measured_voltage = read_raw_voltage();
     if (measured_voltage <= 0.0f) {
         return false;
     }
     
     // Calculate calibration scale factor
     voltage_scale_ = known_voltage / measured_voltage;
     
     // Reasonable bounds check for calibration
     if (voltage_scale_ < 0.8f || voltage_scale_ > 1.2f) {
         voltage_scale_ = 1.0f;  // Reset to default if unreasonable
         return false;
     }
     
     return true;
 }
 
 // =============================================================================
 // VOLTAGE MEASUREMENT
 // =============================================================================
 
 float BatteryManager::get_voltage() {
     if (!is_initialized_) {
         return -1.0f;
     }
     
     float raw_voltage = read_raw_voltage();
     if (raw_voltage < 0.0f) {
         return -1.0f;
     }
     
     // Apply calibration
     float calibrated_voltage = (raw_voltage + voltage_offset_) * voltage_scale_;
     
     // Validate measurement
     if (!validate_voltage_reading(calibrated_voltage)) {
         return -1.0f;
     }
     
     return calibrated_voltage;
 }
 
 float BatteryManager::get_voltage_filtered(uint8_t sample_count) {
     if (!is_initialized_ || sample_count == 0) {
         return -1.0f;
     }
     
     float voltage_sum = 0.0f;
     uint8_t valid_samples = 0;
     
     for (uint8_t i = 0; i < sample_count; i++) {
         float voltage = get_voltage();
         if (voltage > 0.0f) {
             voltage_sum += voltage;
             valid_samples++;
         }
         
         if (i < sample_count - 1) {
             delay(BatteryConfig::SAMPLE_DELAY_MS);
         }
     }
     
     if (valid_samples == 0) {
         return -1.0f;
     }
     
     return voltage_sum / valid_samples;
 }
 
 float BatteryManager::read_raw_voltage() {
     if (!is_initialized_) {
         return -1.0f;
     }
     
     // Read ADC value
     int adc_value = analogRead(analog_pin_);
     
     // Convert to voltage
     float adc_voltage = (adc_value * BatteryConfig::ADC_REFERENCE_VOLTAGE) / BatteryConfig::ADC_RESOLUTION;
     
     // Apply voltage divider scaling
     float battery_voltage = adc_voltage * voltage_divider_ratio_;
     
     return battery_voltage;
 }
 
 bool BatteryManager::validate_voltage_reading(float voltage) {
     // Check for reasonable voltage range
     float min_voltage = cells_to_total_voltage(cell_count_, 2.5f);  // Well below empty
     float max_voltage = cells_to_total_voltage(cell_count_, 4.5f);  // Well above full
     
     return (voltage >= min_voltage && voltage <= max_voltage);
 }
 
 // =============================================================================
 // STATE OF CHARGE ESTIMATION
 // =============================================================================
 
 float BatteryManager::get_remaining_percentage() {
     if (!is_initialized_) {
         return -1.0f;
     }
     
     float voltage = get_voltage();
     if (voltage < 0.0f) {
         return -1.0f;
     }
     
     return calculate_soc_from_voltage(voltage);
 }
 
 float BatteryManager::calculate_soc_from_voltage(float voltage) {
     // Convert total voltage to per-cell voltage
     float cell_voltage = total_voltage_to_per_cell(voltage, cell_count_);
     
     // Handle boundary conditions
     if (cell_voltage >= lipo_discharge_curve[0].voltage) {
         return 100.0f;
     }
     
     if (cell_voltage <= lipo_discharge_curve[lipo_curve_size - 1].voltage) {
         return 0.0f;
     }
     
     // Linear interpolation between lookup table points
     for (uint8_t i = 0; i < lipo_curve_size - 1; i++) {
         if (cell_voltage <= lipo_discharge_curve[i].voltage && 
             cell_voltage >= lipo_discharge_curve[i + 1].voltage) {
             
             float v1 = lipo_discharge_curve[i].voltage;
             float v2 = lipo_discharge_curve[i + 1].voltage;
             float p1 = lipo_discharge_curve[i].percentage;
             float p2 = lipo_discharge_curve[i + 1].percentage;
             
             // Linear interpolation
             float percentage = p1 + (p2 - p1) * (cell_voltage - v1) / (v2 - v1);
             
             return constrain(percentage, 0.0f, 100.0f);
         }
     }
     
     return 0.0f;  // Default fallback
 }
 
 int32_t BatteryManager::get_estimated_runtime_seconds() {
     // Placeholder for future implementation with current sensing
     return -1;
 }
 
 // =============================================================================
 // SAFETY MONITORING
 // =============================================================================
 
 bool BatteryManager::is_low_voltage_warning() {
     if (!is_initialized_) {
         return false;
     }
     
     float voltage = get_voltage();
     return (voltage > 0.0f && voltage <= low_voltage_warning_);
 }
 
 bool BatteryManager::is_critical_voltage() {
     if (!is_initialized_) {
         return false;
     }
     
     float voltage = get_voltage();
     return (voltage > 0.0f && voltage <= critical_voltage_);
 }
 
 bool BatteryManager::is_measurement_valid() {
     return is_initialized_ && battery_info_.is_valid;
 }
 
 // =============================================================================
 // CRITICAL RESPONSE HANDLING
 // =============================================================================
 
 CriticalResponse BatteryManager::trigger_critical_response() {
     // Set critical response flags
     critical_response_.trigger_disarm = true;
     critical_response_.zero_throttle = true;
     critical_response_.zero_control_channels = true;
     critical_response_.activate_alarm = true;
     critical_response_.response_timestamp = millis();
     
     // TODO: Interface with flight control system
     // - Send disarm command to flight controller
     // - Zero throttle output
     // - Zero control channel outputs
     // - Activate alarm/buzzer
     // - Send telemetry alert
     
     return critical_response_;
 }
 
 bool BatteryManager::reset_critical_response() {
     if (!is_initialized_) {
         return false;
     }
     
     // Reset critical response flags
     critical_response_.trigger_disarm = false;
     critical_response_.zero_throttle = false;
     critical_response_.zero_control_channels = false;
     critical_response_.activate_alarm = false;
     critical_response_.response_timestamp = 0;
     
     return true;
 }
 
 // =============================================================================
 // DATA ACCESS AND INTEGRATION
 // =============================================================================
 
 BatteryInfo BatteryManager::get_battery_info() {
     return battery_info_;
 }
 
 BatteryStatus BatteryManager::get_battery_status() {
     return battery_info_.status;
 }
 
 bool BatteryManager::update() {
     if (!is_initialized_) {
         return false;
     }
     
     uint32_t current_time = millis();
     
     // Get current voltage measurement
     float voltage = get_voltage_filtered();
     
     if (voltage < 0.0f) {
         battery_info_.status = BatteryStatus::FAULT;
         battery_info_.is_valid = false;
         return false;
     }
     
     // Update battery info
     battery_info_.voltage = voltage;
     battery_info_.percentage = calculate_soc_from_voltage(voltage);
     battery_info_.last_update_ms = current_time;
     battery_info_.is_valid = true;
     
     // Update battery status
     update_battery_status();
     
     // Handle critical voltage situation
     if (battery_info_.status == BatteryStatus::CRITICAL) {
         trigger_critical_response();
     }
     
     return true;
 }
 
 void BatteryManager::update_battery_status() {
     if (!battery_info_.is_valid) {
         battery_info_.status = BatteryStatus::FAULT;
         return;
     }
     
     if (battery_info_.voltage <= critical_voltage_) {
         battery_info_.status = BatteryStatus::CRITICAL;
     } else if (battery_info_.voltage <= low_voltage_warning_) {
         battery_info_.status = BatteryStatus::LOW_WARNING;
     } else {
         battery_info_.status = BatteryStatus::NORMAL;
     }
 }
 
 // =============================================================================
 // EXTENDED FEATURES (PLACEHOLDERS)
 // =============================================================================
 
 float BatteryManager::get_temperature() {
     // Placeholder for future temperature sensor implementation
     return -999.0f;
 }
 
 float BatteryManager::get_power_consumed_wh() {
     // Placeholder for future current sensing implementation
     return -1.0f;
 }
 
 bool BatteryManager::reset_power_consumption() {
     // Placeholder for future implementation
     return false;
 }
 
 // =============================================================================
 // UTILITY AND DEBUGGING
 // =============================================================================
 
 void BatteryManager::print_battery_info() {
     if (!is_initialized_) {
         Serial.println("Battery Manager not initialized");
         return;
     }
     
     Serial.println("=== Battery Information ===");
     Serial.print("Voltage: ");
     Serial.print(battery_info_.voltage, 2);
     Serial.println(" V");
     
     Serial.print("Percentage: ");
     Serial.print(battery_info_.percentage, 1);
     Serial.println(" %");
     
     Serial.print("Status: ");
     switch (battery_info_.status) {
         case BatteryStatus::NORMAL:
             Serial.println("NORMAL");
             break;
         case BatteryStatus::LOW_WARNING:
             Serial.println("LOW WARNING");
             break;
         case BatteryStatus::CRITICAL:
             Serial.println("CRITICAL");
             break;
         case BatteryStatus::FAULT:
             Serial.println("FAULT");
             break;
         default:
             Serial.println("UNKNOWN");
             break;
     }
     
     Serial.print("Cell Count: ");
     Serial.println(cell_count_);
     
     Serial.print("Warning Threshold: ");
     Serial.print(low_voltage_warning_, 2);
     Serial.println(" V");
     
     Serial.print("Critical Threshold: ");
     Serial.print(critical_voltage_, 2);
     Serial.println(" V");
     
     Serial.print("Last Update: ");
     Serial.print(battery_info_.last_update_ms);
     Serial.println(" ms");
     
     Serial.println("===========================");
 }
 
 // =============================================================================
 // GLOBAL UTILITY FUNCTIONS
 // =============================================================================
 
 float cells_to_total_voltage(uint8_t cell_count, float voltage_per_cell) {
     if (cell_count == 0) {
         return 0.0f;
     }
     return cell_count * voltage_per_cell;
 }
 
 float total_voltage_to_per_cell(float total_voltage, uint8_t cell_count) {
     if (cell_count == 0) {
         return 0.0f;
     }
     return total_voltage / cell_count;
 }
 
 bool validate_cell_count(uint8_t cell_count) {
     // Support 1S to 6S LiPo configurations
     return (cell_count >= 1 && cell_count <= 6);
 }
 
 bool calculate_safety_thresholds(uint8_t cell_count, 
                                 float& low_voltage_warning, 
                                 float& critical_voltage) {
     if (!validate_cell_count(cell_count)) {
         return false;
     }
     
     // Calculate thresholds based on per-cell voltages
     // Warning: 3.7V per cell (nominal voltage)
     // Critical: 3.5V per cell (safe discharge limit)
     low_voltage_warning = cells_to_total_voltage(cell_count, 3.7f);
     critical_voltage = cells_to_total_voltage(cell_count, 3.5f);
     
     return true;
 }
 
 } // namespace BatteryManagement
 } // namespace Velma