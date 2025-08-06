#include "velociraptor.h"
const int BATTERY_VOLT_DIV_PIN = A0;  // Assuming the voltage divider is connected to A0
BatteryManagement battery(BATTERY_VOLT_DIV_PIN);

//This is the main run file for Velma cube
void setup(){
    Serial.begin(9600);
    battery.setLowBatteryThreshold(11.2);  // Set low battery threshold to 11.2V
    battery.setBatteryWarningLevels(20, 30, 50);  // Set warning levels (critical, low, medium)



init_radio();
switch_radio_on();

switch_mpu_on();
delay(30);
reset_mpu_thru_register();
read_mpu_error();
}
void loop(){
//access sequence for blizzard, 

//docking sequence,

//while docked, run arming sequence 
    //check flight mode,  




    battery.readBatteryVoltage();
    float voltage = battery.getBatteryVoltage();
    int capacity = battery.getBatteryCapacity();
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