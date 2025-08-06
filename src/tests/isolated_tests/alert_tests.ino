#include "Alert.h"

// Define the pin numbers for the LEDs and buzzer
const int greenPin = 2;
const int yellowPin = 3;
const int redPin = 4;
const int buzzerPin = 5;

// Create an instance of the Alert class
Alert alert(greenPin, yellowPin, redPin, buzzerPin);

void setup() {
    // No setup needed, all done in the constructor
}

void loop() {
    // Test green LED and short beep
    alert.greenOn();
    delay(1000); // Keep the green LED on for 1 second
    alert.greenOff();
    delay(1000);

    // Test yellow LED and medium beep
    alert.yellowOn();
    delay(1000); // Keep the yellow LED on for 1 second
    alert.yellowOff();
    delay(1000);

    // Test red LED and long beep
    alert.redOn();
    delay(1000); // Keep the red LED on for 1 second
    alert.redOff();
    delay(1000);

    // Test turning off all LEDs and buzzer
    alert.turnOffAll();
    delay(2000); // Wait 2 seconds before starting the cycle again
}
