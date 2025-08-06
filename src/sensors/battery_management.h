#ifndef BATTERY_MANAGEMENT_H
#define BATTERY_MANAGEMENT_H

#include <Arduino.h>

class battery_management {
public:
    battery_management(int batteryVoltDivPin);
    void readBatteryVoltage();
    float getBatteryVoltage();
    int getBatteryCapacity();
    bool isLowBattery();
    void setLowBatteryThreshold(float threshold);
    void setBatteryWarningLevels(int critical, int low, int medium);

private:
    int _batteryVoltDivPin;
    float _batteryVoltage;
    int _batteryCapacity;
    float _lowBatteryThreshold;
    int _criticalLevel;
    int _lowLevel;
    int _mediumLevel;

    float _mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
};

#endif // BATTERY_MANAGEMENT_H