#ifndef RADIO_H
#define RADIO_H

#include <Arduino.h>

namespace Velma {
namespace Radio {

    /**
     * @brief Initialize the radio module pins and state.
     * 
     * @param transistor_pin Pin controlling radio power transistor.
     */
    void init(int transistor_pin);

    /**
     * @brief Switch the radio module power ON via transistor control.
     */
    void switch_on();

    /**
     * @brief Switch the radio module power OFF.
     */
    void switch_off();

    /**
     * @brief Read all radio channels and update internal state.
     */
    void read_all_channels();

    /**
     * @brief Read throttle channel value.
     * 
     * @return int Throttle channel PWM value.
     */
    int read_throttle();

    /**
     * @brief Check if radio module is currently powered on.
     * 
     * @return true if powered on.
     * @return false otherwise.
     */
    bool is_on();

} // namespace Radio
} // namespace Velma

#endif // RADIO_H
#ifndef RADIO_H
#define RADIO_H

#include <Arduino.h>

namespace Velma {
namespace Radio {

    /**
     * @brief Radio channel enumeration for easy channel identification.
     */
    enum ChannelId {
        THROTTLE = 0,
        ROLL,
        PITCH,
        YAW,
        SWITCH3WAY_1,
        POT1,
        SWITCH3WAY_2,
        POT2,
        CHANNEL_COUNT = 8
    };

    /**
     * @brief Radio system status enumeration.
     */
    enum RadioStatus {
        RADIO_OFF,
        RADIO_ON,
        RADIO_FAILSAFE
    };

    /**
     * @brief Initialize the radio module pins and state.
     * 
     * @param transistor_pin Pin controlling radio power transistor.
     * @param ppm_pin Pin connected to PPM signal from receiver.
     * @param failsafe_timeout_ms Timeout in milliseconds for failsafe detection (default: 1000ms).
     */
    void init(int transistor_pin, int ppm_pin, unsigned long failsafe_timeout_ms = 1000);

    /**
     * @brief Switch the radio module power ON via transistor control.
     */
    void switch_on();

    /**
     * @brief Switch the radio module power OFF.
     */
    void switch_off();

    /**
     * @brief Read all radio channels and update internal state.
     * This function should be called regularly in the main loop.
     * It handles failsafe detection and automatic disarming.
     */
    void read_all_channels();

    /**
     * @brief Get raw PWM value for a specific channel.
     * 
     * @param channel Channel ID to read.
     * @return int PWM value in microseconds (typically 1000-2000µs), or 0 if failsafe.
     */
    int get_channel(ChannelId channel);

    /**
     * @brief Get normalized channel value (-100% to +100%).
     * 
     * @param channel Channel ID to read.
     * @return float Normalized value from -100.0 to +100.0, or 0.0 if failsafe.
     */
    float get_channel_normalized(ChannelId channel);

    /**
     * @brief Read throttle channel value (convenience function).
     * 
     * @return int Throttle channel PWM value in microseconds.
     */
    int read_throttle();

    /**
     * @brief Check if radio module is currently powered on.
     * 
     * @return true if powered on.
     * @return false otherwise.
     */
    bool is_on();

    /**
     * @brief Get current radio system status.
     * 
     * @return RadioStatus Current status (OFF, ON, or FAILSAFE).
     */
    RadioStatus get_status();

    /**
     * @brief Check if radio is in failsafe mode.
     * 
     * @return true if in failsafe mode.
     * @return false if receiving valid signals.
     */
    bool is_failsafe();

    /**
     * @brief Get time since last valid signal received.
     * 
     * @return unsigned long Time in milliseconds since last valid signal.
     */
    unsigned long get_time_since_last_signal();

    /**
     * @brief Set custom channel center and range values.
     * 
     * @param channel Channel ID to configure.
     * @param center_us Center value in microseconds (default: 1500).
     * @param range_us Range from center in microseconds (default: 500).
     */
    void set_channel_calibration(ChannelId channel, int center_us = 1500, int range_us = 500);

    /**
     * @brief Reset all channel calibrations to default values.
     */
    void reset_calibration();

} // namespace Radio
} // namespace Velma

#endif // RADIO_H