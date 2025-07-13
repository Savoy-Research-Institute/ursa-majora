#include "GPS.h"

namespace Velma {
namespace GPS {

    // Internal state
    static HardwareSerial* gpsSerial = nullptr;
    static Coordinate currentCoordinate = {0.0, 0.0};
    static bool fixAvailable = false;

    // Define your GPS transistor pin externally in main code
    extern const int gps_transistor_pin;

    void init(HardwareSerial& serial, uint32_t baud_rate) {
        gpsSerial = &serial;
        gpsSerial->begin(baud_rate);
        pinMode(gps_transistor_pin, OUTPUT);
        power_on();
    }

    void power_on() {
        digitalWrite(gps_transistor_pin, HIGH);
    }

    void power_off() {
        digitalWrite(gps_transistor_pin, LOW);
    }

    void read() {
        while (gpsSerial->available()) {
            String line = gpsSerial->readStringUntil('\n');

            // Basic NMEA GGA parsing stub
            if (line.startsWith("$GPGGA")) {
                // Split by comma
                int indices[15];
                int idx = 0, lastIdx = -1;
                for (int i = 0; i < line.length(); i++) {
                    if (line[i] == ',') {
                        indices[idx++] = i;
                        if (idx >= 15) break;
                    }
                }

                // Extract latitude
                String latStr = line.substring(indices[1]+1, indices[2]);
                String latDir = line.substring(indices[2]+1, indices[3]);
                String lonStr = line.substring(indices[3]+1, indices[4]);
                String lonDir = line.substring(indices[4]+1, indices[5]);
                String fixQuality = line.substring(indices[5]+1, indices[6]);

                if (latStr.length() > 0 && lonStr.length() > 0 && fixQuality.toInt() > 0) {
                    // Convert latitude
                    double latRaw = latStr.toDouble();
                    int latDeg = int(latRaw / 100);
                    double latMin = latRaw - (latDeg * 100);
                    double latitude = convertToDecimalDegrees(latDeg, latMin, latDir[0]);

                    // Convert longitude
                    double lonRaw = lonStr.toDouble();
                    int lonDeg = int(lonRaw / 100);
                    double lonMin = lonRaw - (lonDeg * 100);
                    double longitude = convertToDecimalDegrees(lonDeg, lonMin, lonDir[0]);

                    currentCoordinate.latitude = latitude;
                    currentCoordinate.longitude = longitude;
                    fixAvailable = true;
                } else {
                    fixAvailable = false;
                }
            }
        }
    }

    Coordinate get_coordinate() {
        return currentCoordinate;
    }

    bool has_fix() {
        return fixAvailable;
    }

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

    void enable_eco_mode() {
        // Send UBX command to enable eco mode
        // Example stub
        uint8_t msg[] = {0xB5, 0x62, /* ... */};
        send_ubx(msg, sizeof(msg));
    }

    void configure_timepulse(uint32_t frequency) {
        // Configure timepulse frequency via UBX command
        // Example stub
        uint8_t msg[] = {0xB5, 0x62, /* ... */};
        send_ubx(msg, sizeof(msg));
    }

    void enable_assistnow_autonomous() {
        // Enable AssistNow Autonomous via UBX command
        uint8_t msg[] = {0xB5, 0x62, /* ... */};
        send_ubx(msg, sizeof(msg));
    }

    void send_ubx(uint8_t* msg, uint8_t len) {
        for (uint8_t i = 0; i < len; i++) {
            gpsSerial->write(msg[i]);
        }
    }

    void request_raw_data() {
        // Request raw data (e.g. for RTK)
        uint8_t msg[] = {0xB5, 0x62, /* ... */};
        send_ubx(msg, sizeof(msg));
    }

} // namespace GPS
} // namespace Velma
// End of src/gps.cpp