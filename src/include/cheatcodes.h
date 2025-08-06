/**
 * @file cheatcodes.h
 * @brief Cheat Codes module for Velma and Blizzard system
 * @author Velma Development Team
 * @version 1.0
 * @date 2025
 * 
 * @details This module provides a robust cheat code system for handling developer commands,
 * operational mode switches, and parameter tuning. It supports multiple input methods,
 * various code formats, and implements safety mechanisms for critical operations.
 */

 #ifndef VELMA_CHEATCODES_H
 #define VELMA_CHEATCODES_H
 
 #include <stdint.h>
 #include <stdbool.h>
 #include <string.h>
 
 namespace Velma {
 namespace CheatCodes {
 
 // =============================================================================
 // CONSTANTS AND CONFIGURATION
 // =============================================================================
 
 /** @brief Maximum length of a cheat code string */
 static const uint8_t MAX_CODE_LENGTH = 32;
 
 /** @brief Maximum number of registered cheat codes */
 static const uint8_t MAX_CHEAT_CODES = 64;
 
 /** @brief Default lockout duration in milliseconds */
 static const uint32_t DEFAULT_LOCKOUT_MS = 5000;
 
 /** @brief Maximum failed attempts before extended lockout */
 static const uint8_t MAX_FAILED_ATTEMPTS = 3;
 
 /** @brief Extended lockout duration in milliseconds */
 static const uint32_t EXTENDED_LOCKOUT_MS = 30000;
 
 /** @brief Authentication timeout in milliseconds */
 static const uint32_t AUTH_TIMEOUT_MS = 10000;
 
 // =============================================================================
 // ENUMERATIONS
 // =============================================================================
 
 /**
  * @brief Cheat code input source types
  */
 enum class InputSource : uint8_t {
     KEYPAD_BLIZZARD = 0,    ///< Keypad input from Blizzard console
     KEYPAD_VELMA = 1,       ///< Keypad input from Velma handheld
     BUTTON_SEQUENCE = 2,    ///< Physical button sequence
     HC12_COMMUNICATION = 3, ///< HC-12 radio communication
     SERIAL_CONSOLE = 4      ///< Serial console input
 };
 
 /**
  * @brief Cheat code format types
  */
 enum class CodeFormat : uint8_t {
     SHORT_STRING = 0,       ///< Short string commands (e.g., "RESET")
     PASSWORD_STYLE = 1,     ///< Password-style codes (e.g., "UPUPDOWNDOWN")
     NUMERIC = 2,            ///< Numeric codes (e.g., 1234)
     MIXED_ALPHANUMERIC = 3  ///< Mixed alphanumeric/symbolic codes
 };
 
 /**
  * @brief Security level for cheat codes
  */
 enum class SecurityLevel : uint8_t {
     PUBLIC = 0,             ///< No authentication required
     DEVELOPER = 1,          ///< Developer authentication required
     ADMIN = 2,              ///< Admin authentication required
     CRITICAL = 3            ///< Multi-step confirmation required
 };
 
 /**
  * @brief Cheat code execution result
  */
 enum class ExecutionResult : uint8_t {
     SUCCESS = 0,            ///< Command executed successfully
     INVALID_CODE = 1,       ///< Invalid or unrecognized code
     AUTHENTICATION_FAILED = 2, ///< Authentication failed
     LOCKOUT_ACTIVE = 3,     ///< System is in lockout state
     CONFIRMATION_PENDING = 4,  ///< Awaiting confirmation
     PARAMETER_ERROR = 5,    ///< Invalid parameters
     SYSTEM_ERROR = 6,       ///< System error during execution
     EXPIRED_CODE = 7        ///< Code has expired
 };
 
 /**
  * @brief Output destination for responses
  */
 enum class OutputDestination : uint8_t {
     SERIAL_CONSOLE = 0,     ///< Serial console output
     HC12_RADIO = 1,         ///< HC-12 radio response
     BLIZZARD_DISPLAY = 2,   ///< Blizzard screen display
     VELMA_DISPLAY = 3,      ///< Velma screen display
     ALL_OUTPUTS = 4         ///< Send to all available outputs
 };
 
 // =============================================================================
 // DATA STRUCTURES
 // =============================================================================
 
 /**
  * @brief Structure representing a cheat code input
  */
 struct CodeInput {
     char code[MAX_CODE_LENGTH + 1];  ///< Null-terminated code string
     InputSource source;              ///< Source of the input
     CodeFormat format;               ///< Format of the code
     uint32_t timestamp;              ///< Timestamp of input reception
     uint8_t source_id;               ///< Source device ID (for HC-12)
 };
 
 /**
  * @brief Function pointer type for cheat code actions
  */
 typedef ExecutionResult (*CheatCodeAction)(const char* parameters);
 
 /**
  * @brief Structure representing a registered cheat code
  */
 struct CheatCodeEntry {
     char code[MAX_CODE_LENGTH + 1];  ///< The cheat code string
     CheatCodeAction action;          ///< Function to execute
     SecurityLevel security;          ///< Required security level
     CodeFormat format;               ///< Expected code format
     uint32_t expiry_timestamp;       ///< Code expiration (0 = never expires)
     bool enabled;                    ///< Whether code is currently enabled
     const char* description;         ///< Human-readable description
 };
 
 /**
  * @brief Structure for authentication state
  */
 struct AuthenticationState {
     bool authenticated;              ///< Current authentication status
     SecurityLevel current_level;     ///< Current authentication level
     uint32_t auth_timestamp;         ///< Time of last authentication
     uint8_t failed_attempts;         ///< Count of failed attempts
     uint32_t lockout_until;          ///< Lockout expiration timestamp
     bool confirmation_pending;       ///< Whether confirmation is pending
     char pending_code[MAX_CODE_LENGTH + 1]; ///< Code awaiting confirmation
 };
 
 /**
  * @brief Structure for system statistics
  */
 struct CheatCodeStats {
     uint32_t total_attempts;         ///< Total code attempts
     uint32_t successful_executions;  ///< Successful executions
     uint32_t failed_attempts;        ///< Failed attempts
     uint32_t lockout_events;         ///< Number of lockout events
     uint32_t last_execution_time;    ///< Timestamp of last execution
 };
 
 // =============================================================================
 // CLASS DEFINITION
 // =============================================================================
 
 /**
  * @brief Main cheat codes manager class
  */
 class CheatCodeManager {
 private:
     CheatCodeEntry codes_[MAX_CHEAT_CODES];     ///< Registered cheat codes
     uint8_t code_count_;                        ///< Number of registered codes
     AuthenticationState auth_state_;            ///< Authentication state
     CheatCodeStats stats_;                      ///< System statistics
     
     /**
      * @brief Check if system is currently locked out
      * @return true if locked out, false otherwise
      */
     bool isLockedOut() const;
     
     /**
      * @brief Validate code format
      * @param code The code to validate
      * @param expected_format Expected format
      * @return true if format is valid
      */
     bool validateCodeFormat(const char* code, CodeFormat expected_format) const;
     
     /**
      * @brief Check if code has expired
      * @param entry The cheat code entry to check
      * @return true if expired, false otherwise
      */
     bool isCodeExpired(const CheatCodeEntry& entry) const;
     
     /**
      * @brief Update authentication state after attempt
      * @param success Whether authentication was successful
      */
     void updateAuthenticationState(bool success);
     
     /**
      * @brief Log cheat code usage to SD card
      * @param code The executed code
      * @param result Execution result
      * @param source Input source
      */
     void logCodeUsage(const char* code, ExecutionResult result, InputSource source);
 
 public:
     /**
      * @brief Constructor
      */
     CheatCodeManager();
     
     /**
      * @brief Destructor
      */
     ~CheatCodeManager();
     
     // =========================================================================
     // INITIALIZATION AND CONFIGURATION
     // =========================================================================
     
     /**
      * @brief Initialize the cheat codes system
      * @return true if initialization successful
      */
     bool initialize();
     
     /**
      * @brief Register a new cheat code
      * @param code The cheat code string
      * @param action Function to execute when code is entered
      * @param security Security level required
      * @param format Expected code format
      * @param description Human-readable description
      * @param expiry_timestamp Expiration timestamp (0 = never expires)
      * @return true if registration successful
      */
     bool registerCheatCode(const char* code, 
                           CheatCodeAction action,
                           SecurityLevel security = SecurityLevel::PUBLIC,
                           CodeFormat format = CodeFormat::SHORT_STRING,
                           const char* description = nullptr,
                           uint32_t expiry_timestamp = 0);
     
     /**
      * @brief Unregister a cheat code
      * @param code The cheat code to remove
      * @return true if successfully removed
      */
     bool unregisterCheatCode(const char* code);
     
     /**
      * @brief Enable or disable a cheat code
      * @param code The cheat code to modify
      * @param enabled New enabled state
      * @return true if successful
      */
     bool setCodeEnabled(const char* code, bool enabled);
     
     // =========================================================================
     // INPUT PROCESSING
     // =========================================================================
     
     /**
      * @brief Process cheat code input
      * @param input The code input structure
      * @return Execution result
      */
     ExecutionResult processCodeInput(const CodeInput& input);
     
     /**
      * @brief Parse keypad input into cheat code
      * @param keypad_data Raw keypad data
      * @param source Input source
      * @return Parsed CodeInput structure
      */
     CodeInput parseKeypardInput(const char* keypad_data, InputSource source);
     
     /**
      * @brief Parse button sequence into cheat code
      * @param button_sequence Button sequence data
      * @return Parsed CodeInput structure
      */
     CodeInput parseButtonSequence(const uint8_t* button_sequence, uint8_t length);
     
     /**
      * @brief Parse HC-12 message into cheat code
      * @param message HC-12 message data
      * @param source_id Source device ID
      * @return Parsed CodeInput structure
      */
     CodeInput parseHC12Message(const char* message, uint8_t source_id);
     
     // =========================================================================
     // AUTHENTICATION AND SECURITY
     // =========================================================================
     
     /**
      * @brief Authenticate user for given security level
      * @param level Required security level
      * @param credentials Authentication credentials
      * @return true if authentication successful
      */
     bool authenticate(SecurityLevel level, const char* credentials);
     
     /**
      * @brief Confirm pending critical operation
      * @param confirmation_code Confirmation code
      * @return true if confirmation successful
      */
     bool confirmPendingOperation(const char* confirmation_code);
     
     /**
      * @brief Clear authentication state
      */
     void clearAuthentication();
     
     /**
      * @brief Check if authentication is valid for security level
      * @param level Required security level
      * @return true if authenticated for this level
      */
     bool isAuthenticated(SecurityLevel level) const;
     
     /**
      * @brief Force lockout for specified duration
      * @param duration_ms Lockout duration in milliseconds
      */
     void forceLockout(uint32_t duration_ms = DEFAULT_LOCKOUT_MS);
     
     /**
      * @brief Clear lockout state (admin override)
      */
     void clearLockout();
     
     // =========================================================================
     // OUTPUT AND FEEDBACK
     // =========================================================================
     
     /**
      * @brief Send response message to specified destination
      * @param message Message to send
      * @param destination Output destination
      * @param source_id Source device ID (for HC-12 responses)
      */
     void sendResponse(const char* message, 
                      OutputDestination destination,
                      uint8_t source_id = 0);
     
     /**
      * @brief Send execution result message
      * @param result Execution result
      * @param destination Output destination
      * @param additional_info Additional information string
      */
     void sendExecutionResult(ExecutionResult result,
                            OutputDestination destination,
                            const char* additional_info = nullptr);
     
     // =========================================================================
     // SYSTEM MANAGEMENT
     // =========================================================================
     
     /**
      * @brief Get system statistics
      * @return Current statistics structure
      */
     CheatCodeStats getStatistics() const;
     
     /**
      * @brief Reset system statistics
      */
     void resetStatistics();
     
     /**
      * @brief Get list of registered codes (for debugging)
      * @param codes Output array
      * @param max_codes Maximum number of codes to return
      * @return Number of codes returned
      */
     uint8_t getRegisteredCodes(CheatCodeEntry* codes, uint8_t max_codes) const;
     
     /**
      * @brief Update system (call from main loop)
      */
     void update();
     
     /**
      * @brief Save configuration to EEPROM
      * @return true if successful
      */
     bool saveConfiguration();
     
     /**
      * @brief Load configuration from EEPROM
      * @return true if successful
      */
     bool loadConfiguration();
 };
 
 // =============================================================================
 // BUILT-IN CHEAT CODE ACTIONS
 // =============================================================================
 
 namespace BuiltInActions {
     /**
      * @brief Reset system
      * @param parameters Command parameters
      * @return Execution result
      */
     ExecutionResult resetSystem(const char* parameters);
     
     /**
      * @brief Calibrate gyroscope
      * @param parameters Command parameters
      * @return Execution result
      */
     ExecutionResult calibrateGyro(const char* parameters);
     
     /**
      * @brief Clear EEPROM
      * @param parameters Command parameters
      * @return Execution result
      */
     ExecutionResult clearEEPROM(const char* parameters);
     
     /**
      * @brief Reboot system
      * @param parameters Command parameters
      * @return Execution result
      */
     ExecutionResult rebootSystem(const char* parameters);
     
     /**
      * @brief Enable manual override mode
      * @param parameters Command parameters
      * @return Execution result
      */
     ExecutionResult enableManualOverride(const char* parameters);
     
     /**
      * @brief Disarm all motors
      * @param parameters Command parameters
      * @return Execution result
      */
     ExecutionResult disarmMotors(const char* parameters);
     
     /**
      * @brief Enter test mode
      * @param parameters Command parameters
      * @return Execution result
      */
     ExecutionResult enterTestMode(const char* parameters);
     
     /**
      * @brief Adjust PID parameters
      * @param parameters Command parameters (format: "axis:p:i:d")
      * @return Execution result
      */
     ExecutionResult adjustPID(const char* parameters);
     
     /**
      * @brief Set sensor calibration constants
      * @param parameters Command parameters
      * @return Execution result
      */
     ExecutionResult setSensorCalibration(const char* parameters);
     
     /**
      * @brief Show system status
      * @param parameters Command parameters
      * @return Execution result
      */
     ExecutionResult showSystemStatus(const char* parameters);
 }
 
 // =============================================================================
 // UTILITY FUNCTIONS
 // =============================================================================
 
 /**
  * @brief Get current system timestamp
  * @return Current timestamp in milliseconds
  */
 uint32_t getCurrentTimestamp();
 
 /**
  * @brief Convert execution result to string
  * @param result Execution result
  * @return String representation
  */
 const char* executionResultToString(ExecutionResult result);
 
 /**
  * @brief Convert security level to string
  * @param level Security level
  * @return String representation
  */
 const char* securityLevelToString(SecurityLevel level);
 
 /**
  * @brief Convert input source to string
  * @param source Input source
  * @return String representation
  */
 const char* inputSourceToString(InputSource source);
 
 } // namespace CheatCodes
 } // namespace Velma
 
 #endif // VELMA_CHEATCODES_H