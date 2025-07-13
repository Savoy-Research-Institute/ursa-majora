#include "radio.h"
#include <PPMReader.h> // Include your actual PPM library header

namespace Velma {
namespace Radio {

    static int transistorPin = -1;
    static bool radioIsOn = false;

    static unsigned long previousMillis = 0;
    static const unsigned long interval = 50;

    // Channel indices
    enum Channels {
        THROTTLE = 0,
        ROLL,
        PITCH,
        YAW,
        SWITCH3WAY_1,
        POT1,
        SWITCH3WAY_2,
        POT2
    };

    // Channel values
    static int throttle, roll, pitch, yaw;
    static int switch3way_1, pot1, switch3way_2, pot2;

    // PPM reader object (adjust pin as per your wiring)
    static PPMReader ppm(2); // Pin 2 as example

    void init(int pin) {
        transistorPin = pin;
        pinMode(transistorPin, OUTPUT);
        switch_off();
    }

    void switch_on() {
        if (!radioIsOn && transistorPin != -1) {
            digitalWrite(transistorPin, HIGH);
            radioIsOn = true;
        }
    }

    void switch_off() {
        if (radioIsOn && transistorPin != -1) {
            digitalWrite(transistorPin, LOW);
            radioIsOn = false;
        }
    }

    bool is_on() {
        return radioIsOn;
    }

    void read_all_channels() {
        if (!radioIsOn) return;

        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;

            throttle      = ppm.latestValidChannelValue(THROTTLE, 1000);
            roll          = ppm.latestValidChannelValue(ROLL, 1500);
            pitch         = ppm.latestValidChannelValue(PITCH, 1500);
            yaw           = ppm.latestValidChannelValue(YAW, 1500);
            switch3way_1  = ppm.latestValidChannelValue(SWITCH3WAY_1, 1500);
            pot1          = ppm.latestValidChannelValue(POT1, 1500);
            switch3way_2  = ppm.latestValidChannelValue(SWITCH3WAY_2, 1500);
            pot2          = ppm.latestValidChannelValue(POT2, 1500);

            // Optional debug output
            /*
            Serial.print("Throttle: "); Serial.print(throttle); Serial.print(" ");
            Serial.print("Roll: "); Serial.print(roll); Serial.print(" ");
            Serial.print("Pitch: "); Serial.print(pitch); Serial.print(" ");
            Serial.print("Yaw: "); Serial.print(yaw); Serial.print(" ");
            Serial.print("Switch3way_1: "); Serial.print(switch3way_1); Serial.print(" ");
            Serial.print("Pot1: "); Serial.print(pot1); Serial.print(" ");
            Serial.print("Switch3way_2: "); Serial.print(switch3way_2); Serial.print(" ");
            Serial.print("Pot2: "); Serial.print(pot2); Serial.println();
            */
        }
    }

    int read_throttle() {
        if (!radioIsOn) return 0;

        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            throttle = ppm.latestValidChannelValue(THROTTLE, 1000);
        }
        return throttle;
    }

} // namespace Radio
} // namespace Velma
