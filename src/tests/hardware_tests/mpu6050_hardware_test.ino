/**
 * @file mpu6050_hardware_test.ino
 * @brief Hardware test for MPU6050 IMU sensor
 * @author Velma Development Team
 * @version 1.0
 * @date 2025
 * 
 * @details
 * This file contains hardware tests for the MPU6050 6-axis IMU sensor.
 * Tests include sensor initialization, data reading, calibration,
 * and performance validation. This test requires actual hardware.
 */

#include <Wire.h>
#include <MPU6050.h>

// Hardware configuration
const int MPU_I2C_ADDRESS = 0x68;  // Default MPU6050 I2C address
const int MPU_TRANSISTOR_PIN = 8;   // Pin controlling MPU power
const int LED_PIN = 13;             // Built-in LED for status indication

// MPU6050 instance
MPU6050 mpu;

// Test state variables
bool is_mpu_on = false;
bool mpu_initialized = false;
bool calibration_complete = false;

// Calibration data
float acc_err_x = 0, acc_err_y = 0;
float gyro_err_x = 0, gyro_err_y = 0, gyro_err_z = 0;

// Test results
struct TestResults {
    bool connection_test = false;
    bool initialization_test = false;
    bool data_reading_test = false;
    bool calibration_test = false;
    bool performance_test = false;
    String error_message = "";
};

TestResults testResults;

// Test utilities
void blinkLED(int times, int delay_ms = 200) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(delay_ms);
        digitalWrite(LED_PIN, LOW);
        delay(delay_ms);
    }
}

void printTestResult(const char* testName, bool passed) {
    Serial.print(testName);
    Serial.print(": ");
    Serial.println(passed ? "PASS" : "FAIL");
    if (!passed) {
        blinkLED(3, 100);
    }
}

void printTestHeader(const char* testName) {
    Serial.println();
    Serial.println("==========================================");
    Serial.print("Testing: ");
    Serial.println(testName);
    Serial.println("==========================================");
}

// Hardware control functions
void switch_mpu_on() {
    if (!is_mpu_on) {
        digitalWrite(MPU_TRANSISTOR_PIN, HIGH);
        is_mpu_on = true;
        Serial.println("MPU6050 powered ON");
        delay(100); // Allow power stabilization
    }
}

void switch_mpu_off() {
    if (is_mpu_on) {
        digitalWrite(MPU_TRANSISTOR_PIN, LOW);
        is_mpu_on = false;
        Serial.println("MPU6050 powered OFF");
    }
}

void reset_mpu_thru_register() {
    Wire.beginTransmission(MPU_I2C_ADDRESS);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(0x00);  // Clear sleep bit
    Wire.endTransmission(true);
    Serial.println("MPU6050 reset via register");
}

void set_mpu_fullscale_range() {
    // Set accelerometer range to ±8g
    Wire.beginTransmission(MPU_I2C_ADDRESS);
    Wire.write(0x1C);  // ACCEL_CONFIG register
    Wire.write(0x10);  // ±8g range
    Wire.endTransmission(true);
    
    // Set gyroscope range to ±500°/s
    Wire.beginTransmission(MPU_I2C_ADDRESS);
    Wire.write(0x1B);  // GYRO_CONFIG register
    Wire.write(0x08);  // ±500°/s range
    Wire.endTransmission(true);
    
    Serial.println("MPU6050 full-scale ranges configured");
}

// Test functions
bool testMPUConnection() {
    printTestHeader("MPU6050 Connection Test");
    
    Wire.beginTransmission(MPU_I2C_ADDRESS);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
        Serial.println("✓ I2C communication successful");
        Serial.print("✓ MPU6050 found at address 0x");
        Serial.println(MPU_I2C_ADDRESS, HEX);
        testResults.connection_test = true;
        return true;
    } else {
        Serial.print("✗ I2C communication failed with error: ");
        Serial.println(error);
        testResults.error_message = "I2C communication failed";
        testResults.connection_test = false;
        return false;
    }
}

bool testMPUInitialization() {
    printTestHeader("MPU6050 Initialization Test");
    
    if (!testResults.connection_test) {
        Serial.println("✗ Skipping initialization test - connection failed");
        return false;
    }
    
    // Power on MPU
    switch_mpu_on();
    
    // Initialize Wire communication
    Wire.begin();
    
    // Reset MPU
    reset_mpu_thru_register();
    delay(100);
    
    // Configure full-scale ranges
    set_mpu_fullscale_range();
    delay(50);
    
    // Test MPU library initialization
    if (mpu.begin()) {
        Serial.println("✓ MPU6050 library initialization successful");
        
        // Configure MPU settings
        mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
        mpu.setGyroRange(MPU6050_RANGE_500_DEG);
        mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
        
        Serial.println("✓ MPU6050 configuration applied");
        testResults.initialization_test = true;
        mpu_initialized = true;
        return true;
    } else {
        Serial.println("✗ MPU6050 library initialization failed");
        testResults.error_message = "Library initialization failed";
        testResults.initialization_test = false;
        return false;
    }
}

bool testDataReading() {
    printTestHeader("MPU6050 Data Reading Test");
    
    if (!testResults.initialization_test) {
        Serial.println("✗ Skipping data reading test - initialization failed");
        return false;
    }
    
    Serial.println("Reading sensor data for 5 seconds...");
    
    int readings = 0;
    unsigned long startTime = millis();
    unsigned long endTime = startTime + 5000; // 5 seconds
    
    while (millis() < endTime) {
        if (mpu.dataAvailable()) {
            // Read accelerometer data
            int16_t accelX = mpu.getAccelX();
            int16_t accelY = mpu.getAccelY();
            int16_t accelZ = mpu.getAccelZ();
            
            // Read gyroscope data
            int16_t gyroX = mpu.getGyroX();
            int16_t gyroY = mpu.getGyroY();
            int16_t gyroZ = mpu.getGyroZ();
            
            // Read temperature
            float temperature = mpu.getTemperature();
            
            // Print data every 500ms
            if (readings % 25 == 0) {
                Serial.println("--- Sensor Data ---");
                Serial.print("Accel (raw): X=");
                Serial.print(accelX);
                Serial.print(" Y=");
                Serial.print(accelY);
                Serial.print(" Z=");
                Serial.println(accelZ);
                
                Serial.print("Gyro (raw): X=");
                Serial.print(gyroX);
                Serial.print(" Y=");
                Serial.print(gyroY);
                Serial.print(" Z=");
                Serial.println(gyroZ);
                
                Serial.print("Temperature: ");
                Serial.print(temperature);
                Serial.println(" °C");
                Serial.println();
            }
            
            readings++;
        }
        delay(20); // 50Hz reading rate
    }
    
    if (readings > 0) {
        Serial.print("✓ Successfully read ");
        Serial.print(readings);
        Serial.println(" sensor readings");
        Serial.print("✓ Average reading rate: ");
        Serial.print(readings / 5.0);
        Serial.println(" Hz");
        testResults.data_reading_test = true;
        return true;
    } else {
        Serial.println("✗ No sensor data received");
        testResults.error_message = "No sensor data received";
        testResults.data_reading_test = false;
        return false;
    }
}

bool testCalibration() {
    printTestHeader("MPU6050 Calibration Test");
    
    if (!testResults.data_reading_test) {
        Serial.println("✗ Skipping calibration test - data reading failed");
        return false;
    }
    
    Serial.println("Starting calibration process...");
    Serial.println("Place MPU6050 on a flat, stable surface");
    Serial.println("Calibration will take approximately 10 seconds");
    
    // Wait for user to position sensor
    Serial.println("Press any key to start calibration...");
    while (!Serial.available()) {
        delay(100);
    }
    Serial.read(); // Clear the input
    
    Serial.println("Calibrating...");
    
    // Calibration parameters
    const int CALIBRATION_SAMPLES = 1000;
    const int CALIBRATION_DELAY = 10;
    
    // Reset error accumulators
    acc_err_x = 0;
    acc_err_y = 0;
    gyro_err_x = 0;
    gyro_err_y = 0;
    gyro_err_z = 0;
    
    // Collect calibration samples
    for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
        if (mpu.dataAvailable()) {
            // Read accelerometer data
            int16_t accelX = mpu.getAccelX();
            int16_t accelY = mpu.getAccelY();
            int16_t accelZ = mpu.getAccelZ();
            
            // Read gyroscope data
            int16_t gyroX = mpu.getGyroX();
            int16_t gyroY = mpu.getGyroY();
            int16_t gyroZ = mpu.getGyroZ();
            
            // Calculate accelerometer errors (assuming flat surface)
            float accel_x_g = accelX / 4096.0; // ±8g range
            float accel_y_g = accelY / 4096.0;
            float accel_z_g = accelZ / 4096.0;
            
            // Calculate roll and pitch from accelerometer
            float roll = atan2(accel_y_g, sqrt(accel_x_g * accel_x_g + accel_z_g * accel_z_g)) * 180 / PI;
            float pitch = atan2(-accel_x_g, sqrt(accel_y_g * accel_y_g + accel_z_g * accel_z_g)) * 180 / PI;
            
            // Accumulate errors
            acc_err_x += roll;
            acc_err_y += pitch;
            
            // Accumulate gyroscope errors
            gyro_err_x += gyroX / 65.5; // ±500°/s range
            gyro_err_y += gyroY / 65.5;
            gyro_err_z += gyroZ / 65.5;
        }
        delay(CALIBRATION_DELAY);
        
        // Progress indicator
        if (i % 100 == 0) {
            Serial.print(".");
        }
    }
    
    // Calculate average errors
    acc_err_x /= CALIBRATION_SAMPLES;
    acc_err_y /= CALIBRATION_SAMPLES;
    gyro_err_x /= CALIBRATION_SAMPLES;
    gyro_err_y /= CALIBRATION_SAMPLES;
    gyro_err_z /= CALIBRATION_SAMPLES;
    
    Serial.println();
    Serial.println("Calibration complete!");
    Serial.println("--- Calibration Results ---");
    Serial.print("Accelerometer Roll Error: ");
    Serial.print(acc_err_x, 2);
    Serial.println(" degrees");
    Serial.print("Accelerometer Pitch Error: ");
    Serial.print(acc_err_y, 2);
    Serial.println(" degrees");
    Serial.print("Gyroscope X Error: ");
    Serial.print(gyro_err_x, 2);
    Serial.println(" degrees/s");
    Serial.print("Gyroscope Y Error: ");
    Serial.print(gyro_err_y, 2);
    Serial.println(" degrees/s");
    Serial.print("Gyroscope Z Error: ");
    Serial.print(gyro_err_z, 2);
    Serial.println(" degrees/s");
    
    // Validate calibration results
    bool calibration_valid = true;
    if (abs(acc_err_x) > 5.0 || abs(acc_err_y) > 5.0) {
        Serial.println("⚠ Warning: High accelerometer error - check sensor positioning");
        calibration_valid = false;
    }
    if (abs(gyro_err_x) > 10.0 || abs(gyro_err_y) > 10.0 || abs(gyro_err_z) > 10.0) {
        Serial.println("⚠ Warning: High gyroscope error - check for vibration");
        calibration_valid = false;
    }
    
    if (calibration_valid) {
        Serial.println("✓ Calibration within acceptable limits");
        testResults.calibration_test = true;
        calibration_complete = true;
        return true;
    } else {
        Serial.println("✗ Calibration outside acceptable limits");
        testResults.error_message = "Calibration failed";
        testResults.calibration_test = false;
        return false;
    }
}

bool testPerformance() {
    printTestHeader("MPU6050 Performance Test");
    
    if (!testResults.calibration_test) {
        Serial.println("✗ Skipping performance test - calibration failed");
        return false;
    }
    
    Serial.println("Testing sensor performance...");
    
    // Test reading frequency
    const int PERFORMANCE_SAMPLES = 1000;
    unsigned long startTime = micros();
    
    for (int i = 0; i < PERFORMANCE_SAMPLES; i++) {
        if (mpu.dataAvailable()) {
            mpu.getAccelX();
            mpu.getAccelY();
            mpu.getAccelZ();
            mpu.getGyroX();
            mpu.getGyroY();
            mpu.getGyroZ();
        }
    }
    
    unsigned long endTime = micros();
    unsigned long totalTime = endTime - startTime;
    float avgTimePerReading = (float)totalTime / PERFORMANCE_SAMPLES;
    float maxReadingRate = 1000000.0 / avgTimePerReading;
    
    Serial.print("✓ Performance test completed: ");
    Serial.print(PERFORMANCE_SAMPLES);
    Serial.println(" readings");
    Serial.print("✓ Average time per reading: ");
    Serial.print(avgTimePerReading, 2);
    Serial.println(" microseconds");
    Serial.print("✓ Maximum reading rate: ");
    Serial.print(maxReadingRate, 1);
    Serial.println(" Hz");
    
    // Validate performance
    if (maxReadingRate >= 50.0) { // Minimum 50Hz requirement
        Serial.println("✓ Reading rate meets requirements");
        testResults.performance_test = true;
        return true;
    } else {
        Serial.println("✗ Reading rate below requirements");
        testResults.error_message = "Performance below requirements";
        testResults.performance_test = false;
        return false;
    }
}

void printTestSummary() {
    Serial.println();
    Serial.println("==========================================");
    Serial.println("MPU6050 HARDWARE TEST SUMMARY");
    Serial.println("==========================================");
    
    printTestResult("Connection Test", testResults.connection_test);
    printTestResult("Initialization Test", testResults.initialization_test);
    printTestResult("Data Reading Test", testResults.data_reading_test);
    printTestResult("Calibration Test", testResults.calibration_test);
    printTestResult("Performance Test", testResults.performance_test);
    
    Serial.println();
    if (testResults.error_message.length() > 0) {
        Serial.print("Error: ");
        Serial.println(testResults.error_message);
    }
    
    // Overall result
    int passedTests = 0;
    if (testResults.connection_test) passedTests++;
    if (testResults.initialization_test) passedTests++;
    if (testResults.data_reading_test) passedTests++;
    if (testResults.calibration_test) passedTests++;
    if (testResults.performance_test) passedTests++;
    
    Serial.print("Overall Result: ");
    Serial.print(passedTests);
    Serial.print("/5 tests passed - ");
    
    if (passedTests == 5) {
        Serial.println("ALL TESTS PASSED ✓");
        blinkLED(5, 200);
    } else if (passedTests >= 3) {
        Serial.println("MOST TESTS PASSED ⚠");
        blinkLED(3, 300);
    } else {
        Serial.println("MULTIPLE TESTS FAILED ✗");
        blinkLED(10, 100);
    }
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("MPU6050 Hardware Test Suite");
    Serial.println("============================");
    Serial.println("This test requires actual MPU6050 hardware");
    Serial.println("Ensure proper connections:");
    Serial.println("- VCC to 3.3V");
    Serial.println("- GND to GND");
    Serial.println("- SDA to Arduino pin 20");
    Serial.println("- SCL to Arduino pin 21");
    Serial.println();
    
    // Initialize pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(MPU_TRANSISTOR_PIN, OUTPUT);
    
    // Start with MPU off
    switch_mpu_off();
    
    // Run all tests
    testMPUConnection();
    testMPUInitialization();
    testDataReading();
    testCalibration();
    testPerformance();
    
    // Print final summary
    printTestSummary();
}

void loop() {
    // Tests run once in setup
    // Keep loop empty for hardware testing
    
    // Optional: Continuous monitoring mode
    if (calibration_complete && mpu_initialized) {
        // Monitor sensor data continuously
        if (mpu.dataAvailable()) {
            // Read and display calibrated data
            int16_t accelX = mpu.getAccelX();
            int16_t accelY = mpu.getAccelY();
            int16_t accelZ = mpu.getAccelZ();
            
            // Apply calibration offsets
            float roll = atan2((accelY / 4096.0), sqrt(pow((accelX / 4096.0), 2) + pow((accelZ / 4096.0), 2))) * 180 / PI - acc_err_x;
            float pitch = atan2(-(accelX / 4096.0), sqrt(pow((accelY / 4096.0), 2) + pow((accelZ / 4096.0), 2))) * 180 / PI - acc_err_y;
            
            // Print calibrated orientation every 2 seconds
            static unsigned long lastPrint = 0;
            if (millis() - lastPrint > 2000) {
                Serial.print("Calibrated Orientation - Roll: ");
                Serial.print(roll, 2);
                Serial.print("°, Pitch: ");
                Serial.print(pitch, 2);
                Serial.println("°");
                lastPrint = millis();
            }
        }
    }
    
    delay(20); // 50Hz update rate
}