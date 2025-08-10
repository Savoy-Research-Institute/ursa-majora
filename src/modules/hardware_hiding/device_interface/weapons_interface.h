#ifndef WEAPONS_INTERFACE_H
#define WEAPONS_INTERFACE_H

#include <Arduino.h>

// Weapons system types
enum class WeaponsSystemType {
    AIR_TO_AIR_MISSILE,
    AIR_TO_GROUND_MISSILE,
    BOMB,
    ROCKET,
    GUN,
    ELECTRONIC_COUNTERMEASURES,
    CHAFF_FLARE
};

// Weapons status
enum class WeaponsStatus {
    SAFE,
    ARMED,
    READY,
    FIRING,
    RELOADING,
    MAINTENANCE,
    ERROR
};

// Target information
struct WeaponsTarget {
    uint16_t range;          // Range in meters
    uint16_t bearing;        // Bearing in degrees (0-359)
    int16_t velocity;        // Velocity in m/s
    uint8_t priority;        // Target priority (0-255, higher = more important)
    uint8_t targetType;      // Target classification
    bool locked;             // Whether target is locked
    bool valid;              // Whether target data is valid
    unsigned long timestamp; // When target was detected
};

// Weapons configuration
struct WeaponsConfig {
    WeaponsSystemType type;
    uint8_t quantity;        // Number of weapons available
    uint8_t maxQuantity;     // Maximum capacity
    uint16_t range;          // Effective range in meters
    uint8_t accuracy;        // Accuracy percentage (0-100)
    bool safetyEnabled;      // Safety interlocks enabled
    uint8_t armingDelay;     // Arming delay in milliseconds
    bool autoTargeting;      // Automatic targeting enabled
};

// Weapons system status
struct WeaponsSystemStatus {
    WeaponsStatus status;
    uint8_t quantityLoaded;  // Currently loaded weapons
    uint8_t quantityReady;   // Weapons ready to fire
    bool targetLocked;       // Whether a target is locked
    bool safetyEngaged;      // Safety system status
    uint8_t errorFlags;      // Error status flags
    uint16_t uptime;         // System uptime in seconds
    bool healthy;            // Overall system health
};

class WeaponsInterface {
private:
    static const uint8_t MAX_TARGETS = 16;
    static const uint8_t MAX_WEAPONS = 8;
    
    WeaponsConfig config;
    WeaponsSystemStatus status;
    
    // Target tracking
    WeaponsTarget targets[MAX_TARGETS];
    uint8_t targetCount;
    uint8_t lockedTargetIndex;
    
    // Hardware control
    uint8_t safetyPin;       // Safety interlock pin
    uint8_t armingPin;       // Arming control pin
    uint8_t firePin;         // Fire control pin
    uint8_t statusPin;       // Status input pin
    uint8_t quantityPin;     // Quantity sensor pin
    
    // Targeting system
    bool targetTracking;
    uint16_t lastRange;
    uint16_t lastBearing;
    int16_t lastVelocity;
    
    // Timing and intervals
    unsigned long lastUpdateTime;
    unsigned long lastTargetUpdate;
    unsigned long armingStartTime;
    
    // Error handling
    uint8_t errorFlags;
    String lastError;
    
    // Private methods
    bool initializeHardware();
    bool engageSafety();
    bool disengageSafety();
    bool armWeapon();
    bool disarmWeapon();
    bool fireWeapon();
    bool validateTarget(const WeaponsTarget& target);
    void updateTargets();
    void clearTargets();
    bool performTargetLock(uint8_t targetIndex);
    void releaseTargetLock();
    
public:
    WeaponsInterface();
    
    // Configuration
    bool configure(const WeaponsConfig& newConfig);
    WeaponsConfig getConfiguration() const { return config; }
    bool setWeaponsType(WeaponsSystemType type);
    bool setQuantity(uint8_t quantity);
    bool setRange(uint16_t range);
    bool setAccuracy(uint8_t accuracy);
    bool setSafetyEnabled(bool enabled);
    bool setArmingDelay(uint8_t delay);
    bool setAutoTargeting(bool enabled);
    
    // Safety and arming
    bool enableSafety();
    bool disableSafety();
    bool isSafetyEnabled() const { return config.safetyEnabled; }
    bool armSystem();
    bool disarmSystem();
    bool isArmed() const { return status.status == WeaponsStatus::ARMED; }
    
    // Target management
    uint8_t getTargetCount() const { return targetCount; }
    bool getTarget(uint8_t index, WeaponsTarget& target) const;
    bool getLockedTarget(WeaponsTarget& target) const;
    bool lockTarget(uint8_t targetIndex);
    bool releaseTarget();
    bool isTargetLocked() const { return status.targetLocked; }
    uint8_t getLockedTargetIndex() const { return lockedTargetIndex; }
    
    // Firing control
    bool fire();
    bool isReadyToFire() const { return status.status == WeaponsStatus::READY; }
    uint8_t getQuantityReady() const { return status.quantityReady; }
    uint8_t getQuantityLoaded() const { return status.quantityLoaded; }
    
    // Status and health
    WeaponsSystemStatus getStatus() const { return status; }
    WeaponsStatus getWeaponsStatus() const { return status.status; }
    bool isHealthy() const { return status.healthy; }
    uint8_t getErrorFlags() const { return errorFlags; }
    
    // System control
    bool initialize();
    void update();
    void shutdown();
    bool performSelfTest();
    bool performCalibration();
    
    // Error handling
    String getLastError() const { return lastError; }
    void clearErrors();
    
    // Diagnostics
    void printStatus();
    void printTargets();
    void printConfiguration();
    
    // Hardware-specific methods
    bool setSafetyPin(uint8_t pin);
    bool setArmingPin(uint8_t pin);
    bool setFirePin(uint8_t pin);
    bool setStatusPin(uint8_t pin);
    bool setQuantityPin(uint8_t pin);
    
    // Target acquisition
    bool acquireTarget(uint16_t range, uint16_t bearing, int16_t velocity, uint8_t priority);
    bool updateTarget(uint8_t targetIndex, uint16_t range, uint16_t bearing, int16_t velocity);
    bool removeTarget(uint8_t targetIndex);
    
    // Weapons management
    bool reload();
    bool checkWeapons();
    uint8_t getMaxQuantity() const { return config.maxQuantity; }
    bool setMaxQuantity(uint8_t maxQty);
};

#endif // WEAPONS_INTERFACE_H 