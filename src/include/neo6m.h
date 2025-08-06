#ifndef NEO6M_H
#define NEO6M_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class NEO6M {
public:
    // Constructor and initialization
    NEO6M(int rxPin, int txPin);
    void begin(long baud = 9600);

    // Main update function
    bool update();

    // Getters for GPS data
    float getLatitude();
    float getLongitude();
    float getAltitude();
    float getSpeed();
    int getSatellites();
    String getTime();
    String getDate();
    bool isValid();

    // Power management functions
    void switchOn();
    void switchOff();
    void read_gps();
    void setEcoMode(bool enable);

    // Advanced functions
    void calculateDistanceAndHeading(double lat1, double lon1, double lat2, double lon2, double& distance, double& heading);
    void setUpdateRate(int rate);
    void calibrate();

private:
    SoftwareSerial* gpsSerial;
    String _buffer;
    bool _isValid;
    float _latitude;
    float _longitude;
    float _altitude;
    float _speed;
    int _satellites;
    String _time;
    String _date;
    int _powerPin;  // Pin to control GPS module power

    // Parsing functions
    bool parseGPRMC(String sentence);
    bool parseGPGGA(String sentence);
    float convertToDecimalDegrees(String coord);

    // Helper functions
    double toRadians(double degrees);
    double toDegrees(double radians);
};

#endif // NEO6M_H