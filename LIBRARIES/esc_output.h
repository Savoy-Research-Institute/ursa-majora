#ifndef ESC_OUTPUT_H
#define ESC_OUTPUT_H

#include <Arduino.h>

// Function declarations
void esc_output();
void calculate_esc_pulse();
void calibrate_escs();
void compensate_esc_for_voltage();
void setup_motors();

// Shared variables
extern volatile unsigned long timer_channel_1;
extern volatile unsigned long timer_channel_2;
extern volatile unsigned long timer_channel_3;
extern volatile unsigned long timer_channel_4;
extern volatile unsigned long esc_loop_timer;
extern volatile unsigned long loop_timer;

extern int esc_1, esc_2, esc_3, esc_4;
extern int throttle;
extern bool primary_armed;
extern int pid_output_pitch, pid_output_roll, pid_output_yaw;
extern int battery_voltage;

#endif // ESC_OUTPUT_H