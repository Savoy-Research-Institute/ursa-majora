#ifndef NUMERIC_TYPES_H
#define NUMERIC_TYPES_H

#include <Arduino.h>
#include <math.h>

// Precision levels for floating point operations
enum class Precision {
    LOW = 3,      // 3 decimal places
    MEDIUM = 6,   // 6 decimal places
    HIGH = 9,     // 9 decimal places
    EXTREME = 12  // 12 decimal places
};

// Angle units
enum class AngleUnit {
    RADIANS,
    DEGREES,
    GRADIANS
};

// Temperature units
enum class TemperatureUnit {
    CELSIUS,
    FAHRENHEIT,
    KELVIN
};

// Pressure units
enum class PressureUnit {
    PASCALS,
    KILOPASCALS,
    BAR,
    PSI,
    ATMOSPHERES
};

// Distance units
enum class DistanceUnit {
    METERS,
    KILOMETERS,
    FEET,
    MILES,
    NAUTICAL_MILES
};

// Speed units
enum class SpeedUnit {
    METERS_PER_SECOND,
    KILOMETERS_PER_HOUR,
    KNOTS,
    MILES_PER_HOUR,
    FEET_PER_SECOND
};

// 3D Vector structure
struct Vector3D {
    float x, y, z;
    
    Vector3D() : x(0.0), y(0.0), z(0.0) {}
    Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}
    
    // Vector operations
    float magnitude() const;
    Vector3D normalize() const;
    float dot(const Vector3D& other) const;
    Vector3D cross(const Vector3D& other) const;
    
    // Arithmetic operations
    Vector3D operator+(const Vector3D& other) const;
    Vector3D operator-(const Vector3D& other) const;
    Vector3D operator*(float scalar) const;
    Vector3D operator/(float scalar) const;
    
    // Assignment operations
    Vector3D& operator+=(const Vector3D& other);
    Vector3D& operator-=(const Vector3D& other);
    Vector3D& operator*=(float scalar);
    Vector3D& operator/=(float scalar);
    
    // Comparison operations
    bool operator==(const Vector3D& other) const;
    bool operator!=(const Vector3D& other) const;
};

// Quaternion structure for 3D rotations
struct Quaternion {
    float w, x, y, z;
    
    Quaternion() : w(1.0), x(0.0), y(0.0), z(0.0) {}
    Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}
    
    // Quaternion operations
    float magnitude() const;
    Quaternion normalize() const;
    Quaternion conjugate() const;
    Quaternion inverse() const;
    
    // Rotation operations
    Quaternion operator*(const Quaternion& other) const;
    Vector3D rotateVector(const Vector3D& vector) const;
    
    // Utility functions
    void fromEulerAngles(float roll, float pitch, float yaw);
    void toEulerAngles(float& roll, float& pitch, float& yaw) const;
};

// Matrix3x3 structure for 3D transformations
struct Matrix3x3 {
    float m[3][3];
    
    Matrix3x3();
    Matrix3x3(float m00, float m01, float m02,
               float m10, float m11, float m12,
               float m20, float m21, float m22);
    
    // Matrix operations
    Matrix3x3 transpose() const;
    float determinant() const;
    Matrix3x3 inverse() const;
    
    // Arithmetic operations
    Matrix3x3 operator+(const Matrix3x3& other) const;
    Matrix3x3 operator-(const Matrix3x3& other) const;
    Matrix3x3 operator*(const Matrix3x3& other) const;
    Vector3D operator*(const Vector3D& vector) const;
    
    // Utility functions
    void setIdentity();
    void setRotation(float roll, float pitch, float yaw);
    void setRotation(const Quaternion& q);
};

// Statistical data structure
struct Statistics {
    uint32_t count;
    float sum;
    float sumSquares;
    float min;
    float max;
    float mean;
    float variance;
    float standardDeviation;
    
    Statistics();
    
    // Statistical operations
    void addValue(float value);
    void removeValue(float value);
    void clear();
    void calculate();
    
    // Utility functions
    bool isValid() const;
    float getRange() const;
    float getCoefficientOfVariation() const;
};

// Filter structures
struct LowPassFilter {
    float alpha;
    float filteredValue;
    bool initialized;
    
    LowPassFilter(float alpha = 0.1);
    
    float filter(float input);
    void reset();
    void setAlpha(float alpha);
};

struct MovingAverageFilter {
    static const uint8_t MAX_SAMPLES = 32;
    float samples[MAX_SAMPLES];
    uint8_t sampleCount;
    uint8_t sampleIndex;
    float sum;
    
    MovingAverageFilter(uint8_t maxSamples = 16);
    
    float filter(float input);
    void reset();
    void setSampleCount(uint8_t count);
};

// Kalman filter structure
struct KalmanFilter {
    float Q;  // Process noise
    float R;  // Measurement noise
    float P;  // Estimation error covariance
    float X;  // State estimate
    float K;  // Kalman gain
    
    KalmanFilter(float Q = 0.1, float R = 1.0);
    
    float filter(float measurement);
    void reset();
    void setNoise(float Q, float R);
};

class NumericTypes {
private:
    // Configuration
    Precision defaultPrecision;
    bool enableErrorChecking;
    bool enableOverflowProtection;
    
    // Error tracking
    uint32_t errorCount;
    String lastError;
    
    // Performance metrics
    uint32_t operationCount;
    uint32_t errorOperationCount;
    
public:
    NumericTypes();
    
    // Configuration
    void setDefaultPrecision(Precision precision);
    void enableErrorChecking(bool enabled);
    void enableOverflowProtection(bool enabled);
    
    // Unit conversions
    float convertAngle(float value, AngleUnit from, AngleUnit to);
    float convertTemperature(float value, TemperatureUnit from, TemperatureUnit to);
    float convertPressure(float value, PressureUnit from, PressureUnit to);
    float convertDistance(float value, DistanceUnit from, DistanceUnit to);
    float convertSpeed(float value, SpeedUnit from, SpeedUnit to);
    
    // Mathematical utilities
    float roundToPrecision(float value, Precision precision = Precision::MEDIUM);
    float clamp(float value, float min, float max);
    float lerp(float a, float b, float t);
    float mapRange(float value, float fromMin, float fromMax, float toMin, float toMax);
    
    // Trigonometric functions with unit conversion
    float sin(float angle, AngleUnit unit = AngleUnit::RADIANS);
    float cos(float angle, AngleUnit unit = AngleUnit::RADIANS);
    float tan(float angle, AngleUnit unit = AngleUnit::RADIANS);
    float asin(float value, AngleUnit unit = AngleUnit::RADIANS);
    float acos(float value, AngleUnit unit = AngleUnit::RADIANS);
    float atan(float value, AngleUnit unit = AngleUnit::RADIANS);
    float atan2(float y, float x, AngleUnit unit = AngleUnit::RADIANS);
    
    // Advanced mathematical functions
    float hypot(float x, float y);
    float hypot3(float x, float y, float z);
    float normalizeAngle(float angle, AngleUnit unit = AngleUnit::RADIANS);
    float angleDifference(float angle1, float angle2, AngleUnit unit = AngleUnit::RADIANS);
    
    // Error handling
    uint32_t getErrorCount() const { return errorCount; }
    String getLastError() const { return lastError; }
    void clearErrors();
    
    // Performance monitoring
    uint32_t getOperationCount() const { return operationCount; }
    uint32_t getErrorOperationCount() const { return errorOperationCount; }
    float getErrorRate() const;
    
    // System interface
    bool initialize();
    void update();
    void shutdown();
    
    // Diagnostic functions
    void runSelfTest();
    void printDiagnostics();
    
    // Utility functions
    bool isFinite(float value) const;
    bool isNaN(float value) const;
    bool isInfinity(float value) const;
    float getSmallestPositive() const;
    float getLargestFinite() const;
    
    // Testing functions
    void testUnitConversions();
    void testMathematicalFunctions();
    void testVectorOperations();
    void testQuaternionOperations();
    void testMatrixOperations();
    void testFilterOperations();
};

// Global numeric types instance
extern NumericTypes numericTypes;

#endif // NUMERIC_TYPES_H 