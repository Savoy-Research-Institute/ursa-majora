#include "hc12.h"

namespace Velma {
namespace HC12 {

    // ===============================
    // INTERNAL STATE VARIABLES
    // ===============================
    
    static HardwareSerial* hc12Serial = nullptr;
    static uint8_t localNodeId = 1;
    static int setPinNumber = -1;
    static uint8_t currentSequenceNumber = 1;
    
    // Message handling
    static MessageHandler messageHandlers[256];
    static MessageHandler defaultHandler = nullptr;
    
    // Reliability tracking
    static TransmissionContext pendingTransmissions[MAX_RETRIES];
    static uint8_t pendingTransmissionCount = 0;
    
    // Network management
    static NodeInfo networkNodes[MAX_NODES];
    static uint8_t networkNodeCount = 0;
    
    // Statistics
    static uint32_t messagesSent = 0;
    static uint32_t messagesReceived = 0;
    static uint32_t transmissionErrors = 0;
    
    // Configuration
    static uint8_t maxRetries = MAX_RETRIES;
    static uint32_t ackTimeout = ACK_TIMEOUT_MS;
    
    // Message buffers
    static uint8_t receiveBuffer[HEADER_SIZE + MAX_PAYLOAD_SIZE + 1];
    static uint8_t receiveBufferIndex = 0;
    static bool messageInProgress = false;
    
    // ===============================
    // INTERNAL HELPER FUNCTIONS
    // ===============================
    
    /**
     * @brief Send AT command and wait for response
     */
    static bool send_at_command(const String& command, const String& expected_response = "OK", uint32_t timeout_ms = 1000) {
        if (!hc12Serial) return false;
        
        hc12Serial->print(command);
        
        uint32_t start_time = millis();
        String response = "";
        
        while (millis() - start_time < timeout_ms) {
            if (hc12Serial->available()) {
                char c = hc12Serial->read();
                if (c == '\r' || c == '\n') {
                    if (response.length() > 0) {
                        if (response.indexOf(expected_response) != -1) {
                            return true;
                        }
                        response = "";
                    }
                } else {
                    response += c;
                }
            }
        }
        
        return false;
    }
    
    /**
     * @brief Add transmission to pending list for reliability tracking
     */
    static void add_pending_transmission(uint8_t sequence_num) {
        if (pendingTransmissionCount < MAX_RETRIES) {
            TransmissionContext& ctx = pendingTransmissions[pendingTransmissionCount];
            ctx.sequence_num = sequence_num;
            ctx.retry_count = 0;
            ctx.last_send_time = millis();
            ctx.timeout_ms = ackTimeout;
            ctx.waiting_for_ack = true;
            pendingTransmissionCount++;
        }
    }
    
    /**
     * @brief Remove transmission from pending list
     */
    static void remove_pending_transmission(uint8_t sequence_num) {
        for (uint8_t i = 0; i < pendingTransmissionCount; i++) {
            if (pendingTransmissions[i].sequence_num == sequence_num) {
                // Shift remaining elements down
                for (uint8_t j = i; j < pendingTransmissionCount - 1; j++) {
                    pendingTransmissions[j] = pendingTransmissions[j + 1];
                }
                pendingTransmissionCount--;
                break;
            }
        }
    }
    
    /**
     * @brief Build complete message with header and payload
     */
    static void build_message(Message& message, uint8_t dest_id, MessageType msg_type, 
                             const uint8_t* payload, uint8_t payload_size, bool require_ack) {
        // Build header
        message.header.start_byte = 0xAA;
        message.header.source_id = localNodeId;
        message.header.dest_id = dest_id;
        message.header.message_type = static_cast<uint8_t>(msg_type);
        message.header.sequence_num = get_next_sequence_number();
        message.header.payload_length = payload_size;
        message.header.flags = require_ack ? 0x01 : 0x00;
        
        // Calculate header checksum
        message.header.checksum = calculate_checksum(reinterpret_cast<const uint8_t*>(&message.header), HEADER_SIZE - 1);
        
        // Copy payload
        if (payload && payload_size > 0) {
            memcpy(message.payload, payload, payload_size);
        }
        
        // Calculate payload checksum
        message.payload_checksum = calculate_checksum(message.payload, payload_size);
        
        // Set timestamp
        message.timestamp = millis();
    }
    
    /**
     * @brief Send raw message over serial
     */
    static CommStatus send_raw_message(const Message& message) {
        if (!hc12Serial) return COMM_HARDWARE_ERROR;
        
        // Send header
        hc12Serial->write(reinterpret_cast<const uint8_t*>(&message.header), HEADER_SIZE);
        
        // Send payload
        if (message.header.payload_length > 0) {
            hc12Serial->write(message.payload, message.header.payload_length);
        }
        
        // Send payload checksum
        hc12Serial->write(message.payload_checksum);
        
        messagesSent++;
        return COMM_SUCCESS;
    }
    
    // ===============================
    // CONFIGURATION & CONTROL
    // ===============================
    
    bool init(HardwareSerial& serial, uint32_t baud_rate, uint8_t node_id, int set_pin) {
        hc12Serial = &serial;
        localNodeId = node_id;
        setPinNumber = set_pin;
        
        // Initialize serial communication
        hc12Serial->begin(baud_rate);
        
        // Configure SET pin if provided
        if (setPinNumber >= 0) {
            pinMode(setPinNumber, OUTPUT);
            digitalWrite(setPinNumber, HIGH); // Default to transparent mode
        }
        
        // Clear message handlers
        for (int i = 0; i < 256; i++) {
            messageHandlers[i] = nullptr;
        }
        
        // Initialize network node tracking
        networkNodeCount = 0;
        
        // Reset statistics
        reset_stats();
        
        delay(100); // Allow HC-12 to stabilize
        
        return true;
    }
    
    bool configure(uint8_t channel, PowerLevel power_level, uint32_t baud_rate) {
        if (!enter_command_mode()) return false;
        
        delay(100);
        
        // Set channel (1-127)
        String channel_cmd = "AT+C" + String(channel, DEC);
        if (!send_at_command(channel_cmd)) {
            exit_command_mode();
            return false;
        }
        
        // Set power level (1-8)
        String power_cmd = "AT+P" + String(static_cast<uint8_t>(power_level), DEC);
        if (!send_at_command(power_cmd)) {
            exit_command_mode();
            return false;
        }
        
        // Set baud rate
        String baud_cmd = "AT+B";
        switch (baud_rate) {
            case 1200: baud_cmd += "1200"; break;
            case 2400: baud_cmd += "2400"; break;
            case 4800: baud_cmd += "4800"; break;
            case 9600: baud_cmd += "9600"; break;
            case 19200: baud_cmd += "19200"; break;
            case 38400: baud_cmd += "38400"; break;
            case 57600: baud_cmd += "57600"; break;
            case 115200: baud_cmd += "115200"; break;
            default: baud_cmd += "9600"; break;
        }
        
        if (!send_at_command(baud_cmd)) {
            exit_command_mode();
            return false;
        }
        
        return exit_command_mode();
    }
    
    bool reset() {
        if (!enter_command_mode()) return false;
        
        delay(100);
        
        // Send reset command
        bool success = send_at_command("AT+DEFAULT");
        
        delay(500); // Allow time for reset
        
        exit_command_mode();
        
        if (success) {
            // Reinitialize after reset
            delay(1000);
            // Reset internal state
            currentSequenceNumber = 1;
            pendingTransmissionCount = 0;
            networkNodeCount = 0;
        }
        
        return success;
    }
    
    bool enter_command_mode() {
        if (setPinNumber < 0) return false;
        
        digitalWrite(setPinNumber, LOW);
        delay(100);
        
        // Test if in command mode
        return send_at_command("AT", "OK", 500);
    }
    
    bool exit_command_mode() {
        if (setPinNumber < 0) return false;
        
        digitalWrite(setPinNumber, HIGH);
        delay(100);
        return true;
    }
    
    bool test_link(uint8_t dest_id, uint32_t timeout_ms) {
        String test_payload = "PING";
        CommStatus result = send_string(dest_id, MSG_DEBUG, test_payload, true);
        
        if (result == COMM_SUCCESS) {
            // Wait for response
            uint32_t start_time = millis();
            while (millis() - start_time < timeout_ms) {
                if (process_messages() > 0) {
                    // Check if we received a response from the target node
                    // This is a simplified check - in practice, you'd want to verify
                    // the response is actually a PONG to our PING
                    return true;
                }
                delay(10);
            }
        }
        
        return false;
    }}