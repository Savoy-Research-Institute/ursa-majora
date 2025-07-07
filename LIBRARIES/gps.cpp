/* Neo6M GPS functions:
   - Read and parse NMEA sentences for position, velocity, time, altitude, DOP, and constellation data
   - Handle GPS lock, power modes, and waypoint calculations
*/

//~~~~~~~~~~~~~~~~GPS functions~~~~~~~~~~~~~~~~~~~~~~~~

#include <Arduino.h>
#include "gps.h"

// Function to switch the GPS module on
void switch_gps_on() {
    digitalWrite(gps_transistor_pin, HIGH);
}

// Function to switch the GPS module off
void switch_gps_off() {
    digitalWrite(gps_transistor_pin, LOW);
}

// Read and print raw GPS data from Serial1
void read_gps() {
    if (Serial1.available()) {
        char incomingByte = Serial1.read();
        Serial.print(incomingByte);
    }
}

// Placeholder for eco mode (power saving)
void run_gps_ecomode() {
    // Implement eco mode logic as needed
}

// Converts coordinates from DDMM.MMM or DDDMM.MMM format to decimal degrees
double convertToDecimalDegrees(int degrees, double minutes, char direction) {
    double decimalDegrees = degrees + (minutes / 60.0);
    if (direction == 'S' || direction == 'W') {
        decimalDegrees = -decimalDegrees;
    }
    return decimalDegrees;
}

// Calculate distance between two GPS coordinates using the Haversine formula
double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371000; // Earth's radius in meters
    double lat1Rad = radians(lat1);
    double lat2Rad = radians(lat2);
    double deltaLat = radians(lat2 - lat1);
    double deltaLon = radians(lon2 - lon1);

    double a = sin(deltaLat / 2) * sin(deltaLat / 2) +
               cos(lat1Rad) * cos(lat2Rad) *
               sin(deltaLon / 2) * sin(deltaLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return R * c;
}

// Calculate heading from one GPS coordinate to another
double calculateHeading(double lat1, double lon1, double lat2, double lon2) {
    double lat1Rad = radians(lat1);
    double lat2Rad = radians(lat2);
    double deltaLon = radians(lon2 - lon1);

    double y = sin(deltaLon) * cos(lat2Rad);
    double x = cos(lat1Rad) * sin(lat2Rad) -
               sin(lat1Rad) * cos(lat2Rad) * cos(deltaLon);
    double heading = atan2(y, x);
    heading = degrees(heading);
    heading = fmod((heading + 360), 360); // Normalize to 0-360 degrees
    return heading;
}

// Main function to read GPS coordinates, calculate distance and heading
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

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" meters");
    Serial.print("Heading: ");
    Serial.print(heading);
    Serial.println(" degrees");
}