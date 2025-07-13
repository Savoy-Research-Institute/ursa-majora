#include "hc12.h"

void hc12_init(uint32_t baud_rate) {
    Serial1.begin(baud_rate); // Assuming HC-12 on Serial1
}

void hc12_send(String message) {
    Serial1.println(message);
}

String hc12_receive() {
    if (Serial1.available()) {
        return Serial1.readStringUntil('\n');
    }
    return "";
}

bool hc12_check_ack(String response) {
    return response == "ACK";
}

void hc12_send_ack() {
    hc12_send("ACK");
}

void hc12_handle_retransmit() {
    // Placeholder for retransmission logic on timeout/NAK
}
// This function can be expanded to handle retransmission logic
void hc12_reset() {
    // Reset HC-12 module if needed
    // This could involve sending a specific command or resetting the serial interface
    Serial1.end();
    Serial1.begin(9600); // Reset to default baud rate
}
void hc12_set_mode(uint8_t mode) {
    // Set HC-12 mode (e.g., command mode, data mode)
    // This could involve sending a specific command to the HC-12
    Serial1.print("AT+MODE");
    Serial1.println(mode);
}
void hc12_set_channel(uint8_t channel) {
    // Set HC-12 channel
    // This could involve sending a specific command to the HC-12
    Serial1.print("AT+C");
    Serial1.println(channel);
}
void hc12_set_power(uint8_t power) {
    // Set HC-12 transmission power
    // This could involve sending a specific command to the HC-12
    Serial1.print("AT+P");
    Serial1.println(power);
}
void hc12_set_baud(uint32_t baud_rate) {
    // Set HC-12 baud rate
    // This could involve sending a specific command to the HC-12
    Serial1.print("AT+B");
    Serial1.println(baud_rate);
}
void hc12_get_status() {
    // Get HC-12 status
    // This could involve sending a specific command to the HC-12
    Serial1.println("AT+S");
    // Read and process the response
    if (Serial1.available()) {
        String status = Serial1.readStringUntil('\n');
        Serial.println("HC-12 Status: " + status);
    }
}
void hc12_set_address(uint8_t address) {
    // Set HC-12 address
    // This could involve sending a specific command to the HC-12
    Serial1.print("AT+ADDR");
    Serial1.println(address);
}
void hc12_get_address() {
    // Get HC-12 address
    // This could involve sending a specific command to the HC-12
    Serial1.println("AT+ADDR?");
    // Read and process the response
    if (Serial1.available()) {
        String address = Serial1.readStringUntil('\n');
        Serial.println("HC-12 Address: " + address);
    }
}
void hc12_set_sleep_mode(bool enable) {
    // Set HC-12 sleep mode
    // This could involve sending a specific command to the HC-12
    if (enable) {
        Serial1.println("AT+SLEEP=1");
    } else {
        Serial1.println("AT+SLEEP=0");
    }
}
void hc12_get_signal_strength() {
    // Get HC-12 signal strength
    // This could involve sending a specific command to the HC-12
    Serial1.println("AT+RSSI");
    // Read and process the response
    if (Serial1.available()) {
        String rssi = Serial1.readStringUntil('\n');
        Serial.println("HC-12 RSSI: " + rssi);
    }
}
void hc12_set_transmission_mode(uint8_t mode) {
    // Set HC-12 transmission mode (e.g., normal, low power)
    // This could involve sending a specific command to the HC-12
    Serial1.print("AT+TMODE");
    Serial1.println(mode
);
}
void hc12_get_transmission_mode() {
    // Get HC-12 transmission mode
    // This could involve sending a specific command to the HC-12
    Serial1.println("AT+TMODE?");
    // Read and process the response
    if (Serial1.available()) {
        String mode = Serial1.readStringUntil('\n');
        Serial.println("HC-12 Transmission Mode: " + mode);
    }
}
void hc12_set_encryption_key(String key) {
    // Set HC-12 encryption key
    // This could involve sending a specific command to the HC-12
    Serial1.print("AT+KEY");
    Serial1.println(key);
}
void hc12_get_encryption_key() {
    // Get HC-12 encryption key
    // This could involve sending a specific command to the HC-12
    Serial1.println("AT+KEY?");
    // Read and process the response
    if (Serial1.available()) {
        String key = Serial1.readStringUntil('\n');
        Serial.println("HC-12 Encryption Key: " + key);
    }
}
void hc12_set_transmission_interval(uint16_t interval) {
    // Set HC-12 transmission interval
    // This could involve sending a specific command to the HC-12
    Serial1.print("AT+TINT");
    Serial1.println(interval
);  
}
void hc12_get_transmission_interval() {
    // Get HC-12 transmission interval
    // This could involve sending a specific command to the HC-12
    Serial1.println("AT+TINT?");
    // Read and process the response
    if (Serial1.available()) {
        String interval = Serial1.readStringUntil('\n');
        Serial.println("HC-12 Transmission Interval: " + interval);
    }
}
void hc12_set_retransmission_count(uint8_t count) {
    // Set HC-12 retransmission count
    // This could involve sending a specific command to the HC-12
    Serial1.print("AT+RETRY");
    Serial1.println(count);
}
void hc12_get_retransmission_count() {
    // Get HC-12 retransmission count
    // This could involve sending a specific command to the HC-12
    Serial1.println("AT+RETRY?");
    // Read and process the response
    if (Serial1.available()) {
        String count = Serial1.readStringUntil('\n');
        Serial.println("HC-12 Retransmission Count: " + count);
    }
}
