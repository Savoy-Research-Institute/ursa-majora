// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

// BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
/*Neo6M gps,
REad and parse NMEA sentenses to get position, velocity and time data, absolute altitude, track made good, DOP, constellation data
Position of status, System mode indicator, GPRMC, GPGGA, GPGSA, GPGLL.
GPS lock, absolute altitude redundant source, change data sample rate, change power modes, waypoint.read(){}  waypoint.write(){}
the blizzard tells the cube heading and distance to follow by streaming positive and negative error
*/
// the blizzard will do what the drone can when it comes to geolocation, shepherd
/************GPS CODE********
code to read the GPS blizzard_greedy_location, */
//~~~~~~~~~~~~~~~~GPS functions~~~~~~~~~~~~~~~~~~~~~~~~
// Function to switch the module on
#include "NEO6M.h"
#include <math.h>

NEO6M::NEO6M(int rxPin, int txPin)
{
    gpsSerial = new SoftwareSerial(rxPin, txPin);
    _powerPin = -1; // Default to no power control
}

void NEO6M::begin(long baud)
{
    gpsSerial->begin(baud);
}

bool NEO6M::update()
{
    while (gpsSerial->available())
    {
        char c = gpsSerial->read();
        if (c == '\n')
        {
            if (_buffer.startsWith("$GPRMC"))
            {
                _isValid = parseGPRMC(_buffer);
            }
            else if (_buffer.startsWith("$GPGGA"))
            {
                parseGPGGA(_buffer);
            }
            _buffer = "";
        }
        else
        {
            _buffer += c;
        }
    }
    return _isValid;
}

// Getter functions implementation
float NEO6M::getLatitude() { return _latitude; }
float NEO6M::getLongitude() { return _longitude; }
float NEO6M::getAltitude() { return _altitude; }
float NEO6M::getSpeed() { return _speed; }
int NEO6M::getSatellites() { return _satellites; }
String NEO6M::getTime() { return _time; }
String NEO6M::getDate() { return _date; }
bool NEO6M::isValid() { return _isValid; }

void NEO6M::switchOn()
{
    if (_powerPin != -1)
    { // Turn the transistor on
        digitalWrite(_powerPin, HIGH);
    }
}

void NEO6M::switchOff()
{
    if (_powerPin != -1)
    { // Turn the transistor off
        digitalWrite(_powerPin, LOW);
    }
}
void NEO6M::read_gps()
{
    // Check if data is available to read from NEO-6M GPS module
    if (Serial1.available())
    {
        // Read the incoming byte from NEO-6M GPS module
        char incomingByte = Serial1.read();

        // Print the incoming byte to Serial Monitor
        Serial.print(incomingByte);
    }
}
void NEO6M::setEcoMode(bool enable)
{
    // Implementation depends on specific GPS module commands
    // This is a placeholder for the concept
    if (enable)
    {
        gpsSerial->println("$PMTK161,0*28"); // Enter eco mode
    }
    else
    {
        gpsSerial->println("$PMTK161,1*29"); // Exit eco mode
    }
    /*
  During a Cold start, a receiver in Eco Mode works exactly as in Maximum Performance Mode. Once a position
  can be calculated and a sufficient number of satellites are being tracked, the acquisition engine is
  powered off resulting in significant power savings. The tracking engine continuously tracks acquired
  satellites and acquires other available or emerging satellites.
  Note that even if the acquisition engine is powered off, satellites continue to be acquired.*/
}
// Main function to read GPS coordinates, calculate distance and heading
void NEO6M::calculateDistanceAndHeading(double lat1, double lon1, double lat2, double lon2, double &distance, double &heading)
{
    const double R = 6371000; // Earth's radius in meters
    double lat1Rad = toRadians(lat1);
    double lat2Rad = toRadians(lat2);
    double deltaLat = toRadians(lat2 - lat1);
    double deltaLon = toRadians(lon2 - lon1);

    double a = sin(deltaLat / 2) * sin(deltaLat / 2) +
               cos(lat1Rad) * cos(lat2Rad) *
                   sin(deltaLon / 2) * sin(deltaLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    distance = R * c;

    double y = sin(deltaLon) * cos(lat2Rad);
    double x = cos(lat1Rad) * sin(lat2Rad) -
               sin(lat1Rad) * cos(lat2Rad) * cos(deltaLon);
    heading = toDegrees(atan2(y, x));
    heading = fmod((heading + 360), 360); // Normalize to 0-360 degrees
}

void NEO6M::setUpdateRate(int rate)
{
    // Implementation depends on specific GPS module commands
    // This is an example for NMEA update rate
    String command = "$PMTK220," + String(1000 / rate) + "*1C";
    gpsSerial->println(command);
    // change the gps data sample rate, the blizzard equivalent to this in seting form,
}

void NEO6M::calibrate()
{
    // Placeholder for GPS calibration
    // Actual implementation would depend on the specific calibration procedure
    // for the NEO-6M module
}

double NEO6M::toRadians(double degrees)
{
    return degrees * M_PI / 180.0;
}

double NEO6M::toDegrees(double radians)
{
    return radians * 180.0 / M_PI;
}

bool NEO6M::parseGPRMC(String sentence)
{
    String parts[13];
    int stringex = 0;
    for (int i = 0; i < 13; i++)
    {
        int index = sentence.indexOf(',', stringex);
        parts[i] = sentence.substring(stringex, index);
        stringex = index + 1;
    }

    if (parts[2] == "A")
    {
        _latitude = convertToDecimalDegrees(parts[3]);
        _longitude = convertToDecimalDegrees(parts[5]);
        _speed = parts[7].toFloat() * 1.852; // Convert knots to km/h
        _time = parts[1].substring(0, 2) + ":" + parts[1].substring(2, 4) + ":" + parts[1].substring(4, 6);
        _date = parts[9].substring(0, 2) + "/" + parts[9].substring(2, 4) + "/20" + parts[9].substring(4, 6);
        return true;
    }
    return false;
}

bool NEO6M::parseGPGGA(String sentence)
{
    String parts[15];
    int stringex = 0;
    for (int i = 0; i < 15; i++)
    {
        int index = sentence.indexOf(',', stringex);
        parts[i] = sentence.substring(stringex, index);
        stringex = index + 1;
    }

    _altitude = parts[9].toFloat();
    _satellites = parts[7].toInt();
    return true;
}

double NEO6M::convertToDecimalDegrees(const String &coord, char direction)
{
    int dotIndex = coord.indexOf('.');
    if (dotIndex == -1)
    {
        // Handle error: no decimal point found
        return 0.0;
    }

    String degreePart = coord.substring(0, dotIndex - 2);
    String minutePart = coord.substring(dotIndex - 2);

    double degrees = degreePart.toDouble();
    double minutes = minutePart.toDouble();

    if (degrees < 0 || minutes < 0 || minutes >= 60)
    {
        // Handle error: invalid degree or minute values
        return 0.0;
    }

    double decimalDegrees = degrees + (minutes / 60.0);

    if (direction == 'S' || direction == 'W')
    {
        decimalDegrees = -decimalDegrees;
    }
    else if (direction != 'N' && direction != 'E')
    {
        // Handle error: invalid direction
        return 0.0;
    }

    return decimalDegrees;
}

/*To write a function that calculates the distance and heading from one set of GPS coordinates to another, we'll use the Haversine formula for distance and basic trigonometry for heading. The input coordinates will be in the specified formats, so we'll need to parse and convert them into decimal degrees.

### Haversine Formula for Distance
The Haversine formula calculates the distance between two points on the Earth given their latitude and longitude in decimal degrees.

### Formula for Heading
The heading or bearing from one point to another can be calculated using the following formula:
\[ \theta = \text{atan2}( \sin(\Delta \lambda) \cdot \cos(\phi_2), \cos(\phi_1) \cdot \sin(\phi_2) - \sin(\phi_1) \cdot \cos(\phi_2) \cdot \cos(\Delta \lambda) ) \]

Here is the function in Arduino code:

```cpp*/

// SHOULd this be on the blizzard?
//  Converts coordinates from DDMM.MMM and DDDMM.MMM format to decimal degrees

void change_gps_power_modes()
{
    // max power, power saving, eco mode, etc
}
void gps_loc_lock()
{
    // this feature, allows the drone to return back to designated location and stay there, it takes the heading between the two greedy and destination coordinates
    // and calculates an error in degrees to, that the drone can use to fly back to, the algorithm that does this...
}
void waypoint_read()
{
    // this reads, and stored waypoint data over a period of time, such that can be used to reconstruct the same.
}
void waypoint_write()
{
    // this uses algorithms to calculate heading and distance between two coordinates, calculates their error data that can influence flight,
}
void absolute_altitude_redundancy()
{
    // this function checks barometer and gps altitude data, if similar to an error of like 5%, it reads good, otherwise it flags,
}
