#ifndef VEC_H
#define VEC_H

#include <Arduino.h>
// Include other necessary libraries here
// #include "Battery.h"
// #include "MPU.h"
// #include "ESC.h"
// #include "BlizzardConsole.h"
#include "console_interface.h"

class VEC {
public:
    VEC();
    bool runAllChecks();
    bool checkKeypad();

private:
    bool checkBattery();
    bool checkMPU();
    bool checkSensors();
    bool checkESCs();
    bool checkBlizzardConsole();

    // Add any necessary member variables here
};

VEC::VEC() {
    // Initialize any necessary components
}

bool VEC::runAllChecks() {
    bool allPassed = true;
    
    allPassed &= checkBattery();
    allPassed &= checkMPU();
    allPassed &= checkSensors();
    allPassed &= checkESCs();
    allPassed &= checkBlizzardConsole();
    allPassed &= checkKeypad();

    return allPassed;
}

bool VEC::checkBattery() {
    // Implement battery check
    // Return true if battery is OK, false otherwise
}
bool VEC::checkKeypad() {
    ConsoleInterface console; // Make sure you have access to the ConsoleInterface
    return console.runKeypadTest();
    /*    
    if (vec.checkKeypad()) {//run this on main file.............
        Serial.println(F("Keypad test passed"));
    } else {
        Serial.println(F("Keypad test failed"));
    }*/
}
bool VEC::checkMPU() {
    // Implement MPU check
    // Return true if MPU is OK, false otherwise
}

bool VEC::checkSensors() {
    // Implement checks for all sensors
    // Return true if all sensors are OK, false otherwise
}

bool VEC::checkESCs() {
    // Implement ESC checks
    // Return true if all ESCs are OK, false otherwise
}

bool VEC::checkBlizzardConsole() {
    // Implement Blizzard Console check
    // Return true if Blizzard Console is OK, false otherwise
}

#endif // VEC_H