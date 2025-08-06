#include "esc_output.h"

volatile unsigned long timer_channel_1, timer_channel_2, timer_channel_3, timer_channel_4;
volatile unsigned long esc_loop_timer, loop_timer;
int esc_1, esc_2, esc_3, esc_4;
int throttle;
bool primary_armed;
int pid_output_pitch, pid_output_roll, pid_output_yaw;
int battery_voltage;

void esc_output() {
    while (micros() - loop_timer < 4000);// We wait until 4000us are passed.
    loop_timer = micros(); // makes sure refresh rate always 250Hz. Set the timer for the next loop.
    PORTH |= B00011000;    //set esc pins 4,5,6,7 HIGH
    PORTE |= B00001000;
    PORTG |= B00100000;

    timer_channel_1 = esc_1 + loop_timer;// Calculate the time of the falling edge of the esc-1 pulse.
    timer_channel_2 = esc_2 + loop_timer;
    timer_channel_3 = esc_3 + loop_timer;
    timer_channel_4 = esc_4 + loop_timer;

    while ((PORTH & B00001000) || (PORTE & B00001000) || (PORTG & B00100000) || (PORTH & B00010000)) {
        esc_loop_timer = micros();
        if (timer_channel_1 <= esc_loop_timer) PORTG &= B11011111;// When the delay time is expired, digital port 4 is set low.
        if (timer_channel_2 <= esc_loop_timer) PORTE &= B11110111;
        if (timer_channel_3 <= esc_loop_timer) PORTH &= B11110111;
        if (timer_channel_4 <= esc_loop_timer) PORTH &= B11101111;
    }
}

void calculate_esc_pulse() {
    if (primary_armed) {
        //get throttle;
        if (throttle > 1800) throttle = 1800;

        esc_1 = throttle - pid_output_pitch + pid_output_roll - pid_output_yaw;
        esc_2 = throttle + pid_output_pitch + pid_output_roll + pid_output_yaw;
        esc_3 = throttle + pid_output_pitch - pid_output_roll - pid_output_yaw;
        esc_4 = throttle - pid_output_pitch - pid_output_roll + pid_output_yaw;

        compensate_esc_for_voltage();

        esc_1 = constrain(esc_1, 1050, 2000);
        esc_2 = constrain(esc_2, 1050, 2000);
        esc_3 = constrain(esc_3, 1050, 2000);
        esc_4 = constrain(esc_4, 1050, 2000);
    } else {//if primary armed is false
        esc_1 = esc_2 = esc_3 = esc_4 = 1000;
    }
}

void calibrate_escs() {
    // Implement ESC calibration logic here
}

void compensate_esc_for_voltage() {
    if (battery_voltage < 1260 && battery_voltage > 1100) {
        float compensation = (1240 - battery_voltage) / 3000.0f;
        //if the drone rises with decrease in battery, increase compensation number
        esc_1 += esc_1 * compensation;
        esc_2 += esc_2 * compensation;
        esc_3 += esc_3 * compensation;
        esc_4 += esc_4 * compensation;
    }
}

void setup_motors() {
    // Implement motor setup logic here
}

// You'll need to implement or declare these functions elsewhere:
// void read_throttle();
