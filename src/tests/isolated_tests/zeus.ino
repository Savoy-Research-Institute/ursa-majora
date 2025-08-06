#include "zeus.h"

Zeus zeus;

void setup() {
    Serial.begin(9600);
    zeus.init();
}

void loop() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        
        if (input == "toggle") {
            zeus.toggleCheatCodeMode();
            Serial.println(zeus.isCheatCodeModeActive() ? "Cheat mode ON" : "Cheat mode OFF");
        } else if (zeus.isCheatCodeModeActive()) {
            if (zeus.executeCheatCode(input.c_str())) {
                Serial.println("Cheat code executed successfully");
            } else {
                Serial.println("Invalid cheat code");
            }
        }
    }
}

//Cruise mode activated
//Cheat code executed successfully