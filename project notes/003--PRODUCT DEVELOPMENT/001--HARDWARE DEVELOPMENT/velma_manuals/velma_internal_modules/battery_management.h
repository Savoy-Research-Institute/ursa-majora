/*BMS code running on a separate arduino nano that does the management 
it speaks to the arduino mega, etc
Read voltage via divider, code to convert true voltage, compensate, for ESCs,
An actual BMS that accounts for charging and discharging, give max and min discharging and charging currents,
cut off charge and discharge if exceeds bounds
Onboard voltage regulator,
*/
// POwer management functions
void read_battery_voltage()
{ // calculate battery capacity via voltage divider on pin A0(BATT_pin)
    // Load the battery voltage to the batt_anlog_val variable.
    // 65 is the voltage compensation for the diode.
    // 12.6V equals ~5.04V @ Analog 0.
    // 12.6V equals 1023 analogRead(0).
    // 1260 / 1023 = 1.2317.
    // 0.09853 = 0.08 * 1.2317.
    // The variable battery_voltage holds 1050 if the battery voltage is 10.5V.
    // battery_voltage = (analogRead(0) + 65) * 1.2317;

    batt_anlog_val = analogRead(battery_voltdiv_pin); // 0-5v after ADC conversion 0-1024
    // batt_anlog_val = batt_anlog_val * 0.92 + analogRead(battery_voltdiv_pin) * 0.09853;
    //  A complementary filter is used to reduce noise.
    batt_true_val = (map(batt_anlog_val, 0, 1023, 0, 5)) * 2.5; // volt divider rescaling
    battery_voltage = batt_true_val * 100;                      // 1250-1150
    battery_capacity = map(batt_true_val, 11.2, 12.6, 20, 100); // 20-100
                                                                //  if (battery_voltage < low_battery_voltage){//show LED indication
}
void compensate_esc_for_voltage()
{
    if (battery_voltage < 1260 && battery_voltage > 1100)
    {                                                              // Is the battery connected?
        esc_1 += esc_1 * ((1240 - battery_voltage) / (float)3000); // Compensate the esc-1 pulse for voltage drop.
        esc_2 += esc_2 * ((1240 - battery_voltage) / (float)3000); // Compensate the esc-2 pulse for voltage drop.
        esc_3 += esc_3 * ((1240 - battery_voltage) / (float)3000); // Compensate the esc-3 pulse for voltage drop.
        esc_4 += esc_4 * ((1240 - battery_voltage) / (float)3000); // Compensate the esc-4 pulse for voltage drop.
    } // increase if drone rises
}

/*code for separate BMS
check charging and discharging for each cell, 
*/
/*voltage regulator inclusion*/