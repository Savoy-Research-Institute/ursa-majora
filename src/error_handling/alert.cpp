#include "Alert.h"

// Constructor: Initialize the pins for the LEDs and buzzer
Alert::Alert(int greenPin, int yellowPin, int redPin, int buzzerPin)
    : greenPin(greenPin), yellowPin(yellowPin), redPin(redPin), buzzerPin(buzzerPin)
{
    pinMode(greenPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(redPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);

    // Initialize all LEDs to off
    digitalWrite(greenPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(redPin, LOW);
    digitalWrite(buzzerPin, LOW);
}
// Function to control the buzzer duration
void Alert::switch_beeper(bool var, int duration)
{
    if (var == true)
    {                                  // switch beepers on,
        digitalWrite(buzzerPin, HIGH); // turn the LED on (HIGH is the voltage level)
        delay(duration);               // wait for a second
        digitalWrite(buzzerPin, LOW);
    }
    else if (var == false)
    {
        digitalWrite(buzzerPin, LOW);
    }
}

// LEDS sequences
void Alert::switch_green_led(bool var)
{
    if (var == true)
    {
        // switch green LED on,
        digitalWrite(greenPin, HIGH); // turn the LED on (HIGH is the voltage level)
        delay(1000);                  // wait for a second
    }
    else if (var == false)
    {
        // switch green LED off,
        digitalWrite(greenPin, LOW); // turn the LED off (LOW is the voltage level)
        delay(1000);                 // wait for a second
    }
}
void Alert::switch_red_led(bool var)
{
    if (var == true)
    {
        // switch red LED on,
        digitalWrite(redPin, HIGH); // turn the LED on (HIGH is the voltage level)
        delay(1000);                // wait for a second
    }
    else if (var == false)
    {
        // switch red LED off,
        digitalWrite(redPin, LOW); // turn the LED off (LOW is the voltage level)
        delay(1000);               // wait for a second
    }
}
void Alert::switch_yellow_led(bool var)
{
    if (var == true)
    {
        // switch green LED on,
        digitalWrite(yellowPin, HIGH); // turn the LED on (HIGH is the voltage level)
        delay(1000);                   // wait for a second
    }
    else if (var == false)
    {
        // switch green LED off,
        digitalWrite(yellowPin, LOW); // turn the LED off (LOW is the voltage level)
        delay(1000);                  // wait for a second
    }
}
// Function to turn off all LEDs and buzzer
void Alert::turnOffAll()
{
    switch_yellow_led(OFF);
    switch_red_led(OFF);
    switch_green_led(OFF);
    switch_beeper(OFF)
    //digitalWrite(buzzerPin, LOW);
}
void Alert::turnOnAll()
{
    switch_yellow_led(ON);
    switch_red_led(ON);
    switch_green_led(ON);
    switch_beeper(ON)
    //digitalWrite(buzzerPin, HIGH);
}
// Function to control LED status indicators
void setLEDStatus(String status)
{
    if (status == "ON")
    {
        // turn on LED
    }
    else if (status == "OFF")
    {
        // turn off LED
    }
    else if (status == "BLINK")
    { 
        // blink LED twice
    }
}
void switch_LED_BUILTIN(bool built_in_led)
{
    if (built_in_led == true)
    {
        // switch it on
        digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
        delay(1000);                     // wait for a second
    }
    else if (built_in_led == false)
    {
        digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
        delay(1000);                    // wait for a second
    }
}