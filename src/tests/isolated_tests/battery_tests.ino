#include "BatteryManagement.h"

const int BATTERY_VOLT_DIV_PIN = A0;  // Assuming the voltage divider is connected to A0
BatteryManagement battery(BATTERY_VOLT_DIV_PIN);

void setup() {
    Serial.begin(9600);
    battery.setLowBatteryThreshold(11.2);  // Set low battery threshold to 11.2V
    battery.setBatteryWarningLevels(20, 30, 50);  // Set warning levels (critical, low, medium)
}

void loop() {
    battery.readBatteryVoltage();
    
    float voltage = battery.getBatteryVoltage();
    int capacity = battery.getBatteryCapacity();
    
    Serial.print("Battery Voltage: ");
    Serial.print(voltage);
    Serial.print("V, Capacity: ");
    Serial.print(capacity);
    Serial.println("%");
    
    if (battery.isLowBattery()) {
        Serial.println("WARNING: Low Battery!");
    }
    
    // You can add LED warnings based on capacity levels
    if (capacity <= 20) {
        // Critical level - Red LED
    } else if (capacity <= 30) {
        // Low level - Yellow LED
    } else if (capacity <= 50) {
        // Medium level - Green LED
    }
    
    delay(1000);  // Read every second
}