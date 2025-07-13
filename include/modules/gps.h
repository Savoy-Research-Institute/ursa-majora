#ifndef GPS_H
#define GPS_H

#include <Arduino.h>

namespace Velma {
namespace GPS {

    // ===============================
    // CONFIGURATION & CONTROL
    // ===============================

    /**
     * @brief Initialize the GPS module (UART setup).
     * 
     * @param serial Reference to HardwareSerial interface (e.g. Serial1).
     * @param baud_rate Communication baud rate (default 9600).
     */
    void init(HardwareSerial& serial, uint32_t baud_rate = 9600);

    /**
     * @brief Power on the GPS module (if controlled by transistor).
     */
    void power_on();

    /**
     * @brief Power off the GPS module.
     */
    void power_off();

    /**
     * @brief Enable eco mode to reduce power consumption.
     * Uses UBX command.
     */
    void enable_eco_mode();

    /**
     * @brief Configure PPS (Timepulse) output frequency.
     * 
     * @param frequency Desired PPS frequency (e.g. 1Hz).
     */
    void configure_timepulse(uint32_t frequency);

    /**
     * @brief Enable AssistNow Autonomous mode (if supported).
     */
    void enable_assistnow_autonomous();

    /**
     * @brief Send raw UBX command.
     * 
     * @param msg Pointer to command bytes.
     * @param len Length of command.
     */
    void send_ubx(uint8_t* msg, uint8_t len);

    /**
     * @brief Request raw measurement data for RTK applications.
     */
    void request_raw_data();

    // ===============================
    // DATA STRUCTURES
    // ===============================

    /**
     * @brief Structure to hold GPS coordinate data.
     */
    struct Coordinate {
        double latitude;
        double longitude;
    };

    // ===============================
    // DATA HANDLING
    // ===============================

    /**
     * @brief Read and parse available GPS data.
     * Updates internal state with latest position.
     */
    void read();

    /**
     * @brief Get current GPS coordinate.
     * 
     * @return Coordinate struct with latitude and longitude.
     */
    Coordinate get_coordinate();

    /**
     * @brief Check if a valid GPS fix is available.
     * 
     * @return true if fix available.
     * @return false otherwise.
     */
    bool has_fix();

    // ===============================
    // NAVIGATION UTILITIES
    // ===============================

    /**
     * @brief Convert NMEA degrees/minutes to decimal degrees.
     * 
     * @param degrees Integer degrees.
     * @param minutes Fractional minutes.
     * @param direction 'N', 'S', 'E', 'W'.
     * @return Decimal degrees with sign.
     */
    double convertToDecimalDegrees(int degrees, double minutes, char direction);

    /**
     * @brief Calculate distance between two coordinates (Haversine formula).
     * 
     * @param lat1 Latitude of point 1.
     * @param lon1 Longitude of point 1.
     * @param lat2 Latitude of point 2.
     * @param lon2 Longitude of point 2.
     * @return Distance in meters.
     */
    double calculateDistance(double lat1, double lon1, double lat2, double lon2);

    /**
     * @brief Calculate heading (bearing) from point 1 to point 2.
     * 
     * @param lat1 Latitude of point 1.
     * @param lon1 Longitude of point 1.
     * @param lat2 Latitude of point 2.
     * @param lon2 Longitude of point 2.
     * @return Heading in degrees (0 = North).
     */
    double calculateHeading(double lat1, double lon1, double lat2, double lon2);

    /**
     * @brief Combined distance and heading calculation using NMEA input format.
     */
    void calculateDistanceAndHeading(
        int lat1Degrees, double lat1Minutes, char lat1Direction,
        int lon1Degrees, double lon1Minutes, char lon1Direction,
        int lat2Degrees, double lat2Minutes, char lat2Direction,
        int lon2Degrees, double lon2Minutes, char lon2Direction);

} // namespace GPS
} // namespace Velma

#endif // GPS_H
