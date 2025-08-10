#ifndef SINGULAR_VALUES_H
#define SINGULAR_VALUES_H

#include <Arduino.h>
#include "../application_data_types/numeric_types.h"

// Value types for singular values
enum class SingularValueType {
    SYSTEM_ID,
    COMPONENT_ID,
    SESSION_ID,
    TRANSACTION_ID,
    EVENT_ID,
    MESSAGE_ID,
    FILE_ID,
    USER_ID,
    DEVICE_ID,
    NETWORK_ID
};

// Value status
enum class SingularValueStatus {
    AVAILABLE,
    IN_USE,
    RESERVED,
    EXPIRED,
    INVALID,
    LOCKED
};

// Value metadata
struct SingularValueMetadata {
    uint32_t value;
    SingularValueType type;
    SingularValueStatus status;
    String description;
    String owner;
    unsigned long creationTime;
    unsigned long lastUsedTime;
    unsigned long expirationTime;
    uint16_t useCount;
    bool persistent;
    bool encrypted;
};

// Value generation configuration
struct ValueGenerationConfig {
    uint32_t startValue;
    uint32_t endValue;
    uint32_t currentValue;
    uint16_t increment;
    bool circular;
    bool random;
    uint32_t seed;
    uint16_t maxRetries;
};

// Value validation rules
struct ValueValidationRule {
    String name;
    bool (*validator)(uint32_t value);
    String errorMessage;
    uint8_t priority;
    bool enabled;
};

// Singular values configuration
struct SingularValuesConfig {
    uint16_t maxValues;
    uint16_t maxTypes;
    uint16_t cleanupInterval;
    bool enablePersistence;
    bool enableValidation;
    bool enableLogging;
    uint16_t logInterval;
    uint32_t defaultExpiration;
};

class SingularValues {
private:
    static const uint16_t MAX_SINGULAR_VALUES = 1024;
    static const uint16_t MAX_VALUE_TYPES = 32;
    static const uint16_t MAX_VALIDATION_RULES = 64;
    
    // Value storage
    SingularValueMetadata values[MAX_SINGULAR_VALUES];
    uint16_t valueCount;
    
    // Type configurations
    ValueGenerationConfig typeConfigs[MAX_VALUE_TYPES];
    uint16_t typeCount;
    
    // Validation rules
    ValueValidationRule validationRules[MAX_VALIDATION_RULES];
    uint16_t ruleCount;
    
    // Configuration
    SingularValuesConfig config;
    
    // Timing and cleanup
    unsigned long lastCleanupTime;
    unsigned long lastLogTime;
    
    // Error tracking
    uint8_t errorFlags;
    String lastError;
    
    // Helper methods
    uint16_t findValueIndex(uint32_t value) const;
    uint16_t findTypeIndex(SingularValueType type) const;
    uint16_t findRuleIndex(const String& ruleName) const;
    bool validateValue(uint32_t value, SingularValueType type) const;
    uint32_t generateNextValue(SingularValueType type);
    bool isValueExpired(const SingularValueMetadata& metadata) const;
    void cleanupExpiredValues();
    bool persistValue(const SingularValueMetadata& metadata);
    bool loadPersistedValue(SingularValueMetadata& metadata);
    void logValueOperation(const String& operation, uint32_t value, const String& details);

public:
    SingularValues();
    ~SingularValues();
    
    // Initialization and configuration
    bool initialize(const SingularValuesConfig& config = SingularValuesConfig{});
    void setConfiguration(const SingularValuesConfig& newConfig);
    SingularValuesConfig getConfiguration() const { return config; }
    
    // Type management
    bool registerValueType(SingularValueType type, const String& description,
                          uint32_t startValue = 1, uint32_t endValue = 0xFFFFFFFF,
                          uint16_t increment = 1, bool circular = false);
    
    bool configureValueType(SingularValueType type, const ValueGenerationConfig& config);
    ValueGenerationConfig getValueTypeConfig(SingularValueType type) const;
    bool isValueTypeRegistered(SingularValueType type) const;
    
    // Value generation and management
    uint32_t generateValue(SingularValueType type, const String& description = "", 
                          const String& owner = "");
    uint32_t generateValue(SingularValueType type, uint32_t specificValue,
                          const String& description = "", const String& owner = "");
    
    bool reserveValue(uint32_t value, const String& owner);
    bool releaseValue(uint32_t value);
    bool lockValue(uint32_t value, const String& owner);
    bool unlockValue(uint32_t value);
    
    // Value information and status
    bool getValueMetadata(uint32_t value, SingularValueMetadata& metadata) const;
    bool isValueAvailable(uint32_t value) const;
    bool isValueInUse(uint32_t value) const;
    bool isValueExpired(uint32_t value) const;
    SingularValueStatus getValueStatus(uint32_t value) const;
    
    // Value ownership and tracking
    String getValueOwner(uint32_t value) const;
    bool transferValueOwnership(uint32_t value, const String& newOwner);
    uint16_t getValueUseCount(uint32_t value) const;
    unsigned long getValueCreationTime(uint32_t value) const;
    unsigned long getValueLastUsedTime(uint32_t value) const;
    
    // Value expiration management
    bool setValueExpiration(uint32_t value, unsigned long expirationTime);
    bool extendValueExpiration(uint32_t value, unsigned long additionalTime);
    bool removeValueExpiration(uint32_t value);
    uint16_t getExpiredValueCount() const;
    
    // Validation rules
    bool addValidationRule(const String& name, bool (*validator)(uint32_t value),
                          const String& errorMessage = "", uint8_t priority = 128);
    bool removeValidationRule(const String& name);
    bool enableValidationRule(const String& name, bool enable);
    bool isValidationRuleEnabled(const String& name) const;
    
    // Bulk operations
    uint32_t* generateMultipleValues(SingularValueType type, uint16_t count,
                                    const String& description = "", const String& owner = "");
    bool releaseMultipleValues(const uint32_t* values, uint16_t count);
    bool validateMultipleValues(const uint32_t* values, uint16_t count) const;
    
    // Value search and filtering
    bool findValuesByType(SingularValueType type, uint32_t* values, uint16_t& count) const;
    bool findValuesByOwner(const String& owner, uint32_t* values, uint16_t& count) const;
    bool findValuesByStatus(SingularValueStatus status, uint32_t* values, uint16_t& count) const;
    bool findExpiredValues(uint32_t* values, uint16_t& count) const;
    
    // Statistics and monitoring
    uint16_t getValueCount() const { return valueCount; }
    uint16_t getTypeCount() const { return typeCount; }
    uint16_t getRuleCount() const { return ruleCount; }
    uint16_t getAvailableValueCount(SingularValueType type) const;
    uint16_t getUsedValueCount(SingularValueType type) const;
    uint16_t getExpiredValueCount(SingularValueType type) const;
    
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
    void printValue(uint32_t value);
    void printTypeValues(SingularValueType type);
    void printValidationRules();
    void printConfiguration();
    void printStatistics();
    
    // Cleanup and maintenance
    bool cleanupExpiredValues();
    bool cleanupUnusedValues(uint16_t maxAge);
    bool defragmentValueSpace();
    bool validateAllValues();
    
    // Persistence
    bool saveAllValues();
    bool loadAllValues();
    bool exportValues(const String& filename);
    bool importValues(const String& filename);
};

#endif // SINGULAR_VALUES_H