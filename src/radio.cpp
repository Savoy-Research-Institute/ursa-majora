#include <Arduino.h>
#include "radio.h"

// State variable to track radio power
bool radioIsOn = false;

// Timing variables for channel reading
unsigned long previousMillis = 0;
const unsigned long interval = 50; // Interval in ms

// Function to switch the radio on
void switch_radio_on()
{
    if (!radioIsOn)
    {
        digitalWrite(radio_transistor_pin, HIGH);
        radioIsOn = true;
    }
}

// Function to switch the radio off
void switch_radio_off()
{
    if (radioIsOn)
    {
        digitalWrite(radio_transistor_pin, LOW);
        radioIsOn = false;
    }
}

// Reads all radio channels using the PPM library
void read_radio_allchannels()
{
    if (!radioIsOn) return;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;

        throttle      = ppm.read_channel(THROTTLE);
        roll          = ppm.read_channel(ROLL);
        pitch         = ppm.read_channel(PITCH);
        yaw           = ppm.read_channel(YAW);
        switch3way_1  = ppm.read_channel(SWITCH3WAY_1);
        pot1          = ppm.read_channel(POT1);
        switch3way_2  = ppm.read_channel(SWITCH3WAY_2);
        pot2          = ppm.read_channel(POT2);

        // Uncomment for debugging:
        // Serial.print("Throttle: "); Serial.print(throttle); Serial.print(" ");
        // Serial.print("Roll: "); Serial.print(roll); Serial.print(" ");
        // Serial.print("Pitch: "); Serial.print(pitch); Serial.print(" ");
        // Serial.print("Yaw: "); Serial.print(yaw); Serial.print(" ");
        // Serial.print("Switch_3way_1: "); Serial.print(switch3way_1); Serial.print(" ");
        // Serial.print("Pot1: "); Serial.print(pot1); Serial.print(" ");
        // Serial.print("Switch_3way_2: "); Serial.print(switch3way_2); Serial.print(" ");
        // Serial.print("Pot2: "); Serial.print(pot2); Serial.println();
    }
}

// Reads only the throttle channel
int read_throttle()
{
    if (!radioIsOn) return 0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;
        return ppm.read_channel(THROTTLE);
    }
    return 0;
}