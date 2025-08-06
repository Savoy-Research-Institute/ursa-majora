#include "radio.h"
#include <PPMReader.h> // Include your actual PPM library header

namespace Velma {
namespace Radio {

    // Static variables for radio state management
    static int transistorPin = -1;
    static bool radioIsOn = false;
    static RadioStatus currentStatus = RADIO_OFF;
    static unsigned long failsafeTimeoutMs = 1000;
    static unsigned long lastValidSignalTime = 0;
    static unsigned long previousMillis = 0;
    static const unsigned long readInterval = 20; // 50Hz reading rate

    // Channel storage
    static int channelValues[CHANNEL_COUNT];
    static int channelCenters[CHANNEL_COUNT];
    static int channelRanges[CHANNEL_COUNT];

    // PPM reader object (initialized in init function)
    static PPMReader* ppm = nullptr;

    // Forward declaration of failsafe function
    void disarm_due_to_failsafe();

    /**
     * @brief Initialize channel calibration to default values.
     */
    void initialize_calibration() {
        // Set default center and range values for all channels
        for (int i = 0; i < CHANNEL_COUNT; i++) {
            if (i == THROTTLE) {
                channelCenters[i] = 1000; // Throttle center at minimum
                channelRanges[i] = 1000;  // Full range 0-2000
            } else {
                channelCenters[i] = 1500; // Standard center for other channels
                channelRanges[i] = 500;   // ±500µs range
            }
            channelValues[i] = 0; // Initialize to failsafe value
        }
    }

    void init(int transistor_pin, int ppm_pin, unsigned long failsafe_timeout_ms) {
        transistorPin = transistor_pin;
        failsafeTimeoutMs = failsafe_timeout_ms;
        
        // Initialize hardware pins
        pinMode(transistorPin, OUTPUT);
        switch_off();
        
        // Initialize PPM reader
        if (ppm != nullptr) {
            delete ppm;
        }
        ppm = new PPMReader(ppm_pin);
        
        // Initialize channel calibration
        initialize_calibration();
        
        // Reset timing variables
        lastValidSignalTime = 0;
        previousMillis = 0;
        currentStatus = RADIO_OFF;
    }

    void switch_on() {
        if (!radioIsOn && transistorPin != -1) {
            digitalWrite(transistorPin, HIGH);
            radioIsOn = true;
            currentStatus = RADIO_ON;
            lastValidSignalTime = millis(); // Reset failsafe timer
        }
    }

    void switch_off() {
        if (radioIsOn && transistorPin != -1) {
            digitalWrite(transistorPin, LOW);
            radioIsOn = false;
            currentStatus = RADIO_OFF;
            
            // Set all channels to failsafe values
            for (int i = 0; i < CHANNEL_COUNT; i++) {
                channelValues[i] = 0;
            }
        }
    }

    bool is_on() {
        return radioIsOn;
    }

    RadioStatus get_status() {
        return currentStatus;
    }

    bool is_failsafe() {
        return currentStatus == RADIO_FAILSAFE;
    }

    unsigned long get_time_since_last_signal() {
        if (lastValidSignalTime == 0) return ULONG_MAX;
        return millis() - lastValidSignalTime;
    }

    void read_all_channels() {
        if (!radioIsOn || ppm == nullptr) return;

        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= readInterval) {
            previousMillis = currentMillis;

            // Check if we have valid PPM data
            bool validSignalReceived = false;
            
            // Read all channels
            for (int i = 0; i < CHANNEL_COUNT; i++) {
                int newValue = ppm->latestValidChannelValue(i, channelCenters[i]);
                
                // Validate channel value (typical PPM range: 800-2200µs)
                if (newValue >= 800 && newValue <= 2200) {
                    channelValues[i] = newValue;
                    validSignalReceived = true;
                } else {
                    // Invalid value, keep previous value or set to center
                    if (channelValues[i] == 0) {
                        channelValues[i] = channelCenters[i];
                    }
                }
            }

            // Update failsafe status
            if (validSignalReceived) {
                lastValidSignalTime = currentMillis;
                if (currentStatus == RADIO_FAILSAFE) {
                    currentStatus = RADIO_ON; // Recovered from failsafe
                }
            } else {
                // Check for failsafe timeout
                if (currentMillis - lastValidSignalTime > failsafeTimeoutMs) {
                    if (currentStatus != RADIO_FAILSAFE) {
                        currentStatus = RADIO_FAILSAFE;
                        
                        // Set all channels to failsafe values
                        for (int i = 0; i < CHANNEL_COUNT; i++) {
                            channelValues[i] = 0;
                        }
                        
                        // Call system disarm function
                        disarm_due_to_failsafe();
                    }
                }
            }

            // Optional debug output (uncomment for debugging)
            /*
            Serial.print("Status: "); Serial.print(currentStatus); Serial.print(" | ");
            Serial.print("T: "); Serial.print(channelValues[THROTTLE]); Serial.print(" ");
            Serial.print("R: "); Serial.print(channelValues[ROLL]); Serial.print(" ");
            Serial.print("P: "); Serial.print(channelValues[PITCH]); Serial.print(" ");
            Serial.print("Y: "); Serial.print(channelValues[YAW]); Serial.print(" ");
            Serial.print("S1: "); Serial.print(channelValues[SWITCH3WAY_1]); Serial.print(" ");
            Serial.print("P1: "); Serial.print(channelValues[POT1]); Serial.print(" ");
            Serial.print("S2: "); Serial.print(channelValues[SWITCH3WAY_2]); Serial.print(" ");
            Serial.print("P2: "); Serial.print(channelValues[POT2]); Serial.println();
            */
        }
    }

    int get_channel(ChannelId channel) {
        if (channel >= CHANNEL_COUNT || !radioIsOn) return 0;
        if (currentStatus == RADIO_FAILSAFE) return 0;
        return channelValues[channel];
    }

    float get_channel_normalized(ChannelId channel) {
        if (channel >= CHANNEL_COUNT || !radioIsOn) return 0.0;
        if (currentStatus == RADIO_FAILSAFE) return 0.0;
        
        int rawValue = channelValues[channel];
        int center = channelCenters[channel];
        int range = channelRanges[channel];
        
        // Special handling for throttle (0-100% instead of -100% to +100%)
        if (channel == THROTTLE) {
            // Throttle: 1000µs = 0%, 2000µs = 100%
            float normalized = ((float)(rawValue - 1000) / 1000.0) * 100.0;
            return constrain(normalized, 0.0, 100.0);
        } else {
            // Other channels: center ± range mapped to ±100%
            float normalized = ((float)(rawValue - center) / (float)range) * 100.0;
            return constrain(normalized, -100.0, 100.0);
        }
    }

    int read_throttle() {
        return get_channel(THROTTLE);
    }

    void set_channel_calibration(ChannelId channel, int center_us, int range_us) {
        if (channel >= CHANNEL_COUNT) return;
        
        channelCenters[channel] = center_us;
        channelRanges[channel] = range_us;
    }

    void reset_calibration() {
        initialize_calibration();
    }

    /**
     * @brief Placeholder function for system disarming due to failsafe.
     * This function should be implemented elsewhere in the Velma system
     * to safely disarm motors and enter safe mode.
     */
    void disarm_due_to_failsafe() {
        // TODO: Implement actual disarm logic
        // This should:
        // 1. Immediately stop all motors
        // 2. Set system to safe/disarmed state
        // 3. Log failsafe event
        // 4. Possibly trigger emergency procedures
        
        // Example placeholder:
        Serial.println("FAILSAFE TRIGGERED - SYSTEM DISARMED");
        
        // Call actual disarm function from flight controller
        // FlightController::emergency_disarm();
    }

} // namespace Radio
} // namespace Velma