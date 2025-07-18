#include "GPS.h"

namespace Velma {
namespace GPS {

    // ===============================
    // INTERNAL STATE
    // ===============================
    
    static HardwareSerial* gpsSerial = nullptr;
    static TinyGPSPlus gps;
    static Coordinate currentCoordinate = {0.0, 0.0};
    static GPSStatus currentStatus = {false, 0, 99.99, 0.0, 0.0, 0.0, 0};
    static uint32_t lastValidUpdate = 0;

    // Define your GPS transistor pin externally in main code
    extern const int gps_transistor_pin;

    // ===============================
    // CONFIGURATION & CONTROL
    // ===============================

    void init(HardwareSerial& serial, uint32_t baud_rate) {
        gpsSerial = &serial;
        gpsSerial->begin(baud_rate);
        pinMode(gps_transistor_pin, OUTPUT);
        power_on();
        
        // Wait a moment for GPS to initialize
        delay(100);
        
        // Optional: Configure for better performance
        set_navigation_rate(1); // 1Hz default
        configure_nmea_output(0xF0, 0x00, 1); // Enable GGA
        configure_nmea_output(0xF0, 0x02, 1); // Enable GSA
        configure_nmea_output(0xF0, 0x04, 1); // Enable RMC
    }

    void power_on() {
        digitalWrite(gps_transistor_pin, HIGH);
    }

    void power_off() {
        digitalWrite(gps_transistor_pin, LOW);
    }

    void enable_eco_mode() {
        // UBX-CFG-PMS: Power Management Settings
        // Enable power save mode with update period of 1000ms
        uint8_t msg[] = {
            0xB5, 0x62,           // Sync chars
            0x06, 0x86,           // Class CFG, ID PMS
            0x08, 0x00,           // Length: 8 bytes
            0x00,                 // Version
            0x01,                 // Power setup value (1 = power save mode)
            0x00, 0x00,           // Period (not used in power save mode)
            0x00, 0x00,           // On time (not used in power save mode)
            0x00, 0x00,           // Reserved
            0x95, 0x8A            // Checksum (calculated)
        };
        send_ubx(msg, sizeof(msg));
    }

    void configure_timepulse(uint32_t frequency) {
        // UBX-CFG-TP5: Time Pulse Parameters
        // Configure timepulse 0 with specified frequency
        uint8_t msg[] = {
            0xB5, 0x62,           // Sync chars
            0x06, 0x31,           // Class CFG, ID TP5
            0x20, 0x00,           // Length: 32 bytes
            0x00,                 // Time pulse selection (0 = TIMEPULSE)
            0x00,                 // Reserved
            0x00, 0x00,           // Reserved
            0x32, 0x00,           // Antenna cable delay (50ns)
            0x00, 0x00,           // RF group delay
            (uint8_t)(frequency & 0xFF), (uint8_t)((frequency >> 8) & 0xFF),
            (uint8_t)((frequency >> 16) & 0xFF), (uint8_t)((frequency >> 24) & 0xFF), // Freq period
            (uint8_t)(frequency & 0xFF), (uint8_t)((frequency >> 8) & 0xFF),
            (uint8_t)((frequency >> 16) & 0xFF), (uint8_t)((frequency >> 24) & 0xFF), // Freq period locked
            0x80, 0x96, 0x98, 0x00, // Pulse length ratio
            0x80, 0x96, 0x98, 0x00, // Pulse length ratio locked
            0x00, 0x00, 0x00, 0x00, // User config delay
            0x77, 0x00, 0x00, 0x00, // Flags (active, lock to GPS time, etc.)
            0x00, 0x00              // Checksum placeholders
        };
        
        // Calculate checksum
        uint8_t ck_a = 0, ck_b = 0;
        for (int i = 2; i < sizeof(msg) - 2; i++) {
            ck_a += msg[i];
            ck_b += ck_a;
        }
        msg[sizeof(msg) - 2] = ck_a;
        msg[sizeof(msg) - 1] = ck_b;
        
        send_ubx(msg, sizeof(msg));
    }

    void enable_assistnow_autonomous() {
        // UBX-CFG-NAVX5: Navigation Expert Settings
        // Enable AssistNow Autonomous
        uint8_t msg[] = {
            0xB5, 0x62,           // Sync chars
            0x06, 0x23,           // Class CFG, ID NAVX5
            0x28, 0x00,           // Length: 40 bytes
            0x00, 0x00,           // Version
            0x04, 0x00,           // Mask1 (enable aopCfg)
            0x00, 0x00, 0x00, 0x00, // Mask2
            0x00, 0x00,           // Reserved
            0x03,                 // Min SVs
            0x10,                 // Max SVs
            0x06,                 // Min CNO
            0x00,                 // Reserved
            0x00,                 // Initial fix 3D
            0x00, 0x00,           // Reserved
            0x00,                 // Reserved
            0x00,                 // Reserved
            0x00, 0x00,           // Reserved
            0x00, 0x00, 0x00, 0x00, // Reserved
            0x01,                 // Use AssistNow Autonomous
            0x00, 0x00,           // Reserved
            0x00, 0x00,           // Reserved
            0x00, 0x00, 0x00, 0x00, // Reserved
            0x00, 0x00, 0x00, 0x00, // Reserved
            0x00, 0x00, 0x00, 0x00, // Reserved
            0x00, 0x00            // Checksum placeholders
        };
        
        // Calculate checksum
        uint8_t ck_a = 0, ck_b = 0;
        for (int i = 2; i < sizeof(msg) - 2; i++) {
            ck_a += msg[i];
            ck_b += ck_a;
        }
        msg[sizeof(msg) - 2] = ck_a;
        msg[sizeof(msg) - 1] = ck_b;
        
        send_ubx(msg, sizeof(msg));
    }

    void send_ubx(uint8_t* msg, uint8_t len) {
        if (gpsSerial) {
            for (uint8_t i = 0; i < len; i++) {
                gpsSerial->write(msg[i]);
            }
        }
    }

    void request_raw_data() {
        // Enable RXM-RAWX messages (raw measurement data)
        uint8_t rawx_msg[] = {
            0xB5, 0x62,           // Sync chars
            0x06, 0x01,           // Class CFG, ID MSG
            0x08, 0x00,           // Length: 8 bytes
            0x02, 0x15,           // Message class RXM, ID RAWX
            0x00, 0x01, 0x01, 0x01, 0x01, 0x01, // Rate on each port
            0x2D, 0x83            // Checksum
        };
        send_ubx(rawx_msg, sizeof(rawx_msg));
        
        // Enable RXM-SFRBX messages (subframe buffer)
        uint8_t sfrbx_msg[] = {
            0xB5, 0x62,           // Sync chars
            0x06, 0x01,           // Class CFG, ID MSG
            0x08, 0x00,           // Length: 8 bytes
            0x02, 0x13,           // Message class RXM, ID SFRBX
            0x00, 0x01, 0x01, 0x01, 0x01, 0x01, // Rate on each port
            0x2B, 0x7F            // Checksum
        };
        send_ubx(sfrbx_msg, sizeof(sfrbx_msg));
    }

    void set_navigation_rate(uint8_t rate_hz) {
        uint16_t meas_rate = 1000 / rate_hz; // Convert Hz to ms
        
        // UBX-CFG-RATE: Navigation/Measurement Rate Settings
        uint8_t msg[] = {
            0xB5, 0x62,           // Sync chars
            0x06, 0x08,           // Class CFG, ID RATE
            0x06, 0x00,           // Length: 6 bytes
            (uint8_t)(meas_rate & 0xFF), (uint8_t)((meas_rate >> 8) & 0xFF), // Measurement rate
            0x01, 0x00,           // Navigation rate (1 nav per meas)
            0x00, 0x00,           // Time reference (0 = GPS time)
            0x00, 0x00            // Checksum placeholders
        };
        
        // Calculate checksum
        uint8_t ck_a = 0, ck_b = 0;
        for (int i = 2; i < sizeof(msg) - 2; i++) {
            ck_a += msg[i];
            ck_b += ck_a;
        }
        msg[sizeof(msg) - 2] = ck_a;
        msg[sizeof(msg) - 1] = ck_b;
        
        send_ubx(msg, sizeof(msg));
    }

    void configure_nmea_output(uint8_t msg_id, uint8_t msg_sub_id, uint8_t rate) {
        // UBX-CFG-MSG: Message Configuration
        uint8_t msg[] = {
            0xB5, 0x62,           // Sync chars
            0x06, 0x01,           // Class CFG, ID MSG
            0x08, 0x00,           // Length: 8 bytes
            msg_id, msg_sub_id,   // Message class and ID
            0x00, rate, rate, rate, rate, rate, // Rate on each port
            0x00, 0x00            // Checksum placeholders
        };
        
        // Calculate checksum
        uint8_t ck_a = 0, ck_b = 0;
        for (int i = 2; i < sizeof(msg) - 2; i++) {
            ck_a += msg[i];
            ck_b += ck_a;
        }
        msg[sizeof(msg) - 2] = ck_a;
        msg[sizeof(msg) - 1] = ck_b;
        
        send_ubx(msg, sizeof(msg));
    }

    // ===============================
    // DATA HANDLING
    // ===============================

    bool read() {
        bool newData = false;
        
        if (!gpsSerial) return false;
        
        while (gpsSerial->available()) {
            char c = gpsSerial->read();
            if (gps.encode(c)) {
                newData = true;
            }
        }
        
        if (newData) {
            // Update coordinate data
            if (gps.location.isValid()) {
                currentCoordinate.latitude = gps.location.lat();
                currentCoordinate.longitude = gps.location.lng();
                currentStatus.fix_valid = true;
                lastValidUpdate = millis();
                currentStatus.last_update_ms = lastValidUpdate;
            } else {
                currentStatus.fix_valid = false;
            }
            
            // Update status information
            currentStatus.satellites_used = gps.satellites.value();
            currentStatus.hdop = gps.hdop.hdop();
            currentStatus.altitude = gps.altitude.meters();
            currentStatus.speed_knots = gps.speed.knots();
            currentStatus.course_deg = gps.course.deg();
        }
        
        return newData;
    }

    Coordinate get_coordinate() {
        return currentCoordinate;
    }

    bool has_fix() {
        return currentStatus.fix_valid && (millis() - lastValidUpdate < 5000); // 5 second timeout
    }

    GPSStatus get_status() {
        return currentStatus;
    }

    uint32_t get_fix_age_ms() {
        return millis() - lastValidUpdate;
    }

    // ===============================
    // NAVIGATION UTILITIES
    // ===============================

    double convertToDecimalDegrees(int degrees, double minutes, char direction) {
        double decimal = degrees + (minutes / 60.0);
        if (direction == 'S' || direction == 'W') {
            decimal *= -1.0;
        }
        return decimal;
    }

    double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
        // Haversine formula
        const double R = 6371000; // Radius of Earth in meters
        double dLat = radians(lat2 - lat1);
        double dLon = radians(lon2 - lon1);
        lat1 = radians(lat1);
        lat2 = radians(lat2);

        double a = sin(dLat/2) * sin(dLat/2) +
                   sin(dLon/2) * sin(dLon/2) * cos(lat1) * cos(lat2);
        double c = 2 * atan2(sqrt(a), sqrt(1 - a));
        return R * c;
    }

    double calculateHeading(double lat1, double lon1, double lat2, double lon2) {
        double dLon = radians(lon2 - lon1);
        lat1 = radians(lat1);
        lat2 = radians(lat2);

        double y = sin(dLon) * cos(lat2);
        double x = cos(lat1) * sin(lat2) -
                   sin(lat1) * cos(lat2) * cos(dLon);
        double heading = atan2(y, x);
        heading = degrees(heading);
        return (heading < 0) ? heading + 360 : heading;
    }

    void calculateDistanceAndHeading(
        int lat1Degrees, double lat1Minutes, char lat1Direction,
        int lon1Degrees, double lon1Minutes, char lon1Direction,
        int lat2Degrees, double lat2Minutes, char lat2Direction,
        int lon2Degrees, double lon2Minutes, char lon2Direction)
    {
        double lat1 = convertToDecimalDegrees(lat1Degrees, lat1Minutes, lat1Direction);
        double lon1 = convertToDecimalDegrees(lon1Degrees, lon1Minutes, lon1Direction);
        double lat2 = convertToDecimalDegrees(lat2Degrees, lat2Minutes, lat2Direction);
        double lon2 = convertToDecimalDegrees(lon2Degrees, lon2Minutes, lon2Direction);

        double distance = calculateDistance(lat1, lon1, lat2, lon2);
        double heading = calculateHeading(lat1, lon1, lat2, lon2);

        Serial.print("Distance (m): "); Serial.println(distance);
        Serial.print("Heading (deg): "); Serial.println(heading);
    }

    double degrees(double radians) {
        return radians * 180.0 / PI;
    }

    double radians(double degrees) {
        return degrees * PI / 180.0;
    }

} // namespace GPS
} // namespace Velma