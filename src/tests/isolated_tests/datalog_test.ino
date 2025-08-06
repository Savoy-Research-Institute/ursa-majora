#include "data_logger.h"

const int SD_CHIP_SELECT_PIN = 10;  // Adjust this to your SD card's CS pin
DataLogger dataLogger(SD_CHIP_SELECT_PIN);

void setup() {
    Serial.begin(9600);
    dataLogger.begin();
    dataLogger.switchOn();
    
    if (dataLogger.createLogFile("flight_log.txt")) {
        Serial.println("Log file created successfully");
    } else {
        Serial.println("Failed to create log file");
    }
}

void loop() {
    // Example: Logging some data
    String logData = "Time: " + String(millis()) + ", Altitude: 100, Speed: 50";
    if (dataLogger.writeLogData("flight_log.txt", logData)) {
        Serial.println("Data logged successfully");
    } else {
        Serial.println("Failed to log data");
    }

    // Example: Storing a cheat code script
    String cheatCodeScript = "function activateCheat() print('Cheat activated!') end";
    if (dataLogger.storeCheatCodeScript("superSpeed", cheatCodeScript)) {
        Serial.println("Cheat code script stored successfully");
    } else {
        Serial.println("Failed to store cheat code script");
    }

    delay(1000);  // Log data every second
}