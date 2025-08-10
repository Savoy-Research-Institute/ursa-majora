#ifndef SYSTEM_VALUES_H
#define SYSTEM_VALUES_H

#include <Arduino.h>
#include "../../software_decision/application_data_types/numeric_types.h"

// System value categories
enum class SystemValueCategory {
    FLIGHT_CONTROL,
    NAVIGATION,
    PROPULSION,
    COMMUNICATION,
    SENSORS,
    SAFETY,
    MAINTENANCE,
    CONFIGURATION
};

// System value types
enum class SystemValueType {
    BOOLEAN,
    INTEGER,
    FLOAT,
    STRING,
    ARRAY,
    STRUCT
};

// System value access levels
enum class SystemValueAccess {
    READ_ONLY,
    READ_WRITE,
    WRITE_ONLY,
    INTERNAL_ONLY
};

// System value metadata
struct SystemValueMetadata {
    String name;
    String description;
    SystemValueCategory category;
    SystemValueType type;
    SystemValueAccess access;
    bool persistent;
    bool encrypted;
    uint16_t updateInterval;
    unsigned long lastUpdate;
    bool valid;
    String units;
    float minValue;
    float maxValue;
    float defaultValue;
};

// System value entry
struct SystemValueEntry {
    SystemValueMetadata metadata;
    union {
        bool boolValue;
        int32_t intValue;
        float floatValue;
        String* stringValue;
        void* arrayValue;
        void* structValue;
    } data;
    bool isInitialized;
};

// System configuration
struct SystemValuesConfig {
    uint16_t maxValues;
    uint16_t updateInterval;
    bool enablePersistence;
    bool enableValidation;
    bool enableLogging;
    uint16_t logInterval;
};

class SystemValues {
private:
    static const uint16_t MAX_SYSTEM_VALUES = 256;
    static const uint16_t DEFAULT_UPDATE_INTERVAL = 100; // ms
    
    // System values storage
    SystemValueEntry values[MAX_SYSTEM_VALUES];
    uint16_t valueCount;
    
    // Configuration
    SystemValuesConfig config;
    
    // Timing
    unsigned long lastUpdateTime;
    unsigned long lastLogTime;
    
    // Error tracking
    uint8_t errorFlags;
    String lastError;
    
    // Helper methods
    uint16_t findValueIndex(const String& name) const;
    bool validateValue(const SystemValueEntry& entry, const void* newValue) const;
    void updateValueMetadata(SystemValueEntry& entry);
    bool persistValue(const SystemValueEntry& entry);
    bool loadPersistedValue(SystemValueEntry& entry);
    void logValueChange(const SystemValueEntry& entry, const String& oldValue, const String& newValue);

public:
    SystemValues();
    ~SystemValues();
    
    // Initialization and configuration
    bool initialize(const SystemValuesConfig& config = SystemValuesConfig{});
    void setConfiguration(const SystemValuesConfig& newConfig);
    SystemValuesConfig getConfiguration() const { return config; }
    
    // Value management
    bool registerValue(const String& name, const String& description, 
                      SystemValueCategory category, SystemValueType type,
                      SystemValueAccess access = SystemValueAccess::READ_WRITE,
                      bool persistent = false);
    
    bool setValue(const String& name, bool value);
    bool setValue(const String& name, int32_t value);
    bool setValue(const String& name, float value);
    bool setValue(const String& name, const String& value);
    bool setValue(const String& name, const void* arrayData, size_t size);
    bool setValue(const String& name, const void* structData, size_t size);
    
    bool getValue(const String& name, bool& value) const;
    bool getValue(const String& name, int32_t& value) const;
    bool getValue(const String& name, float& value) const;
    bool getValue(const String& name, String& value) const;
    bool getValue(const String& name, void* arrayData, size_t size) const;
    bool getValue(const String& name, void* structData, size_t size) const;
    
    // Value information
    bool getValueMetadata(const String& name, SystemValueMetadata& metadata) const;
    bool isValueRegistered(const String& name) const;
    bool isValueValid(const String& name) const;
    uint16_t getValueCount() const { return valueCount; }
    
    // Category operations
    bool getValuesByCategory(SystemValueCategory category, String* names, uint16_t& count) const;
    uint16_t getCategoryValueCount(SystemValueCategory category) const;
    
    // Access control
    bool setValueAccess(const String& name, SystemValueAccess access);
    SystemValueAccess getValueAccess(const String& name) const;
    
    // Persistence
    bool enableValuePersistence(const String& name, bool enable);
    bool isValuePersistent(const String& name) const;
    bool saveAllPersistentValues();
    bool loadAllPersistentValues();
    
    // Validation
    bool setValueRange(const String& name, float minValue, float maxValue);
    bool setValueDefault(const String& name, float defaultValue);
    bool validateAllValues();
    
    // System operations
    void update();
    void shutdown();
    bool performSelfTest();
    
    // Status and monitoring
    bool isHealthy() const;
    uint8_t getErrorFlags() const { return errorFlags; }
    String getLastError() const { return lastError; }
    void clearErrors();
    
    // Utility methods
    void printAllValues();
    void printValue(const String& name);
    void printCategoryValues(SystemValueCategory category);
    void printConfiguration();
    
    // Value search and filtering
    bool findValuesByPattern(const String& pattern, String* names, uint16_t& count) const;
    bool findValuesByType(SystemValueType type, String* names, uint16_t& count) const;
    
    // Bulk operations
    bool setMultipleValues(const String* names, const void** values, uint16_t count);
    bool getMultipleValues(const String* names, void** values, uint16_t count);
    
    // Value statistics
    uint16_t getCategoryCount(SystemValueCategory category) const;
    float getAverageUpdateRate() const;
    uint16_t getStaleValueCount(uint16_t maxAgeMs) const;
    
    // Cleanup
    bool removeValue(const String& name);
    void clearAllValues();
    bool cleanupInvalidValues();
};

#endif // SYSTEM_VALUES_H 