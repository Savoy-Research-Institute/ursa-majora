/**
 * @file mpu6050.cpp
 * @brief MPU6050 IMU Driver Implementation for Velma Flight Control System
 * @author Velma Development Team
 * @version 1.0
 * @date 2025
 */

 #include "mpu6050.h"
 #include <math.h>
 
 namespace Velma {
 namespace IMU {
 
 // === CONSTANTS ===
 static constexpr float GRAVITY_MS2 = 9.80665f;  ///< Standard gravity in m/s²
 static constexpr float RAD_TO_DEG = 180.0f / M_PI;  ///< Radians to degrees conversion
 static constexpr float DEG_TO_RAD = M_PI / 180.0f;  ///< Degrees to radians conversion
 
 // === CONSTRUCTOR / DESTRUCTOR ===
 
 MPU6050Driver::MPU6050Driver(int interrupt_pin)
     : interrupt_pin_(interrupt_pin)
     , initialized_(false)
     , calibrated_(false)
     , dmp_enabled_(false)
     , gyro_range_(GyroRange::RANGE_500_DPS)
     , accel_range_(AccelRange::RANGE_4G)
     , dlpf_setting_(DLPF::DLPF_44HZ)
     , gyro_scale_(65.5f)
     , accel_scale_(8192.0f)
     , last_update_time_(0)
 {
     // Initialize structures
     memset(&offsets_, 0, sizeof(offsets_));
     memset(&filtered_angles_, 0, sizeof(filtered_angles_));
     memset(error_message_, 0, sizeof(error_message_));
 }
 
 MPU6050Driver::~MPU6050Driver() {
     if (initialized_) {
         sleep();
     }
 }
 
 // === INITIALIZATION AND CONFIGURATION ===
 
 bool MPU6050Driver::init(GyroRange gyro_range, AccelRange accel_range, DLPF dlpf_setting) {
     // Initialize I2C
     Wire.begin();
     
     // Initialize MPU6050
     mpu_.initialize();
     
     // Test connection
     if (!test_connection()) {
         set_error("MPU6050 connection test failed");
         return false;
     }
     
     // Store configuration
     gyro_range_ = gyro_range;
     accel_range_ = accel_range;
     dlpf_setting_ = dlpf_setting;
     
     // Set full scale ranges
     mpu_.setFullScaleGyroRange(static_cast<uint8_t>(gyro_range));
     mpu_.setFullScaleAccelRange(static_cast<uint8_t>(accel_range));
     
     // Set DLPF
     mpu_.setDLPFMode(static_cast<uint8_t>(dlpf_setting));
     
     // Update scale factors
     update_scale_factors();
     
     // Configure interrupt pin if specified
     if (interrupt_pin_ >= 0) {
         pinMode(interrupt_pin_, INPUT);
         mpu_.setIntEnabled(0x00);  // Disable all interrupts initially
     }
     
     // Reset FIFO
     mpu_.resetFIFO();
     
     initialized_ = true;
     last_update_time_ = millis();
     
     return true;
 }
 
 bool MPU6050Driver::test_connection() {
     return mpu_.testConnection();
 }
 
 bool MPU6050Driver::reset() {
     if (!initialized_) {
         set_error("MPU6050 not initialized");
         return false;
     }
     
     // Reset device
     mpu_.reset();
     delay(100);  // Wait for reset to complete
     
     // Reinitialize
     calibrated_ = false;
     dmp_enabled_ = false;
     
     return init(gyro_range_, accel_range_, dlpf_setting_);
 }
 
 // === CALIBRATION ===
 
 bool MPU6050Driver::calibrate(uint16_t samples, uint8_t delay_ms) {
     if (!initialized_) {
         set_error("MPU6050 not initialized");
         return false;
     }
     
     int32_t accel_sum[3] = {0, 0, 0};
     int32_t gyro_sum[3] = {0, 0, 0};
     
     // Collect samples
     for (uint16_t i = 0; i < samples; i++) {
         RawIMUData data;
         if (!read_raw(data)) {
             set_error("Failed to read data during calibration");
             return false;
         }
         
         accel_sum[0] += data.accel_x;
         accel_sum[1] += data.accel_y;
         accel_sum[2] += data.accel_z;
         
         gyro_sum[0] += data.gyro_x;
         gyro_sum[1] += data.gyro_y;
         gyro_sum[2] += data.gyro_z;
         
         delay(delay_ms);
     }
     
     // Calculate offsets
     offsets_.accel_x_offset = accel_sum[0] / samples;
     offsets_.accel_y_offset = accel_sum[1] / samples;
     offsets_.accel_z_offset = (accel_sum[2] / samples) - static_cast<int16_t>(accel_scale_);  // Remove gravity
     
     offsets_.gyro_x_offset = gyro_sum[0] / samples;
     offsets_.gyro_y_offset = gyro_sum[1] / samples;
     offsets_.gyro_z_offset = gyro_sum[2] / samples;
     
     // Apply offsets to MPU6050
     mpu_.setXAccelOffset(offsets_.accel_x_offset);
     mpu_.setYAccelOffset(offsets_.accel_y_offset);
     mpu_.setZAccelOffset(offsets_.accel_z_offset);
     
     mpu_.setXGyroOffset(offsets_.gyro_x_offset);
     mpu_.setYGyroOffset(offsets_.gyro_y_offset);
     mpu_.setZGyroOffset(offsets_.gyro_z_offset);
     
     calibrated_ = true;
     return true;
 }
 
 CalibrationOffsets MPU6050Driver::get_calibration_offsets() const {
     return offsets_;
 }
 
 bool MPU6050Driver::set_calibration_offsets(const CalibrationOffsets& offsets) {
     if (!initialized_) {
         set_error("MPU6050 not initialized");
         return false;
     }
     
     offsets_ = offsets;
     
     // Apply offsets to MPU6050
     mpu_.setXAccelOffset(offsets_.accel_x_offset);
     mpu_.setYAccelOffset(offsets_.accel_y_offset);
     mpu_.setZAccelOffset(offsets_.accel_z_offset);
     
     mpu_.setXGyroOffset(offsets_.gyro_x_offset);
     mpu_.setYGyroOffset(offsets_.gyro_y_offset);
     mpu_.setZGyroOffset(offsets_.gyro_z_offset);
     
     calibrated_ = true;
     return true;
 }
 
 // === DATA READING ===
 
 bool MPU6050Driver::read_raw(RawIMUData& data) {
     if (!initialized_) {
         set_error("MPU6050 not initialized");
         return false;
     }
     
     // Read accelerometer and gyroscope
     mpu_.getMotion6(&data.accel_x, &data.accel_y, &data.accel_z,
                     &data.gyro_x, &data.gyro_y, &data.gyro_z);
     
     // Read temperature
     data.temp = mpu_.getTemperature();
     
     return true;
 }
 
 bool MPU6050Driver::read_scaled(ScaledIMUData& data) {
     RawIMUData raw_data;
     if (!read_raw(raw_data)) {
         return false;
     }
     
     // Convert to scaled values
     data.accel_x = raw_accel_to_ms2(raw_data.accel_x);
     data.accel_y = raw_accel_to_ms2(raw_data.accel_y);
     data.accel_z = raw_accel_to_ms2(raw_data.accel_z);
     
     data.gyro_x = raw_gyro_to_dps(raw_data.gyro_x);
     data.gyro_y = raw_gyro_to_dps(raw_data.gyro_y);
     data.gyro_z = raw_gyro_to_dps(raw_data.gyro_z);
     
     // Convert temperature (formula from MPU6050 datasheet)
     data.temp = (raw_data.temp / 340.0f) + 36.53f;
     
     return true;
 }
 
 float MPU6050Driver::get_temperature() {
     if (!initialized_) {
         set_error("MPU6050 not initialized");
         return 0.0f;
     }
     
     int16_t temp_raw = mpu_.getTemperature();
     return (temp_raw / 340.0f) + 36.53f;
 }
 
 // === ORIENTATION CALCULATION ===
 
 bool MPU6050Driver::get_pitch_roll(EulerAngles& angles) {
     ScaledIMUData data;
     if (!read_scaled(data)) {
         return false;
     }
     
     angles.pitch = calculate_pitch(data.accel_x, data.accel_y, data.accel_z);
     angles.roll = calculate_roll(data.accel_x, data.accel_y, data.accel_z);
     angles.yaw = 0.0f;  // Cannot be determined from accelerometer alone
     
     return true;
 }
 
 bool MPU6050Driver::get_euler_angles(EulerAngles& angles, float dt) {
     ScaledIMUData data;
     if (!read_scaled(data)) {
         return false;
     }
     
     // Calculate angles from accelerometer
     float accel_pitch = calculate_pitch(data.accel_x, data.accel_y, data.accel_z);
     float accel_roll = calculate_roll(data.accel_x, data.accel_y, data.accel_z);
     
     // Apply complementary filter
     filtered_angles_.pitch = complementary_filter(accel_pitch, data.gyro_y, filtered_angles_.pitch, dt);
     filtered_angles_.roll = complementary_filter(accel_roll, data.gyro_x, filtered_angles_.roll, dt);
     
     // Integrate yaw (no accelerometer reference)
     filtered_angles_.yaw += data.gyro_z * dt;
     
     angles = filtered_angles_;
     return true;
 }
 
 // === DMP SUPPORT ===
 
 bool MPU6050Driver::init_dmp() {
     if (!initialized_) {
         set_error("MPU6050 not initialized");
         return false;
     }
     
     // Note: DMP initialization requires the MPU6050 DMP library
     // This is a placeholder implementation
     // In a real implementation, you would use Jeff Rowberg's DMP library
     
     set_error("DMP functionality requires additional library implementation");
     return false;
     
     // TODO: Implement DMP initialization
     // uint8_t devStatus = mpu_.dmpInitialize();
     // if (devStatus == 0) {
     //     mpu_.setDMPEnabled(true);
     //     dmp_enabled_ = true;
     //     return true;
     // }
     // return false;
 }
 
 bool MPU6050Driver::is_dmp_enabled() const {
     return dmp_enabled_;
 }
 
 bool MPU6050Driver::read_dmp_quaternion(Quaternion& quat) {
     if (!dmp_enabled_) {
         set_error("DMP not enabled");
         return false;
     }
     
     // Placeholder implementation
     set_error("DMP quaternion reading not yet implemented");
     return false;
     
     // TODO: Implement DMP quaternion reading
     // uint8_t fifoBuffer[64];
     // if (mpu_.dmpGetCurrentFIFOPacket(fifoBuffer)) {
     //     mpu_.dmpGetQuaternion(&quat, fifoBuffer);
     //     return true;
     // }
     // return false;
 }
 
 void MPU6050Driver::quaternion_to_euler(const Quaternion& quat, EulerAngles& angles) {
     // Convert quaternion to Euler angles
     float test = quat.x * quat.y + quat.z * quat.w;
     
     if (test > 0.499f) { // singularity at north pole
         angles.yaw = 2.0f * atan2(quat.x, quat.w) * RAD_TO_DEG;
         angles.pitch = 90.0f;
         angles.roll = 0.0f;
         return;
     }
     
     if (test < -0.499f) { // singularity at south pole
         angles.yaw = -2.0f * atan2(quat.x, quat.w) * RAD_TO_DEG;
         angles.pitch = -90.0f;
         angles.roll = 0.0f;
         return;
     }
     
     float sqx = quat.x * quat.x;
     float sqy = quat.y * quat.y;
     float sqz = quat.z * quat.z;
     
     angles.yaw = atan2(2.0f * quat.y * quat.w - 2.0f * quat.x * quat.z, 1.0f - 2.0f * sqy - 2.0f * sqz) * RAD_TO_DEG;
     angles.pitch = asin(2.0f * quat.x * quat.y + 2.0f * quat.z * quat.w) * RAD_TO_DEG;
     angles.roll = atan2(2.0f * quat.x * quat.w - 2.0f * quat.y * quat.z, 1.0f - 2.0f * sqx - 2.0f * sqz) * RAD_TO_DEG;
 }
 
 // === INTERRUPT SUPPORT ===
 
 bool MPU6050Driver::configure_motion_interrupt(uint8_t threshold, uint8_t duration) {
     if (!initialized_) { 
         set_error("MPU6050 not initialized");
         return false;
     }

     return true;
 }
 
 bool MPU6050Driver::is_motion_detected() {
     if (!initialized_) {
         set_error("MPU6050 not initialized");
         return false;
     }

     return true;
 }
 
 bool MPU6050Driver::clear_motion_interrupt() {
     if (!initialized_) {
         set_error("MPU6050 not initialized");
         return false;
     }

    }}