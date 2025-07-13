//HC-12 communication initialization, ACK/NAK implementation, transmission/receive parsing, error handling, and reconnection logic.
#ifndef HC12_H
#define HC12_H

#include <Arduino.h>

// Initialize HC-12 module
void hc12_init(uint32_t baud_rate);

// Send data via HC-12
void hc12_send(String message);

// Receive data from HC-12
String hc12_receive();

// Check for ACK from receiver
bool hc12_check_ack(String response);

// Send ACK to transmitter
void hc12_send_ack();

// Handle retransmission logic if NAK or timeout
void hc12_handle_retransmit();

#endif // HC12_H
