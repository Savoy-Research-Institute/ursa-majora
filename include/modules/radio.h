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
