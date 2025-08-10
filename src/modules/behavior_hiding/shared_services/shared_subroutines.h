#ifndef SHARED_SUBROUTINES_H
#define SHARED_SUBROUTINES_H

#include <Arduino.h>
#include "../../software_decision/application_data_types/numeric_types.h"

// Mathematical constants
#define PI 3.14159265359f
#define TWO_PI 6.28318530718f
#define HALF_PI 1.57079632679f
#define DEG_TO_RAD 0.01745329252f
#define RAD_TO_DEG 57.2957795131f

// Utility function categories
enum class UtilityCategory {
    MATHEMATICAL,
    CONVERSION,
    VALIDATION,
    ARRAY_OPERATIONS,
    STRING_OPERATIONS,
    TIME_OPERATIONS,
    MEMORY_OPERATIONS,
    CRYPTOGRAPHIC
};

// Error codes for utility functions
enum class UtilityError {
    SUCCESS,
    INVALID_PARAMETER,
    BUFFER_OVERFLOW,
    INSUFFICIENT_MEMORY,
    TIMEOUT,
    NOT_IMPLEMENTED,
    UNKNOWN_ERROR
};

// Mathematical utility functions
class MathUtils {
public:
    // Basic math operations
    static float clamp(float value, float min, float max);
    static int32_t clamp(int32_t value, int32_t min, int32_t max);
    static float map(float value, float fromLow, float fromHigh, float toLow, float toHigh);
    static float constrain(float value, float min, float max);
    
    // Trigonometric functions
    static float sinDeg(float degrees);
    static float cosDeg(float degrees);
    static float tanDeg(float degrees);
    static float asinDeg(float value);
    static float acosDeg(float value);
    static float atanDeg(float value);
    static float atan2Deg(float y, float x);
    
    // Statistical functions
    static float average(const float* values, uint16_t count);
    static float variance(const float* values, uint16_t count);
    static float standardDeviation(const float* values, uint16_t count);
    static float min(const float* values, uint16_t count);
    static float max(const float* values, uint16_t count);
    
    // Filtering functions
    static float lowPassFilter(float input, float previous, float alpha);
    static float highPassFilter(float input, float previous, float alpha);
    static float movingAverage(float input, float* buffer, uint16_t bufferSize, uint16_t& index);
    
    // Interpolation functions
    static float linearInterpolate(float x1, float y1, float x2, float y2, float x);
    static float bilinearInterpolate(float x, float y, float q11, float q12, float q21, float q22);
    
    // Rounding and precision
    static float roundToDecimal(float value, uint8_t decimals);
    static bool isApproximatelyEqual(float a, float b, float tolerance);
    static float normalizeAngle(float angle);
};

// Conversion utility functions
class ConversionUtils {
public:
    // Unit conversions
    static float feetToMeters(float feet);
    static float metersToFeet(float meters);
    static float knotsToMph(float knots);
    static float mphToKnots(float mph);
    static float celsiusToFahrenheit(float celsius);
    static float fahrenheitToCelsius(float fahrenheit);
    static float psiToPascals(float psi);
    static float pascalsToPsi(float pascals);
    
    // Coordinate conversions
    static void latLonToNED(float lat, float lon, float& north, float& east, float& down);
    static void NEDToLatLon(float north, float east, float down, float& lat, float& lon);
    static void eulerToQuaternion(float roll, float pitch, float yaw, float* quaternion);
    static void quaternionToEuler(const float* quaternion, float& roll, float& pitch, float& yaw);
    
    // Data type conversions
    static uint16_t floatToUint16(float value, float scale);
    static float uint16ToFloat(uint16_t value, float scale);
    static uint32_t floatToUint32(float value, float scale);
    static float uint32ToFloat(uint32_t value, float scale);
};

// Validation utility functions
class ValidationUtils {
public:
    // Range validation
    static bool isInRange(float value, float min, float max);
    static bool isInRange(int32_t value, int32_t min, int32_t max);
    static bool isInRange(uint32_t value, uint32_t min, uint32_t max);
    
    // Type validation
    static bool isValidFloat(float value);
    static bool isValidInteger(int32_t value);
    static bool isValidString(const String& str);
    static bool isValidArray(const void* array, size_t size);
    
    // Format validation
    static bool isValidEmail(const String& email);
    static bool isValidIPAddress(const String& ip);
    static bool isValidMACAddress(const String& mac);
    static bool isValidCoordinate(float lat, float lon);
    
    // Checksum validation
    static uint8_t calculateChecksum(const uint8_t* data, size_t length);
    static uint16_t calculateCRC16(const uint8_t* data, size_t length);
    static uint32_t calculateCRC32(const uint8_t* data, size_t length);
    static bool verifyChecksum(const uint8_t* data, size_t length, uint8_t expected);
};

// Array utility functions
class ArrayUtils {
public:
    // Array operations
    static void copyArray(const void* source, void* destination, size_t size);
    static void fillArray(void* array, size_t size, uint8_t value);
    static void reverseArray(void* array, size_t size);
    static void shuffleArray(void* array, size_t size);
    
    // Array searching
    static int16_t findElement(const void* array, size_t size, const void* element);
    static int16_t findElement(const float* array, size_t size, float element);
    static int16_t findElement(const int32_t* array, size_t size, int32_t element);
    
    // Array sorting
    static void sortArray(float* array, size_t size, bool ascending = true);
    static void sortArray(int32_t* array, size_t size, bool ascending = true);
    static void sortArray(uint32_t* array, size_t size, bool ascending = true);
    
    // Array statistics
    static float arraySum(const float* array, size_t size);
    static float arrayProduct(const float* array, size_t size);
    static float arrayMean(const float* array, size_t size);
    static float arrayMedian(const float* array, size_t size);
};

// String utility functions
class StringUtils {
public:
    // String operations
    static String trim(const String& str);
    static String toUpperCase(const String& str);
    static String toLowerCase(const String& str);
    static String replace(const String& str, const String& find, const String& replace);
    
    // String parsing
    static bool parseFloat(const String& str, float& result);
    static bool parseInt(const String& str, int32_t& result);
    static bool parseUint(const String& str, uint32_t& result);
    static bool parseBool(const String& str, bool& result);
    
    // String formatting
    static String formatFloat(float value, uint8_t decimals);
    static String formatInteger(int32_t value, uint8_t base = 10);
    static String formatHex(uint32_t value, uint8_t digits = 8);
    static String formatBinary(uint32_t value, uint8_t digits = 32);
    
    // String validation
    static bool startsWith(const String& str, const String& prefix);
    static bool endsWith(const String& str, const String& suffix);
    static bool contains(const String& str, const String& substring);
    static uint16_t countOccurrences(const String& str, char character);
};

// Time utility functions
class TimeUtils {
public:
    // Time operations
    static uint32_t getElapsedTime(uint32_t startTime);
    static bool isTimeout(uint32_t startTime, uint32_t timeout);
    static uint32_t getRemainingTime(uint32_t startTime, uint32_t timeout);
    
    // Time formatting
    static String formatTime(uint32_t milliseconds);
    static String formatDuration(uint32_t seconds);
    static String formatTimestamp(uint32_t timestamp);
    
    // Time validation
    static bool isValidTime(uint8_t hour, uint8_t minute, uint8_t second);
    static bool isValidDate(uint8_t day, uint8_t month, uint16_t year);
    static bool isLeapYear(uint16_t year);
    
    // Time calculations
    static uint8_t getDaysInMonth(uint8_t month, uint16_t year);
    static uint16_t getDayOfYear(uint8_t day, uint8_t month, uint16_t year);
    static uint8_t getDayOfWeek(uint8_t day, uint8_t month, uint16_t year);
};

// Memory utility functions
class MemoryUtils {
public:
    // Memory operations
    static void* allocateMemory(size_t size);
    static void freeMemory(void* ptr);
    static void clearMemory(void* ptr, size_t size);
    static void copyMemory(void* destination, const void* source, size_t size);
    
    // Memory validation
    static bool isValidPointer(const void* ptr);
    static size_t getAvailableMemory();
    static size_t getTotalMemory();
    static float getMemoryUsagePercent();
    
    // Memory optimization
    static void defragmentMemory();
    static void compactMemory();
    static void optimizeMemory();
};

// Cryptographic utility functions
class CryptoUtils {
public:
    // Hash functions
    static uint32_t simpleHash(const String& input);
    static uint32_t simpleHash(const uint8_t* data, size_t length);
    static String md5Hash(const String& input);
    static String sha1Hash(const String& input);
    
    // Encryption/Decryption
    static String simpleEncrypt(const String& input, const String& key);
    static String simpleDecrypt(const String& input, const String& key);
    static void xorEncrypt(uint8_t* data, size_t length, const uint8_t* key, size_t keyLength);
    
    // Random number generation
    static uint32_t generateRandomNumber();
    static uint32_t generateRandomNumber(uint32_t min, uint32_t max);
    static void generateRandomBytes(uint8_t* buffer, size_t length);
};

// Main utility class that provides access to all utilities
class SharedSubroutines {
private:
    static MathUtils mathUtils;
    static ConversionUtils conversionUtils;
    static ValidationUtils validationUtils;
    static ArrayUtils arrayUtils;
    static StringUtils stringUtils;
    static TimeUtils timeUtils;
    static MemoryUtils memoryUtils;
    static CryptoUtils cryptoUtils;
    
    // Error tracking
    static UtilityError lastError;
    static String lastErrorMessage;
    
public:
    // Utility access
    static MathUtils& math() { return mathUtils; }
    static ConversionUtils& conversion() { return conversionUtils; }
    static ValidationUtils& validation() { return validationUtils; }
    static ArrayUtils& array() { return arrayUtils; }
    static StringUtils& string() { return stringUtils; }
    static TimeUtils& time() { return timeUtils; }
    static MemoryUtils& memory() { return memoryUtils; }
    static CryptoUtils& crypto() { return cryptoUtils; }
    
    // Error handling
    static UtilityError getLastError() { return lastError; }
    static String getLastErrorMessage() { return lastErrorMessage; }
    static void clearErrors();
    
    // System utilities
    static void initialize();
    static void shutdown();
    static bool performSelfTest();
    static bool isHealthy();
    
    // Utility information
    static String getVersion();
    static String getBuildInfo();
    static void printStatus();
};

#endif // SHARED_SUBROUTINES_H 