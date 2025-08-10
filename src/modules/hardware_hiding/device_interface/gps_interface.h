#ifndef GPS_INTERFACE_H
#define GPS_INTERFACE_H

#include <Arduino.h>
#include <TinyGPSPlus.h>

// GPS States
enum class GPSState {
    OFF,
    INITIALIZING,
    SEARCHING,
    FIX_2D,
    FIX_3D,
    ERROR
};

// GPS Configuration
enum class GPSBaudRate {
    BAUD_9600 = 9600,
    BAUD_19200 = 19200,
    BAUD_38400 = 38400,
    BAUD_57600 = 57600,
    BAUD_115200 = 115200
};

// GPS Data Structure
struct GPSData {
    double latitude;
    double longitude;
    double altitude;
    double speed;
    double course;
    uint8_t satellites;
    uint8_t fix_quality;
    uint32_t timestamp;
    bool is_valid;
};

// GPS Status Structure
struct GPSStatus {
    bool has_fix;
    uint8_t satellites;
    double hdop;
    double altitude;
    double speed;
    uint8_t fix_quality;
};

// GPS Configuration
struct GPSConfig {
    GPSBaudRate baud_rate;
    uint8_t navigation_rate;
    bool enable_eco_mode;
    bool enable_assistnow;
    uint8_t update_rate_hz;
};

// GPS Error Information
struct GPSError {
    bool communication_error;
    bool timeout_error;
    bool fix_lost_error;
    bool antenna_error;
    String error_message;
};

class GPSInterface {
private:
    // Hardware Configuration
    HardwareSerial* gpsSerial;
    uint8_t transistor_pin;
    GPSBaudRate baud_rate;
    
    // State Management
    GPSState currentState;
    GPSData currentData;
    GPSStatus currentStatus;
    GPSConfig config;
    GPSError errorInfo;
    
    // GPS Object
    TinyGPSPlus gps;
    
    // Timing
    uint32_t lastValidUpdate;
    uint32_t lastReadTime;
    
    // Private Methods
    bool initializeHardware();
    bool configureGPS();
    void readGPSData();
    void processGPSData();
    bool validateData();
    void updateErrorInfo();
    void sendUBXCommand(const uint8_t* command, uint8_t length);
    
public:
    GPSInterface(HardwareSerial& serial, uint8_t pin = -1);
    ~GPSInterface();
    
    // Initialization and Control
    bool initialize();
    void powerOn();
    void powerOff();
    bool isPowered() const;
    void reset();
    
    // Configuration
    void setBaudRate(GPSBaudRate baud);
    void setNavigationRate(uint8_t rate_hz);
    void setUpdateRate(uint8_t rate_hz);
    void setTransistorPin(uint8_t pin);
    
    // Data Access
    bool read();
    GPSData getData() const { return currentData; }
    GPSStatus getStatus() const { return currentStatus; }
    double getLatitude() const { return currentData.latitude; }
    double getLongitude() const { return currentData.longitude; }
    double getAltitude() const { return currentData.altitude; }
    double getSpeed() const { return currentData.speed; }
    double getCourse() const { return currentData.course; }
    uint8_t getSatellites() const { return currentData.satellites; }
    uint8_t getFixQuality() const { return currentData.fix_quality; }
    
    // Status Information
    bool hasFix() const { return currentStatus.has_fix; }
    GPSState getState() const { return currentState; }
    bool isReady() const { return currentState == GPSState::FIX_3D || currentState == GPSState::FIX_2D; }
    uint32_t getFixAge() const;
    
    // Power Management
    void enableEcoMode();
    void disableEcoMode();
    void setPowerMode(uint8_t mode);
    
    // Advanced Configuration
    void configureNMEAOutput(uint8_t msg_id, uint8_t msg_sub_id, uint8_t rate);
    void configureTimePulse(uint32_t frequency);
    void enableAssistNowAutonomous();
    void setDynamicModel(uint8_t model);
    
    // Calculations
    double calculateDistance(double lat1, double lon1, double lat2, double lon2);
    double calculateHeading(double lat1, double lon1, double lat2, double lon2);
    double calculateBearing(double lat1, double lon1, double lat2, double lon2);
    
    // Error Handling
    bool hasError() const;
    GPSError getError() const { return errorInfo; }
    void clearErrors();
    
    // Diagnostics
    bool performSelfTest();
    void getDiagnostics(String& diagnostics);
    void printOutput();
    
    // Advanced Features
    void setAntennaCableDelay(uint16_t delay_ns);
    void enableSBAS(bool enable);
    void setMinElevation(uint8_t degrees);
    void setMinSNR(uint8_t snr);
    
    // Data Logging
    void enableDataLogging(bool enable);
    void logRawData();
    void logProcessedData();
};

#endif // GPS_INTERFACE_H 