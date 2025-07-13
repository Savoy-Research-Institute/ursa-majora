#include "hc12.h"

namespace Velma {
namespace HC12 {

    // Define HC-12 serial interface pointer
    static HardwareSerial* hc12Serial = nullptr;

    static const unsigned long timeoutMs = 2000; // ACK wait timeout
    static String lastMessage = "";              // Buffer for retransmission

    void init(HardwareSerial& serial, uint32_t baud_rate) {
        hc12Serial = &serial;
        hc12Serial->begin(baud_rate);
    }

    void send(const String& message) {
        if (!hc12Serial) return;

        lastMessage = message;
        hc12Serial->println(message);
    }

    String receive() {
        if (!hc12Serial) return "";

        String received = "";
        unsigned long startTime = millis();
        while ((millis() - startTime) < timeoutMs) {
            if (hc12Serial->available()) {
                received = hc12Serial->readStringUntil('\n');
                received.trim();
                break;
            }
        }
        return received;
    }

    bool is_ack(const String& response) {
        return (response == "ACK");
    }

    void send_ack() {
        if (!hc12Serial) return;
        hc12Serial->println("ACK");
    }

    bool is_nak(const String& response) {
        return (response == "NAK");
    }

    void send_nak() {
        if (!hc12Serial) return;
        hc12Serial->println("NAK");
    }

    void handle_retransmit() {
        String response = receive();

        if (response == "") {
            Serial.println("[HC12] Timeout: No response received. Retransmitting...");
            send(lastMessage);
            // Optionally implement exponential backoff delay here
        } else if (is_nak(response)) {
            Serial.println("[HC12] NAK received. Retransmitting...");
            send(lastMessage);
        } else if (is_ack(response)) {
            Serial.println("[HC12] ACK received. Transmission successful.");
        } else {
            Serial.print("[HC12] Unexpected response: ");
            Serial.println(response);
        }
    }

    void reconnect() {
        if (!hc12Serial) return;
        hc12Serial->end();
        delay(100);
        hc12Serial->begin(9600); // Restart with default baud rate or previous config
        Serial.println("[HC12] Module reinitialized.");
    }

} // namespace HC12
} // namespace Velma
