#ifndef HC12_H
#define HC12_H

#include <Arduino.h>

namespace Velma {
namespace HC12 {

    // ===============================
    // CONSTANTS & CONFIGURATION
    // ===============================

    static const uint8_t MAX_PAYLOAD_SIZE = 200;       ///< Maximum payload size in bytes
    static const uint8_t MAX_RETRIES = 3;              ///< Maximum number of retransmission attempts
    static const uint16_t ACK_TIMEOUT_MS = 1000;       ///< ACK timeout in milliseconds
    static const uint16_t BASE_BACKOFF_MS = 100;       ///< Base backoff time for exponential backoff
    static const uint8_t HEADER_SIZE = 8;              ///< Protocol header size in bytes
    static const uint8_t MAX_NODES = 16;               ///< Maximum number of nodes in network

    // ===============================
    // ENUMERATIONS
    // ===============================

    /**
     * @brief Message types for multiplexing
     */
    enum MessageType : uint8_t {
        MSG_TELEMETRY = 0x01,       ///< Telemetry data
        MSG_COMMAND = 0x02,         ///< Command message
        MSG_ACK = 0x03,             ///< Acknowledgment
        MSG_NAK = 0x04,             ///< Negative acknowledgment
        MSG_HEARTBEAT = 0x05,       ///< Heartbeat/keep-alive
        MSG_STATUS = 0x06,          ///< Status update
        MSG_DEBUG = 0x07,           ///< Debug information
        MSG_EMERGENCY = 0x08,       ///< Emergency/priority message
        MSG_CUSTOM = 0x10           ///< Custom message types start here
    };

    /**
     * @brief Communication status codes
     */
    enum CommStatus : uint8_t {
        COMM_SUCCESS = 0x00,        ///< Operation successful
        COMM_TIMEOUT = 0x01,        ///< Timeout occurred
        COMM_CHECKSUM_ERROR = 0x02, ///< Checksum validation failed
        COMM_BUFFER_FULL = 0x03,    ///< Buffer overflow
        COMM_INVALID_FORMAT = 0x04, ///< Invalid message format
        COMM_NO_ACK = 0x05,         ///< No acknowledgment received
        COMM_HARDWARE_ERROR = 0x06  ///< Hardware communication error
    };

    /**
     * @brief HC-12 power levels
     */
    enum PowerLevel : uint8_t {
        POWER_MINUS_1DBM = 1,       ///< -1 dBm (minimum power)
        POWER_2DBM = 2,             ///< 2 dBm
        POWER_5DBM = 3,             ///< 5 dBm
        POWER_8DBM = 4,             ///< 8 dBm
        POWER_11DBM = 5,            ///< 11 dBm
        POWER_14DBM = 6,            ///< 14 dBm
        POWER_17DBM = 7,            ///< 17 dBm
        POWER_20DBM = 8             ///< 20 dBm (maximum power)
    };

    // ===============================
    // DATA STRUCTURES
    // ===============================

    /**
     * @brief Message header structure for protocol communication
     */
    struct MessageHeader {
        uint8_t start_byte;         ///< Start delimiter (0xAA)
        uint8_t source_id;          ///< Source node ID
        uint8_t dest_id;            ///< Destination node ID
        uint8_t message_type;       ///< Message type from MessageType enum
        uint8_t sequence_num;       ///< Sequence number for ordering
        uint8_t payload_length;     ///< Payload size in bytes
        uint8_t flags;              ///< Protocol flags (ACK required, etc.)
        uint8_t checksum;           ///< Header checksum
    };

    /**
     * @brief Complete message structure
     */
    struct Message {
        MessageHeader header;                           ///< Message header
        uint8_t payload[MAX_PAYLOAD_SIZE];             ///< Message payload
        uint8_t payload_checksum;                       ///< Payload checksum
        uint32_t timestamp;                             ///< Timestamp when message was created
    };

    /**
     * @brief Transmission context for reliability tracking
     */
    struct TransmissionContext {
        uint8_t sequence_num;       ///< Sequence number being tracked
        uint8_t retry_count;        ///< Current retry attempt
        uint32_t last_send_time;    ///< Timestamp of last transmission
        uint32_t timeout_ms;        ///< Current timeout value
        bool waiting_for_ack;       ///< Flag indicating ACK is expected
    };

    /**
     * @brief Message handler function pointer type
     */
    typedef void (*MessageHandler)(const Message& message);

    /**
     * @brief Network node information
     */
    struct NodeInfo {
        uint8_t node_id;            ///< Node identifier
        uint32_t last_seen;         ///< Timestamp of last communication
        uint8_t signal_strength;    ///< Last known signal strength
        bool is_active;             ///< Node activity status
    };

    // ===============================
    // CONFIGURATION & CONTROL
    // ===============================

    /**
     * @brief Initialize the HC-12 communication module.
     * 
     * Sets up the HC-12 module with specified serial interface and communication parameters.
     * Initializes internal buffers, sequence numbers, and message handlers.
     * 
     * @param serial Reference to HardwareSerial interface (e.g. Serial1).
     * @param baud_rate Communication baud rate (default 9600).
     * @param node_id Local node identifier (1-255).
     * @param set_pin Pin number for HC-12 SET pin (for configuration mode).
     * @return true if initialization successful, false otherwise.
     */
    bool init(HardwareSerial& serial, uint32_t baud_rate = 9600, uint8_t node_id = 1, int set_pin = -1);

    /**
     * @brief Configure HC-12 module parameters.
     * 
     * Enters AT command mode to configure channel, power, and baud rate.
     * Automatically returns to transparent mode after configuration.
     * 
     * @param channel RF channel (1-127).
     * @param power_level Transmission power level.
     * @param baud_rate Serial communication baud rate.
     * @return true if configuration successful, false otherwise.
     */
    bool configure(uint8_t channel, PowerLevel power_level, uint32_t baud_rate = 9600);

    /**
     * @brief Reset and reinitialize the HC-12 module.
     * 
     * Performs a soft reset of the module and reinitializes all parameters.
     * Useful for recovery from communication failures.
     * 
     * @return true if reset successful, false otherwise.
     */
    bool reset();

    /**
     * @brief Enter AT command mode for configuration.
     * 
     * Pulls SET pin low to enter command mode. Module must be in this mode
     * to accept AT commands for configuration changes.
     * 
     * @return true if successfully entered command mode, false otherwise.
     */
    bool enter_command_mode();

    /**
     * @brief Exit AT command mode and return to transparent mode.
     * 
     * Pulls SET pin high to return to normal communication mode.
     * Required after configuration changes.
     * 
     * @return true if successfully exited command mode, false otherwise.
     */
    bool exit_command_mode();

    /**
     * @brief Test communication link with another node.
     * 
     * Sends a test message to specified node and waits for response.
     * Useful for link quality assessment and connectivity testing.
     * 
     * @param dest_id Target node identifier.
     * @param timeout_ms Timeout for response in milliseconds.
     * @return true if test successful, false otherwise.
     */
    bool test_link(uint8_t dest_id, uint32_t timeout_ms = 5000);

    // ===============================
    // MESSAGE TRANSMISSION
    // ===============================

    /**
     * @brief Send a message to another node.
     * 
     * Constructs a complete message with header and payload, then transmits
     * to the specified destination. Supports both reliable and unreliable transmission.
     * 
     * @param dest_id Destination node identifier.
     * @param msg_type Message type from MessageType enum.
     * @param payload Pointer to payload data.
     * @param payload_size Size of payload in bytes.
     * @param require_ack Whether to require acknowledgment.
     * @return COMM_SUCCESS if successful, error code otherwise.
     */
    CommStatus send_message(uint8_t dest_id, MessageType msg_type, 
                           const uint8_t* payload, uint8_t payload_size, 
                           bool require_ack = true);

    /**
     * @brief Send a string message to another node.
     * 
     * Convenience function for sending text messages. Automatically handles
     * string-to-byte conversion and null termination.
     * 
     * @param dest_id Destination node identifier.
     * @param msg_type Message type from MessageType enum.
     * @param message String message to send.
     * @param require_ack Whether to require acknowledgment.
     * @return COMM_SUCCESS if successful, error code otherwise.
     */
    CommStatus send_string(uint8_t dest_id, MessageType msg_type, 
                          const String& message, bool require_ack = true);

    /**
     * @brief Send acknowledgment message.
     * 
     * Sends an ACK message in response to a received message.
     * Used internally by the reliability system and can be called manually.
     * 
     * @param dest_id Destination node identifier.
     * @param sequence_num Sequence number being acknowledged.
     * @return COMM_SUCCESS if successful, error code otherwise.
     */
    CommStatus send_ack(uint8_t dest_id, uint8_t sequence_num);

    /**
     * @brief Send negative acknowledgment message.
     * 
     * Sends a NAK message indicating message was received but contained errors.
     * Triggers retransmission at the sender.
     * 
     * @param dest_id Destination node identifier.
     * @param sequence_num Sequence number being rejected.
     * @return COMM_SUCCESS if successful, error code otherwise.
     */
    CommStatus send_nak(uint8_t dest_id, uint8_t sequence_num);

    /**
     * @brief Broadcast message to all nodes.
     * 
     * Sends a message to all nodes in the network using broadcast addressing.
     * Broadcast messages typically don't require acknowledgment.
     * 
     * @param msg_type Message type from MessageType enum.
     * @param payload Pointer to payload data.
     * @param payload_size Size of payload in bytes.
     * @return COMM_SUCCESS if successful, error code otherwise.
     */
    CommStatus broadcast_message(MessageType msg_type, const uint8_t* payload, uint8_t payload_size);

    // ===============================
    // MESSAGE RECEPTION
    // ===============================

    /**
     * @brief Process incoming messages.
     * 
     * Reads available data from serial interface, parses incoming messages,
     * and dispatches them to appropriate handlers. Should be called frequently
     * in main loop to maintain real-time communication.
     * 
     * @return Number of messages processed.
     */
    uint8_t process_messages();

    /**
     * @brief Check if a message is available for reading.
     * 
     * Non-blocking check for pending messages in the receive buffer.
     * 
     * @return true if message available, false otherwise.
     */
    bool message_available();

    /**
     * @brief Read the next available message.
     * 
     * Retrieves the next message from the receive buffer. Message is removed
     * from buffer after reading.
     * 
     * @param message Reference to Message structure to populate.
     * @param timeout_ms Timeout for message availability in milliseconds.
     * @return COMM_SUCCESS if message read, error code otherwise.
     */
    CommStatus read_message(Message& message, uint32_t timeout_ms = 1000);

    /**
     * @brief Wait for acknowledgment of a sent message.
     * 
     * Blocks until ACK is received for specified sequence number or timeout occurs.
     * Used internally by reliable transmission but can be called manually.
     * 
     * @param sequence_num Sequence number to wait for.
     * @param timeout_ms Timeout in milliseconds.
     * @return true if ACK received, false if timeout or NAK received.
     */
    bool wait_for_ack(uint8_t sequence_num, uint32_t timeout_ms = ACK_TIMEOUT_MS);

    /**
     * @brief Check if received message is an acknowledgment.
     * 
     * Examines message type and payload to determine if it's an ACK message.
     * 
     * @param message Message to check.
     * @param sequence_num Reference to store acknowledged sequence number.
     * @return true if message is ACK, false otherwise.
     */
    bool is_ack_message(const Message& message, uint8_t& sequence_num);

    /**
     * @brief Check if received message is a negative acknowledgment.
     * 
     * Examines message type and payload to determine if it's a NAK message.
     * 
     * @param message Message to check.
     * @param sequence_num Reference to store rejected sequence number.
     * @return true if message is NAK, false otherwise.
     */
    bool is_nak_message(const Message& message, uint8_t& sequence_num);

    // ===============================
    // RELIABILITY & RETRANSMISSION
    // ===============================

    /**
     * @brief Handle message retransmission logic.
     * 
     * Manages retransmission of messages that haven't been acknowledged.
     * Implements exponential backoff and retry limits. Should be called
     * periodically to maintain reliability.
     */
    void handle_retransmissions();

    /**
     * @brief Set maximum number of retransmission attempts.
     * 
     * Configures the reliability system's retry behavior.
     * 
     * @param max_retries Maximum number of retry attempts (1-10).
     */
    void set_max_retries(uint8_t max_retries);

    /**
     * @brief Set acknowledgment timeout.
     * 
     * Configures how long to wait for ACK before retransmitting.
     * 
     * @param timeout_ms Timeout in milliseconds.
     */
    void set_ack_timeout(uint32_t timeout_ms);

    /**
     * @brief Calculate exponential backoff delay.
     * 
     * Calculates the delay before next retransmission attempt using
     * exponential backoff algorithm to reduce network congestion.
     * 
     * @param retry_count Current retry attempt number.
     * @return Delay in milliseconds.
     */
    uint32_t calculate_backoff_delay(uint8_t retry_count);

    // ===============================
    // MESSAGE ROUTING & MULTIPLEXING
    // ===============================

    /**
     * @brief Register a message handler for specific message type.
     * 
     * Associates a callback function with a message type. When messages
     * of the specified type are received, the handler will be called.
     * 
     * @param msg_type Message type to handle.
     * @param handler Callback function to process messages.
     * @return true if handler registered successfully, false otherwise.
     */
    bool register_message_handler(MessageType msg_type, MessageHandler handler);

    /**
     * @brief Unregister a message handler.
     * 
     * Removes the handler association for a specific message type.
     * 
     * @param msg_type Message type to unregister.
     * @return true if handler removed successfully, false otherwise.
     */
    bool unregister_message_handler(MessageType msg_type);

    /**
     * @brief Dispatch message to appropriate handler.
     * 
     * Routes incoming messages to registered handlers based on message type.
     * Called internally by message processing system.
     * 
     * @param message Message to dispatch.
     */
    void dispatch_message(const Message& message);

    /**
     * @brief Set default message handler for unregistered types.
     * 
     * Provides a fallback handler for messages that don't have specific
     * handlers registered.
     * 
     * @param handler Default callback function.
     */
    void set_default_handler(MessageHandler handler);

    // ===============================
    // NETWORK MANAGEMENT
    // ===============================

    /**
     * @brief Get information about active network nodes.
     * 
     * Returns list of known nodes and their status information.
     * 
     * @param nodes Array to populate with node information.
     * @param max_nodes Maximum number of nodes to return.
     * @return Number of active nodes found.
     */
    uint8_t get_network_nodes(NodeInfo* nodes, uint8_t max_nodes);

    /**
     * @brief Update node activity status.
     * 
     * Called when communication is received from a node to update
     * its last-seen timestamp and activity status.
     * 
     * @param node_id Node identifier.
     * @param signal_strength Signal strength indicator (0-255).
     */
    void update_node_status(uint8_t node_id, uint8_t signal_strength = 0);

    /**
     * @brief Check if a node is currently active.
     * 
     * Determines if a node has communicated recently and is considered active.
     * 
     * @param node_id Node identifier to check.
     * @param timeout_ms Timeout for considering node inactive.
     * @return true if node is active, false otherwise.
     */
    bool is_node_active(uint8_t node_id, uint32_t timeout_ms = 30000);

    /**
     * @brief Send heartbeat message to maintain network presence.
     * 
     * Sends a heartbeat message to indicate this node is active.
     * Should be called periodically to maintain network connectivity.
     * 
     * @return COMM_SUCCESS if heartbeat sent, error code otherwise.
     */
    CommStatus send_heartbeat();

    // ===============================
    // UTILITY FUNCTIONS
    // ===============================

    /**
     * @brief Calculate message checksum.
     * 
     * Computes checksum for message integrity verification.
     * 
     * @param data Pointer to data buffer.
     * @param length Length of data in bytes.
     * @return Calculated checksum value.
     */
    uint8_t calculate_checksum(const uint8_t* data, uint8_t length);

    /**
     * @brief Validate message integrity.
     * 
     * Verifies message header and payload checksums.
     * 
     * @param message Message to validate.
     * @return true if message is valid, false otherwise.
     */
    bool validate_message(const Message& message);

    /**
     * @brief Get current local node ID.
     * 
     * Returns the node identifier for this device.
     * 
     * @return Local node identifier.
     */
    uint8_t get_local_node_id();

    /**
     * @brief Get communication statistics.
     * 
     * Returns communication performance metrics.
     * 
     * @param messages_sent Reference to store sent message count.
     * @param messages_received Reference to store received message count.
     * @param transmission_errors Reference to store error count.
     */
    void get_stats(uint32_t& messages_sent, uint32_t& messages_received, uint32_t& transmission_errors);

    /**
     * @brief Reset communication statistics.
     * 
     * Clears all communication counters and statistics.
     */
    void reset_stats();

    /**
     * @brief Get next sequence number for outgoing messages.
     * 
     * Returns the next sequence number and increments internal counter.
     * 
     * @return Next sequence number (1-255, wraps around).
     */
    uint8_t get_next_sequence_number();

    /**
     * @brief Convert CommStatus to human-readable string.
     * 
     * Utility function for debugging and logging.
     * 
     * @param status Status code to convert.
     * @return String representation of status.
     */
    String status_to_string(CommStatus status);

} // namespace HC12
} // namespace Velma

#endif // HC12_H