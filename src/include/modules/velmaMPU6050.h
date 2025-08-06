/**
 * @file mpu6050.h
 * @brief MPU6050 IMU Driver Module for Velma Flight Control System
 * @author Velma Development Team
 * @version 1.0
 * @date 2025
 * 
 * @details
 * This module provides a comprehensive interface for the MPU6050 6-axis IMU sensor
 * integrated into the Velma flight control system. Supports accelerometer, gyroscope,
 * temperature sensing, DMP (Digital Motion Processor), and FIFO buffer operations.
 * 
 * Hardware Configuration:
 * - Arduino Mega 2560
 * - MPU6050 connected via I2C (SDA: Pin 20, SCL: Pin 21)
 * - I2C Address: 0x68
 * - Compatible with GY-521 breakout boards
 */

 #ifndef VELMA_IMU_MPU6050_H
 #define VELMA_IMU_MPU6050_H
 
 #include <Arduino.h>
 #include <Wire.h>
 #include <MPU6050.h>
 
 namespace Velma {
 namespace IMU {
 
 /**
  * @brief MPU6050 sensor data structure for raw readings
  */
 struct RawIMUData {
     int16_t accel_x;  ///< Raw accelerometer X-axis
     int16_t accel_y;  ///< Raw accelerometer Y-axis
     int16_t accel_z;  ///< Raw accelerometer Z-axis
     int16_t gyro_x;   ///< Raw gyroscope X-axis
     int16_t gyro_y;   ///< Raw gyroscope Y-axis
     int16_t gyro_z;   ///< Raw gyroscope Z-axis
     int16_t temp;     ///< Raw temperature
 };
 
 /**
  * @brief MPU6050 sensor data structure for scaled readings
  */
 struct ScaledIMUData {
     float accel_x;    ///< Accelerometer X-axis (m/s²)
     float accel_y;    ///< Accelerometer Y-axis (m/s²)
     float accel_z;    ///< Accelerometer Z-axis (m/s²)
     float gyro_x;     ///< Gyroscope X-axis (deg/s)
     float gyro_y;     ///< Gyroscope Y-axis (deg/s)
     float gyro_z;     ///< Gyroscope Z-axis (deg/s)
     float temp;       ///< Temperature (°C)
 };
 
 /**
  * @brief Euler angles structure for orientation
  */
 struct EulerAngles {
     float roll;       ///< Roll angle (degrees)
     float pitch;      ///< Pitch angle (degrees)
     float yaw;        ///< Yaw angle (degrees)
 };
 
 /**
  * @brief Quaternion structure for DMP output
  */
 struct Quaternion {
     float w;          ///< W component
     float x;          ///< X component
     float y;          ///< Y component
     float z;          ///< Z component
 };
 
 /**
  * @brief Calibration offsets structure
  */
 struct CalibrationOffsets {
     int16_t accel_x_offset;   ///< Accelerometer X-axis offset
     int16_t accel_y_offset;   ///< Accelerometer Y-axis offset
     int16_t accel_z_offset;   ///< Accelerometer Z-axis offset
     int16_t gyro_x_offset;    ///< Gyroscope X-axis offset
     int16_t gyro_y_offset;    ///< Gyroscope Y-axis offset
     int16_t gyro_z_offset;    ///< Gyroscope Z-axis offset
 };
 
 /**
  * @brief MPU6050 configuration parameters
  */
 enum class GyroRange {
     RANGE_250_DPS = 0,    ///< ±250 degrees/second
     RANGE_500_DPS = 1,    ///< ±500 degrees/second
     RANGE_1000_DPS = 2,   ///< ±1000 degrees/second
     RANGE_2000_DPS = 3    ///< ±2000 degrees/second
 };
 
 enum class AccelRange {
     RANGE_2G = 0,         ///< ±2g
     RANGE_4G = 1,         ///< ±4g
     RANGE_8G = 2,         ///< ±8g
     RANGE_16G = 3         ///< ±16g
 };
 
 enum class DLPF {
     DLPF_260HZ = 0,       ///< 260Hz bandwidth
     DLPF_184HZ = 1,       ///< 184Hz bandwidth
     DLPF_94HZ = 2,        ///< 94Hz bandwidth
     DLPF_44HZ = 3,        ///< 44Hz bandwidth
     DLPF_21HZ = 4,        ///< 21Hz bandwidth
     DLPF_10HZ = 5,        ///< 10Hz bandwidth
     DLPF_5HZ = 6          ///< 5Hz bandwidth
 };
 
 /**
  * @class MPU6050Driver
  * @brief Comprehensive MPU6050 IMU driver for Velma flight control system
  * 
  * This class provides a complete interface for the MPU6050 sensor including:
  * - Initialization and configuration
  * - Raw and scaled data reading
  * - Calibration and offset management
  * - Euler angle calculation
  * - DMP (Digital Motion Processor) support
  * - FIFO buffer management
  * - Interrupt handling
  */
 class MPU6050Driver {
 public:
     /**
      * @brief Constructor for MPU6050Driver
      * @param interrupt_pin Optional interrupt pin for motion detection (default: -1, disabled)
      */
     explicit MPU6050Driver(int interrupt_pin = -1);
 
     /**
      * @brief Destructor
      */
     ~MPU6050Driver();
 
     // === INITIALIZATION AND CONFIGURATION ===
 
     /**
      * @brief Initialize the MPU6050 sensor
      * @param gyro_range Gyroscope full-scale range
      * @param accel_range Accelerometer full-scale range
      * @param dlpf_setting Digital Low Pass Filter setting
      * @return true if initialization successful, false otherwise
      */
     bool init(GyroRange gyro_range = GyroRange::RANGE_500_DPS,
               AccelRange accel_range = AccelRange::RANGE_4G,
               DLPF dlpf_setting = DLPF::DLPF_44HZ);
 
     /**
      * @brief Test connection to MPU6050
      * @return true if device responds correctly, false otherwise
      */
     bool test_connection();
 
     /**
      * @brief Reset the MPU6050 to default settings
      * @return true if reset successful, false otherwise
      */
     bool reset();
 
     // === CALIBRATION ===
 
     /**
      * @brief Calibrate the MPU6050 sensor
      * @param samples Number of samples to use for calibration (default: 2000)
      * @param delay_ms Delay between samples in milliseconds (default: 3)
      * @return true if calibration successful, false otherwise
      * 
      * @note This function is blocking and should be called during initialization
      * The sensor must be stationary during calibration
      */
     bool calibrate(uint16_t samples = 2000, uint8_t delay_ms = 3);
 
     /**
      * @brief Get current calibration offsets
      * @return Current calibration offsets structure
      */
     CalibrationOffsets get_calibration_offsets() const;
 
     /**
      * @brief Set calibration offsets manually
      * @param offsets Calibration offsets to apply
      * @return true if offsets applied successfully, false otherwise
      */
     bool set_calibration_offsets(const CalibrationOffsets& offsets);
 
     // === DATA READING ===
 
     /**
      * @brief Read raw sensor data (non-blocking)
      * @param data Reference to structure to store raw data
      * @return true if read successful, false otherwise
      */
     bool read_raw(RawIMUData& data);
 
     /**
      * @brief Read scaled sensor data (non-blocking)
      * @param data Reference to structure to store scaled data
      * @return true if read successful, false otherwise
      */
     bool read_scaled(ScaledIMUData& data);
 
     /**
      * @brief Get temperature reading
      * @return Temperature in degrees Celsius
      */
     float get_temperature();
 
     // === ORIENTATION CALCULATION ===
 
     /**
      * @brief Calculate pitch and roll from accelerometer data
      * @param angles Reference to structure to store calculated angles
      * @return true if calculation successful, false otherwise
      * 
      * @note This function uses only accelerometer data and is suitable for
      * static or low-dynamic conditions
      */
     bool get_pitch_roll(EulerAngles& angles);
 
     /**
      * @brief Calculate pitch, roll, and yaw using complementary filter
      * @param angles Reference to structure to store calculated angles
      * @param dt Time step in seconds
      * @return true if calculation successful, false otherwise
      * 
      * @note This function combines accelerometer and gyroscope data
      * for more accurate orientation estimation
      */
     bool get_euler_angles(EulerAngles& angles, float dt);
 
     // === DMP (Digital Motion Processor) SUPPORT ===
 
     /**
      * @brief Initialize DMP (Digital Motion Processor)
      * @return true if DMP initialization successful, false otherwise
      */
     bool init_dmp();
 
     /**
      * @brief Check if DMP is enabled
      * @return true if DMP is enabled, false otherwise
      */
     bool is_dmp_enabled() const;
 
     /**
      * @brief Read quaternion from DMP
      * @param quat Reference to quaternion structure
      * @return true if read successful, false otherwise
      */
     bool read_dmp_quaternion(Quaternion& quat);
 
     /**
      * @brief Convert quaternion to Euler angles
      * @param quat Input quaternion
      * @param angles Output Euler angles
      */
     void quaternion_to_euler(const Quaternion& quat, EulerAngles& angles);
 
     // === FIFO BUFFER MANAGEMENT ===
 
     /**
      * @brief Initialize FIFO buffer
      * @param enable_accel Enable accelerometer data in FIFO
      * @param enable_gyro Enable gyroscope data in FIFO
      * @return true if FIFO initialization successful, false otherwise
      */
     bool init_fifo(bool enable_accel = true, bool enable_gyro = true);
 
     /**
      * @brief Get FIFO buffer count
      * @return Number of bytes available in FIFO buffer
      */
     uint16_t get_fifo_count();
 
     /**
      * @brief Read data from FIFO buffer
      * @param buffer Buffer to store FIFO data
      * @param length Number of bytes to read
      * @return Number of bytes actually read
      */
     uint16_t read_fifo(uint8_t* buffer, uint16_t length);
 
     /**
      * @brief Reset FIFO buffer
      * @return true if reset successful, false otherwise
      */
     bool reset_fifo();
 
     // === INTERRUPT SUPPORT ===
 
     /**
      * @brief Configure motion detection interrupt
      * @param threshold Motion detection threshold (0-255)
      * @param duration Motion detection duration (0-255)
      * @return true if configuration successful, false otherwise
      */
     bool configure_motion_interrupt(uint8_t threshold = 2, uint8_t duration = 40);
 
     /**
      * @brief Check if motion interrupt has occurred
      * @return true if motion detected, false otherwise
      */
     bool is_motion_detected();
 
     /**
      * @brief Clear motion interrupt
      * @return true if cleared successfully, false otherwise
      */
     bool clear_motion_interrupt();
 
     // === POWER MANAGEMENT ===
 
     /**
      * @brief Put MPU6050 into sleep mode
      * @return true if sleep mode entered successfully, false otherwise
      */
     bool sleep();
 
     /**
      * @brief Wake up MPU6050 from sleep mode
      * @return true if wake up successful, false otherwise
      */
     bool wake();
 
     /**
      * @brief Check if MPU6050 is in sleep mode
      * @return true if in sleep mode, false otherwise
      */
     bool is_sleeping();
 
     // === UTILITY FUNCTIONS ===
 
     /**
      * @brief Get current sensor configuration
      * @param gyro_range Reference to store gyroscope range
      * @param accel_range Reference to store accelerometer range
      * @param dlpf_setting Reference to store DLPF setting
      */
     void get_configuration(GyroRange& gyro_range, AccelRange& accel_range, DLPF& dlpf_setting) const;
 
     /**
      * @brief Print sensor status and configuration for debugging
      */
     void print_status() const;
 
     /**
      * @brief Get last error message
      * @return Pointer to last error message string
      */
     const char* get_last_error() const;
 
 private:
     // === PRIVATE MEMBERS ===
     MPU6050 mpu_;                      ///< MPU6050 library instance
     int interrupt_pin_;                ///< Interrupt pin (-1 if disabled)
     bool initialized_;                 ///< Initialization status
     bool calibrated_;                  ///< Calibration status
     bool dmp_enabled_;                 ///< DMP status
     
     // Configuration
     GyroRange gyro_range_;             ///< Current gyroscope range
     AccelRange accel_range_;           ///< Current accelerometer range
     DLPF dlpf_setting_;                ///< Current DLPF setting
     
     // Calibration offsets
     CalibrationOffsets offsets_;       ///< Current calibration offsets
     
     // Scale factors
     float gyro_scale_;                 ///< Gyroscope scale factor (LSB/deg/s)
     float accel_scale_;                ///< Accelerometer scale factor (LSB/g)
     
     // Complementary filter state
     EulerAngles filtered_angles_;      ///< Current filtered angles
     uint32_t last_update_time_;        ///< Last update timestamp
     
     // Error handling
     mutable char error_message_[64];   ///< Last error message buffer
     
     // === PRIVATE HELPER FUNCTIONS ===
     
     /**
      * @brief Set scale factors based on current configuration
      */
     void update_scale_factors();
     
     /**
      * @brief Set error message
      * @param message Error message to set
      */
     void set_error(const char* message) const;
     
     /**
      * @brief Convert raw accelerometer data to m/s²
      * @param raw_value Raw accelerometer value
      * @return Acceleration in m/s²
      */
     float raw_accel_to_ms2(int16_t raw_value) const;
     
     /**
      * @brief Convert raw gyroscope data to deg/s
      * @param raw_value Raw gyroscope value
      * @return Angular velocity in deg/s
      */
     float raw_gyro_to_dps(int16_t raw_value) const;
     
     /**
      * @brief Calculate pitch from accelerometer data
      * @param ax Accelerometer X-axis (m/s²)
      * @param ay Accelerometer Y-axis (m/s²)
      * @param az Accelerometer Z-axis (m/s²)
      * @return Pitch angle in degrees
      */
     float calculate_pitch(float ax, float ay, float az) const;
     
     /**
      * @brief Calculate roll from accelerometer data
      * @param ax Accelerometer X-axis (m/s²)
      * @param ay Accelerometer Y-axis (m/s²)
      * @param az Accelerometer Z-axis (m/s²)
      * @return Roll angle in degrees
      */
     float calculate_roll(float ax, float ay, float az) const;
     
     /**
      * @brief Apply complementary filter to angle calculation
      * @param accel_angle Angle calculated from accelerometer
      * @param gyro_rate Gyroscope angular velocity
      * @param filtered_angle Current filtered angle
      * @param dt Time step
      * @param alpha Filter coefficient (0.0 to 1.0)
      * @return Filtered angle
      */
     float complementary_filter(float accel_angle, float gyro_rate, float filtered_angle, float dt, float alpha = 0.96f) const;
 };
 
 } // namespace IMU
 } // namespace Velma
 
 #endif // VELMA_IMU_MPU6050_H