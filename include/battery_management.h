#ifndef BATTERY_MANAGEMENT_H
#define BATTERY_MANAGEMENT_H

#include <Arduino.h>

// Pin definition for battery voltage divider
extern const int battery_voltdiv_pin;

// Battery management variables
extern int batt_anlog_val;      // Raw analog value from voltage divider
extern float batt_true_val;     // Calculated true battery voltage
extern int battery_voltage;     // Battery voltage in centivolts (e.g., 1250 = 12.5V)
extern int battery_capacity;    // Estimated battery capacity percentage

// ESC (Electronic Speed Controller) compensation variables
extern float esc_1, esc_2, esc_3, esc_4;

// Reads and updates battery voltage and capacity
void read_battery_voltage();

// Compensates ESC signals based on battery voltage
void compensate_esc_for_voltage();

#endif // BATTERY_MANAGEMENT_H