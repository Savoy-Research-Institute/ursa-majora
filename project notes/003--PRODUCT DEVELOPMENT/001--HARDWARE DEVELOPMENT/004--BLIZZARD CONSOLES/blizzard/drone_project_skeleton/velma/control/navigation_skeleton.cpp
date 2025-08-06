//this is the navigation skeleton file for velma, navigation meaning 
/*
A} this library holds waypoint features, 
enables the cube to find distance from greedy coordinates to destination coordinates,
enables the cube to find heading from greedy coordinates to destination coordinates, 
            the cube will find difference between destination heading and greedy heading, pass that to the user in a +- format in display
*/





//gps lock code, heightlock code, code that gives direction from two coordinates allowing the drone to move like that,  
/*
magnetometer used alongside the GPS module to calculate greedy orientation, calculate for destination orientation, 
calculate offset in form of an error, use this for navigation, 
*/
/*To write a function that calculates the distance and heading from one set of GPS coordinates to another, we'll use the Haversine formula for distance and basic trigonometry for heading. 
The input coordinates will be in the specified formats, so we'll need to parse and convert them into decimal degrees.

### Haversine Formula for Distance
The Haversine formula calculates the distance between two points on the Earth given their latitude and longitude in decimal degrees.

### Formula for Heading
The heading or bearing from one point to another can be calculated using the following formula:
\[ \theta = \text{atan2}( \sin(\Delta \lambda) \cdot \cos(\phi_2), \cos(\phi_1) \cdot \sin(\phi_2) - \sin(\phi_1) \cdot \cos(\phi_2) \cdot \cos(\Delta \lambda) ) \]

waypoint system code is here, 


Here is the function in Arduino code:

```cpp*/

// SHOULd this be on the blizzard?
//  Converts coordinates from DDMM.MMM and DDDMM.MMM format to decimal degrees
double convertToDecimalDegrees(int degrees, double minutes, char direction) // from GPS functions
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