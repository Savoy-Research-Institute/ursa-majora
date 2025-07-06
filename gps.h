#ifndef GPS_H
#define GPS_H

#include <Arduino.h>

// Define the GPS transistor control pin externally
extern const int gps_transistor_pin;

// GPS control functions
void switch_gps_on();
void switch_gps_off();
void read_gps();
void run_gps_ecomode();

// Coordinate conversion and navigation
double convertToDecimalDegrees(int degrees, double minutes, char direction);
double calculateDistance(double lat1, double lon1, double lat2, double lon2);
double calculateHeading(double lat1, double lon1, double lat2, double lon2);
void calculateDistanceAndHeading(
    int lat1Degrees, double lat1Minutes, char lat1Direction,
    int lon1Degrees, double lon1Minutes, char lon1Direction,
    int lat2Degrees, double lat2Minutes, char lat2Direction,
    int lon2Degrees, double lon2Minutes, char lon2Direction);

#endif