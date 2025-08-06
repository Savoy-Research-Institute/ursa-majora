//THIS library is included in cube and blizzard, if will sense which is which thru a variable, 
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

//BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
//https://aprs.gids.nl/nmea/     website link
//https://www.latlong.net/Show-Latitude-Longitude.html     
//Reverse geocoding is the process to convert the latitude and longitude coordinates to a readable address. 

/*Neo6M gps, the module should read and give data naturally without any code
Assisted positioning, where the drone remembers the last place it locked satelittes, this helps reduce time to lock generally, 
REad and parse NMEA sentenses to get position, velocity and time data, absolute altitude, track made good, DOP, constellation data
Position of status, System mode indicator, GPRMC, GPGGA, GPGSA, GPGLL.
GPS lock, absolute altitude redundant source, change data sample rate, change power modes, waypoint.read(){}  waypoint.write(){}
the blizzard tells the cube heading and distance to follow by streaming positive and negative error
*/
/*Requirements
Read gps code via GPS.read(B00000000)toggle what to be read thru this register, 
            i can toggle the data read thru these bits, representing, 
            GPRMC(recommended min data)This log contains time, date, position, 
                track made good and speed data provided by the GPS navigation receiver. 
                RMC and RMB are the recommended minimum navigation data to be provided 
                by a GNSS receiver.
            GPGGA(GLobal positioning system fix data)This log contains time, 
                position and fix related data of the GNSS receiver.
            GPGSA(GNSS, DOP, Active satelittes)This log contains GNSS receiver operating mode, 
                satellites used for navigation and DOP values.
            GPGLL()
use algs to calculate distance between greedy and destination coordinates
            calculate desitnation heading using two coordinates
toggle Eco and sleep modes via GPS.sleep()


*/

//~~~~~~~~~~~~~~~~GPS functions~~~~~~~~~~~~~~~~~~~~~~~~
// Function to switch the module on
   // Turn the transistor on
// Function to switch the module off
    // Turn the transistor off
 void read_gps()
    // Check if data is available to read from NEO-6M GPS module
        // Read the incoming byte from NEO-6M GPS module
        // Print the incoming byte to Serial Monitor
void run_gps_ecomode()
    /*
  During a Cold start, a receiver in Eco Mode works exactly as in Maximum Performance Mode. Once a position
  can be calculated and a sufficient number of satellites are being tracked, the acquisition engine is
  powered off resulting in significant power savings. The tracking engine continuously tracks acquired
  satellites and acquires other available or emerging satellites.
  Note that even if the acquisition engine is powered off, satellites continue to be acquired.*/

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
double convertToDecimalDegrees(int degrees, double minutes, char direction)
{
    double decimalDegrees = degrees + (minutes / 60.0);
    if (direction == 'S' || direction == 'W')
    {
        decimalDegrees = -decimalDegrees;
    }
    return decimalDegrees;
}

// Function to calculate the distance between two GPS coordinates using the Haversine formula
double calculateDistance(double lat1, double lon1, double lat2, double lon2)
{
    const double R = 6371000; // Earth's radius in meters
    double lat1Rad = radians(lat1);
    double lat2Rad = radians(lat2);
    double deltaLat = radians(lat2 - lat1);
    double deltaLon = radians(lon2 - lon1);

    double a = sin(deltaLat / 2) * sin(deltaLat / 2) +
               cos(lat1Rad) * cos(lat2Rad) *
                   sin(deltaLon / 2) * sin(deltaLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    double distance = R * c;
    return distance;
}

// Function to calculate the heading from one GPS coordinate to another
double calculateHeading(double lat1, double lon1, double lat2, double lon2)
{
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

    // Convert input coordinates to decimal degrees
    double lat1 = convertToDecimalDegrees(lat1Degrees, lat1Minutes, lat1Direction);
    double lon1 = convertToDecimalDegrees(lon1Degrees, lon1Minutes, lon1Direction);
    double lat2 = convertToDecimalDegrees(lat2Degrees, lat2Minutes, lat2Direction);
    double lon2 = convertToDecimalDegrees(lon2Degrees, lon2Minutes, lon2Direction);

    // Calculate distance and heading
    double distance = calculateDistance(lat1, lon1, lat2, lon2);
    double heading = calculateHeading(lat1, lon1, lat2, lon2);

    // Print results
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" meters");
    Serial.print("Heading: ");
    Serial.print(heading);
    Serial.println(" degrees");
}

// Function to switch the module on
void switch_gps_on()
{
    // Turn the transistor on
    digitalWrite(gps_transistor_pin, HIGH);
}
// Function to switch the module off
void switch_gps_off()
{
    // Turn the transistor off
    digitalWrite(gps_transistor_pin, LOW);
}
void read_gps()
{
    // Check if data is available to read from NEO-6M GPS module
    if (Serial1.available())
    {
        // Read the incoming byte from NEO-6M GPS module
        char incomingByte = Serial1.read(); // change from hardware serial to software serial

        // Print the incoming byte to Serial Monitor
        Serial.print(incomingByte);
    }
}
void change_gps_sample_rate()
{
    // change the gps data sample rate, the blizzard equivalent to this in seting form,
}
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
void run_gps_ecomode()
{
    /*
    During a Cold start, a receiver in Eco Mode works exactly as in Maximum Performance Mode. Once a position
    can be calculated and a sufficient number of satellites are being tracked, the acquisition engine is
    powered off resulting in significant power savings. The tracking engine continuously tracks acquired
    satellites and acquires other available or emerging satellites.
    Note that even if the acquisition engine is powered off, satellites continue to be acquired.*/
}

// Function to calibrate GPS
void calibrateGPS()
{ // Perform GPS calibration steps
    gpsCalibrationData = gatherGPSCalibrationData();
    applyGPSCalibration(gpsCalibrationData);
}
// Function to parse GPS data
void parseGPSData(gpsData)
{
    latitude = extractLatitude(gpsData);
    longitude = extractLongitude(gpsData);
    altitude = extractAltitude(gpsData) return (latitude, longitude, altitude);
}


/*
#16877 set home location code, this will run on the cube and the blizzard, 









*/
