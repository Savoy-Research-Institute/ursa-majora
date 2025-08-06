#ifndef ALERT_H
#define ALERT_H

#include <Arduino.h>

class Alert {
public:
    // Constructor to initialize pins for the LEDs and buzzer
    Alert(int greenPin, int yellowPin, int redPin, int buzzerPin);

    // Functions to control the LEDs
    void switch_green_led(bool var);
    void switch_yellow_led(bool var);
    void switch_red_led(bool var);
    void switch_beeper(bool var, int duration);// Function to control the buzzer

    
    // Function to turn off all LEDs and buzzer
    void turnOffAll();
    void turnOnAll();
    // void switch_LED_BUILTIN();

private:
    int greenPin, yellowPin, redPin, buzzerPin;
};

#endif // ALERT_H
