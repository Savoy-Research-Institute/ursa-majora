/*Radio RX connect to the
blizzard, the read pin is under interrupt set by PPM library*/
//~~~~~~~~~~~~~~~~~~~~RADIO functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Function to switch the radio on
void switch_radio_on()
{
    // Check if the radio is currently off
    if (!radioIsOn)
    {
        // Turn the transistor on
        digitalWrite(radio_transistor_pin, HIGH);
        // Update the variable to indicate that the radio is on
        radioIsOn = true;
    }
}
void switch_radio_off()
{ // Function to switch the radio off
    // Check if the radio is currently on
    if (radioIsOn)
    {
        // Turn the transistor off
        digitalWrite(radio_t_PIN, LOW);
        // Update the variable to indicate that the radio is off
        radioIsOn = false;
    }
}
void read_radio_allchannels() // reads RADIO_pin behind an interrupt already
{
    while (radioIsOn)
    {
        // Interval at which the PPM is updated
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        { // interval is 50
            previousMillis = currentMillis;

            // Acquiring all the channels values
            throttle = ppm.read_channel(THROTTLE);
            roll = ppm.read_channel(ROLL);
            pitch = ppm.read_channel(PITCH);
            yaw = ppm.read_channel(YAW);
            switch3way_1 = ppm.read_channel(SWITCH3WAY_1);
            pot1 = ppm.read_channel(POT1);
            switch3way_2 = ppm.read_channel(SWITCH3WAY_2);
            pot2 = ppm.read_channel(POT2);

            // Print the values for the Arduino Serial Plotter
            // Serial.print("Throttle:");        Serial.print(throttle);       Serial.print(" ");
            // Serial.print("Roll:");            Serial.print(roll);           Serial.print(" ");
            // Serial.print("Pitch:");           Serial.print(pitch);          Serial.print(" ");
            // Serial.print("Yaw:");             Serial.print(yaw);            Serial.print(" ");
            // Serial.print("Switch_3way_1:");   Serial.print(switch3way_1);   Serial.print(" ");
            // Serial.print("Button:");          Serial.print(pot1);         Serial.print(" ");
            // Serial.print("Switch_3way_2:");   Serial.print(switch3way_2);   Serial.print(" ");
            // Serial.print("Pot:");             Serial.print(pot2);            Serial.print(" ");
            // Serial.println();
        }
    }
}
int read_throttle()
{
    while (radioIsOn)
    {
        // Interval at which the PPM is updated
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        { // interval is 50
            previousMillis = currentMillis;
            return ppm.read_channel(THROTTLE);
        }
    }
}

