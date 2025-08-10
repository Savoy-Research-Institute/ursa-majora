#ifndef COMMUNICATION_INTERFACE_H
#define COMMUNICATION_INTERFACE_H

#include <Arduino.h>
#include <SoftwareSerial.h>

// Communication States
enum class CommState {
    DISABLED,
    INITIALIZING,
    READY,
    TRANSMITTING,
    RECEIVING,
    ERROR
};

// Communication Protocols
enum class CommProtocol {
    HC12,
    XBEE,
    LORA,
    WIFI,
    BLUETOOTH,
    CUSTOM
};

// HC-12 Specific Message Types
enum class HC12MessageType : uint8_t {
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

// HC-12 Constants
static const uint8_t HC12_MAX_PAYLOAD_SIZE = 200;       ///< Maximum payload size in bytes
static const uint8_t HC12_MAX_RETRIES = 3;              ///< Maximum number of retransmission attempts
static const uint16_t HC12_ACK_TIMEOUT_MS = 1000;       ///< ACK timeout in milliseconds
static const uint16_t HC12_BASE_BACKOFF_MS = 100;       ///< Base backoff time for exponential backoff
static const uint8_t HC12_HEADER_SIZE = 8;              ///< Protocol header size in bytes
static const uint8_t HC12_MAX_NODES = 16;               ///< Maximum number of nodes in network

// HC-12 Communication Status Codes
enum class HC12CommStatus : uint8_t {
    COMM_SUCCESS = 0x00,        ///< Operation successful
    COMM_TIMEOUT = 0x01,        ///< Timeout occurred
    COMM_CHECKSUM_ERROR = 0x02, ///< Checksum validation failed
    COMM_BUFFER_FULL = 0x03,    ///< Buffer overflow
    COMM_INVALID_FORMAT = 0x04, ///< Invalid message format
    COMM_NO_ACK = 0x05,         ///< No acknowledgment received
    COMM_HARDWARE_ERROR = 0x06  ///< Hardware communication error
};

// HC-12 Power Levels
enum class HC12PowerLevel : uint8_t {
    POWER_MINUS_1DBM = 1,       ///< -1 dBm (minimum power)
    POWER_2DBM = 2,             ///< 2 dBm
    POWER_5DBM = 3,             ///< 5 dBm
    POWER_8DBM = 4,             ///< 8 dBm
    POWER_11DBM = 5,            ///< 11 dBm
    POWER_14DBM = 6,            ///< 14 dBm
    POWER_17DBM = 7,            ///< 17 dBm
    POWER_20DBM = 8             ///< 20 dBm (maximum power)
};

// HC-12 Message Header Structure
struct HC12MessageHeader {
    uint8_t start_byte;         ///< Start delimiter (0xAA)
    uint8_t source_id;          ///< Source node ID
    uint8_t dest_id;            ///< Destination node ID
    uint8_t message_type;       ///< Message type from HC12MessageType enum
    uint8_t sequence_num;       ///< Sequence number for ordering
    uint8_t payload_length;     ///< Payload size in bytes
    uint8_t flags;              ///< Protocol flags (ACK required, etc.)
    uint8_t checksum;           ///< Header checksum
};

// HC-12 Complete Message Structure
struct HC12Message {
    HC12MessageHeader header;                           ///< Message header
    uint8_t payload[200];                              ///< Message payload (MAX_PAYLOAD_SIZE)
    uint8_t payload_checksum;                          ///< Payload checksum
    uint32_t timestamp;                                ///< Timestamp when message was created
};

// HC-12 Transmission Context
struct HC12TransmissionContext {
    uint8_t sequence_num;       ///< Sequence number being tracked
    uint8_t retry_count;        ///< Current retry attempt
    uint32_t last_send_time;    ///< Timestamp of last transmission
    bool waiting_for_ack;       ///< Flag indicating ACK is expected
};

// HC-12 Node Information
struct HC12NodeInfo {
    uint8_t node_id;            ///< Node identifier
    uint32_t last_seen;         ///< Timestamp of last communication
    uint8_t signal_strength;    ///< Last known signal strength
    bool is_active;             ///< Node activity status
};

// Communication Data Structure
struct CommData {
    String data;
    uint32_t timestamp;
    uint8_t source_id;
    uint8_t destination_id;
    bool is_valid;
    bool is_acknowledged;
};

// Communication Configuration
struct CommConfig {
    CommProtocol protocol;
    uint16_t baud_rate;
    uint8_t tx_pin;
    uint8_t rx_pin;
    uint8_t channel;
    uint8_t power_level;
    bool enable_encryption;
    bool enable_acknowledgment;
    
    // HC-12 specific configuration
    uint8_t max_payload_size;
    uint8_t max_retries;
    uint16_t ack_timeout_ms;
    uint16_t base_backoff_ms;
    uint8_t max_nodes;
};

// Communication Error Information
struct CommError {
    bool transmission_error;
    bool reception_error;
    bool timeout_error;
    bool protocol_error;
    bool hardware_error;
    String error_message;
};

class CommunicationInterface {
private:
    // Hardware Configuration
    CommConfig config;
    SoftwareSerial* serial;
    
    // State Management
    CommState currentState;
    CommData lastReceived;
    CommData lastTransmitted;
    CommError errorInfo;
    
    // Buffer Management
    String receiveBuffer;
    String transmitBuffer;
    uint16_t bufferSize;
    
    // Timing
    unsigned long lastTransmitTime;
    unsigned long lastReceiveTime;
    uint32_t timeout_ms;
    
    // Statistics
    uint32_t packetsTransmitted;
    uint32_t packetsReceived;
    uint32_t transmissionErrors;
    uint32_t receptionErrors;
    
    // HC-12 specific members
    uint8_t local_node_id;
    uint8_t next_sequence_number;
    HC12TransmissionContext transmission_contexts[16];
    HC12NodeInfo network_nodes[16];
    uint8_t active_nodes_count;
    
    // Private Methods
    bool initializeHardware();
    bool configureProtocol();
    void processReceivedData();
    void processTransmitData();
    bool validateData(const String& data);
    void updateErrorInfo();
    void updateStatistics(bool success, bool isTransmit);
    
    // HC-12 specific private methods
    bool enterHC12CommandMode();
    bool exitHC12CommandMode();
    bool configureHC12(uint8_t channel, HC12PowerLevel power_level, uint32_t baud_rate);
    uint8_t calculateHC12Checksum(const uint8_t* data, uint8_t length);
    bool validateHC12Message(const HC12Message& message);
    bool waitForHC12Ack(uint8_t sequence_num, uint32_t timeout_ms);
    void handleHC12Retransmissions();
    uint8_t getNextHC12SequenceNumber();
    
public:
    CommunicationInterface();
    ~CommunicationInterface();
    
    // Initialization and Control
    bool initialize();
    void setConfiguration(const CommConfig& cfg);
    void enable();
    void disable();
    bool isEnabled() const;
    void reset();
    
    // Configuration
    void setProtocol(CommProtocol protocol);
    void setBaudRate(uint16_t baud);
    void setPins(uint8_t tx_pin, uint8_t rx_pin);
    void setChannel(uint8_t channel);
    void setPowerLevel(uint8_t power);
    void setTimeout(uint32_t timeout_ms);
    
    // Data Transmission
    bool transmit(const String& data);
    bool transmitTo(uint8_t destination, const String& data);
    bool transmitWithAck(const String& data);
    bool broadcast(const String& data);
    
    // HC-12 specific transmission methods
    bool sendHC12Message(uint8_t dest_id, HC12MessageType msg_type, 
                        const uint8_t* payload, uint8_t payload_size, 
                        bool require_ack = true);
    bool sendHC12String(uint8_t dest_id, HC12MessageType msg_type, 
                       const String& message, bool require_ack = true);
    bool sendHC12Ack(uint8_t dest_id, uint8_t sequence_num);
    bool sendHC12Nak(uint8_t dest_id, uint8_t sequence_num);
    bool broadcastHC12Message(HC12MessageType msg_type, const uint8_t* payload, uint8_t payload_size);
    
    // Additional HC-12 methods from hc12.h
    bool testHC12Link(uint8_t dest_id, uint32_t timeout_ms = 5000);
    bool enterHC12CommandMode();
    bool exitHC12CommandMode();
    uint8_t getNextHC12SequenceNumber();
    uint8_t calculateHC12Checksum(const uint8_t* data, uint8_t length);
    bool validateHC12Message(const HC12Message& message);
    bool waitForHC12Ack(uint8_t sequence_num, uint32_t timeout_ms);
    void handleHC12Retransmissions();
    uint8_t getHC12LocalNodeId() const;
    void getHC12Stats(uint32_t& messages_sent, uint32_t& messages_received, uint32_t& transmission_errors);
    void resetHC12Stats();
    uint8_t getHC12NetworkNodes(HC12NodeInfo* nodes, uint8_t max_nodes);
    void updateHC12NodeStatus(uint8_t node_id, uint8_t signal_strength = 0);
    bool isHC12NodeActive(uint8_t node_id, uint32_t timeout_ms = 30000);
    bool sendHC12Heartbeat();
    String HC12StatusToString(uint8_t status);
    
    // Data Reception
    bool receive();
    String getReceivedData() const { return lastReceived.data; }
    bool hasReceivedData() const;
    void clearReceivedData();
    bool waitForData(uint32_t timeout_ms);
    
    // HC-12 specific reception methods
    uint8_t processHC12Messages();
    bool HC12MessageAvailable();
    bool readHC12Message(HC12Message& message, uint32_t timeout_ms = 1000);
    bool isHC12AckMessage(const HC12Message& message, uint8_t& sequence_num);
    bool isHC12NakMessage(const HC12Message& message, uint8_t& sequence_num);
    
    // Buffer Management
    void setBufferSize(uint16_t size);
    void clearBuffers();
    uint16_t getBufferUsage() const;
    bool isBufferFull() const;
    
    // Status Information
    CommState getState() const { return currentState; }
    bool isReady() const { return currentState == CommState::READY; }
    bool isTransmitting() const { return currentState == CommState::TRANSMITTING; }
    bool isReceiving() const { return currentState == CommState::RECEIVING; }
    
    // Error Handling
    bool hasError() const;
    CommError getError() const { return errorInfo; }
    void clearErrors();
    
    // Diagnostics and Testing
    bool performSelfTest();
    void getDiagnostics(String& diagnostics);
    void getStatistics(String& stats);
    void printStatus();
    
    // Advanced Configuration
    void enableEncryption(bool enable);
    void setEncryptionKey(const String& key);
    void enableAcknowledgment(bool enable);
    void setRetryCount(uint8_t count);
    void setPacketSize(uint16_t size);
    
    // Protocol-specific Configuration
    void setHC12Channel(uint8_t channel);
    void setHC12Power(uint8_t power);
    void setHC12MaxRetries(uint8_t max_retries);
    void setHC12AckTimeout(uint32_t timeout_ms);
    void setXBeeAddress(uint16_t address);
    void setLoRaFrequency(uint32_t frequency);
    void setWiFiCredentials(const String& ssid, const String& password);
    
    // Utility Methods
    String protocolToString(CommProtocol protocol);
    CommProtocol stringToProtocol(const String& protocol);
    bool validateAddress(uint8_t address);
    void printPacketInfo(const CommData& packet);
    
    // HC-12 specific utility methods
    uint8_t getHC12LocalNodeId() const;
    void getHC12Stats(uint32_t& messages_sent, uint32_t& messages_received, uint32_t& transmission_errors);
    void resetHC12Stats();
    uint8_t getHC12NetworkNodes(HC12NodeInfo* nodes, uint8_t max_nodes);
    void updateHC12NodeStatus(uint8_t node_id, uint8_t signal_strength = 0);
    bool isHC12NodeActive(uint8_t node_id, uint32_t timeout_ms = 30000);
    bool sendHC12Heartbeat();
    String HC12StatusToString(uint8_t status);
    uint32_t calculateBackoffDelay(uint8_t retry_count);
    
    // Message Handler Management
    typedef void (*MessageHandler)(const HC12Message& message);
    bool registerMessageHandler(HC12MessageType msg_type, MessageHandler handler);
    bool unregisterMessageHandler(HC12MessageType msg_type);
    void setDefaultHandler(MessageHandler handler);
    void dispatchMessage(const HC12Message& message);
    
    // Callback Functions
    void onDataReceived(const String& data);
    void onDataTransmitted(const String& data);
    void onError(const String& error);
    void onStateChanged(CommState oldState, CommState newState);
};

#endif // COMMUNICATION_INTERFACE_H 