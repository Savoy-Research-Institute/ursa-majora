#include "battery_management.h"

// Pin definitions
const int battery_voltdiv_pin = A0;

// Battery management variables
int batt_anlog_val = 0;
float batt_true_val = 0.0;
int battery_voltage = 0;
int battery_capacity = 0;

// ESC variables
float esc_1 = 0, esc_2 = 0, esc_3 = 0, esc_4 = 0;

// Helper function for floating-point mapping
static float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Reads battery voltage and updates related variables
void read_battery_voltage()
{
    batt_anlog_val = analogRead(battery_voltdiv_pin); // 0-5V after ADC conversion 0-1024
    batt_true_val = fmap(batt_anlog_val, 0, 1023, 0.0, 5.0) * 2.5; // volt divider rescaling
    battery_voltage = batt_true_val * 100;                          // e.g., 1250 for 12.5V
    battery_capacity = fmap(batt_true_val, 11.2, 12.6, 20, 100);    // 20-100%
    // if (battery_voltage < low_battery_voltage) { /* LED indication */ }
}

// Compensates ESC signals for voltage drop
void compensate_esc_for_voltage()
{
    if (battery_voltage < 1260 && battery_voltage > 1100)
    { // Is the battery connected?
        esc_1 += esc_1 * ((1240 - battery_voltage) / 3000.0f);
        esc_2 += esc_2 * ((1240 - battery_voltage) / 3000.0f);
        esc_3 += esc_3 * ((1240 - battery_voltage) / 3000.0f);
        esc_4 += esc_4 * ((1240 - battery_voltage) / 3000.0f);
    }
}