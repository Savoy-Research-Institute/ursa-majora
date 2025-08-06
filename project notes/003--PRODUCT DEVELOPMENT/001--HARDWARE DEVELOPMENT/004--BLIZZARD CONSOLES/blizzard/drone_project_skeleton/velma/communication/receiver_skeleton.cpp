
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

//BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

/*Radio RX connect to pin 11
blizzard, the read pin is under interrupt set by PPM library*/
//~~~~~~~~~~~~~~~~~~~~RADIO functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Function to switch the radio on
// Function to switch the radio off
    // Check if the radio is currently on
        // Turn the transistor off if it was on
        // Update the variable to indicate that the radio is off
// reads RADIO_pin behind an interrupt already
        // Interval at which the PPM is updated
            // Acquiring all the channels values
            // Can and will Print the values for the Arduino Serial Plotter
            // Serial.print("Throttle:");        Serial.print(throttle);       Serial.print(" ");
            // Serial.print("Roll:");            Serial.print(roll);           Serial.print(" ");
            // Serial.print("Pitch:");           Serial.print(pitch);          Serial.print(" ");
            // Serial.print("Yaw:");             Serial.print(yaw);            Serial.print(" ");
            // Serial.print("Switch_3way_1:");   Serial.print(switch3way_1);   Serial.print(" ");
            // Serial.print("Button:");          Serial.print(pot1);         Serial.print(" ");
            // Serial.print("Switch_3way_2:");   Serial.print(switch3way_2);   Serial.print(" ");
            // Serial.print("Pot:");             Serial.print(pot2);            Serial.print(" ");
            // Serial.println();
//int read_throttle() reads throttle only, 
// on a transistor pin, join bind pin and volt pin to initiate radio transmitter, radio receiver  binding sequence
//ECO mode? switch on to read, switch off, this sequence is supposed to save on power, 
//calibration tests? where the pilot calibrates the endpoints of each channel, the software then uses these as min and max thresholds

//binding process, pin under transistors, 