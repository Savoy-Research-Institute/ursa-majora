#ifndef HC12_H
#define HC12_H

#include <Arduino.h>

namespace Velma {
namespace HC12 {

    /**
     * @brief Initialize the HC-12 module.
     * 
     * @param serial Reference to HardwareSerial interface (e.g. Serial1, Serial2).
     * @param baud_rate Communication baud rate.
     */
    void init(HardwareSerial& serial, uint32_t baud_rate);

    /**
     * @brief Send data via HC-12.
     * 
     * @param message String message to send.
     */
    void send(const String& message);

    /**
     * @brief Receive data from HC-12 with timeout.
     * 
     * @return String received message (empty if timeout).
     */
    String receive();

    /**
     * @brief Check if the response is ACK.
     * 
     * @param response String response to check.
     * @return true if ACK.
     * @return false otherwise.
     */
    bool is_ack(const String& response);

    /**
     * @brief Send ACK response.
     */
    void send_ack();

    /**
     * @brief Check if the response is NAK.
     * 
     * @param response String response to check.
     * @return true if NAK.
     * @return false otherwise.
     */
    bool is_nak(const String& response);

    /**
     * @brief Send NAK response.
     */
    void send_nak();

    /**
     * @brief Handle retransmission logic if NAK or timeout.
     */
    void handle_retransmit();

    /**
     * @brief Reconnect and reinitialize the HC-12 module.
     */
    void reconnect();

} // namespace HC12
} // namespace Velma

#endif // HC12_H
