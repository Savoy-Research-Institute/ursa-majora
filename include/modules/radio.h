#ifndef RADIO_H
#define RADIO_H

#include <Arduino.h>

// Define the radio transistor control pin externally
extern const int radio_transistor_pin;

// State variable for radio power
extern bool radioIsOn;

// Timing variables for channel reading
extern unsigned long previousMillis;
extern const unsigned long interval;

// Channel values (should be defined elsewhere or here as needed)
extern int throttle, roll, pitch, yaw;
extern int switch3way_1, pot1, switch3way_2, pot2;

// PPM library object (should be defined in your main sketch or another file)
extern PPM ppm;

// Channel indices (define as needed)
#define THROTTLE      0
#define ROLL          1
#define PITCH         2
#define YAW           3
#define SWITCH3WAY_1  4
#define POT1          5
#define SWITCH3WAY_2  6
#define POT2          7

// Function declarations
void switch_radio_on();
void switch_radio_off();
void read_radio_allchannels();
int read_throttle();
#endif // RADIO_H