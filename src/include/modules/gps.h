#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <TinyGPS++.h>

namespace Velma {
namespace GPS {

    // ===============================
    // CONFIGURATION & CONTROL
    // ===============================

    /**
     * @brief Initialize the GPS module (UART setup).
     * 
     * Sets up the GPS module with specified serial interface and baud rate.
     * Also initializes the transistor pin for power control.
     * 
     * @param serial Reference to HardwareSerial interface (e.g. Serial1).
     * @param baud_rate Communication baud rate (default 9600).
     */
    void init(HardwareSerial& serial, uint32_t baud_rate = 9600);

    /**
     * @brief Power on the GPS module (if controlled by transistor).
     * 
     * Enables power to the GPS module by setting the transistor pin HIGH.
     * Should be called after init() and before attempting to read GPS data.
     */
    void power_on();

    /**
     * @brief Power off the GPS module.
     * 
     * Disables power to the GPS module by setting the transistor pin LOW.
     * Use this to save power during extended periods of inactivity.
     */
    void power_off();

    /**
     * @brief Enable eco mode to reduce power consumption.
     * 
     * Sends UBX-CFG-PMS command to enable power management mode.
     * This reduces power consumption but may affect fix acquisition time.
     */
    void enable_eco_mode();

    /**
     * @brief Configure PPS (Timepulse) output frequency.
     * 
     * Configures the GPS module's timepulse output for sensor synchronization.
     * Sends UBX-CFG-TP5 command with specified frequency.
     * 
     * @param frequency Desired PPS frequency in Hz (e.g. 1 for 1Hz).
     */
    void configure_timepulse(uint32_t frequency);

    /**
     * @brief Enable AssistNow Autonomous mode (if supported).
     * 
     * Enables autonomous orbit prediction to reduce Time To First Fix (TTFF).
     * Sends UBX-CFG-NAVX5 command to enable AssistNow Autonomous.
     */
    void enable_assistnow_autonomous();

    /**
     * @brief Send raw UBX command.
     * 
     * Sends a raw UBX protocol command to the GPS module.
     * Command should include sync chars, class, ID, length, payload, and checksum.
     * 
     * @param msg Pointer to complete UBX command bytes.
     * @param len Length of command in bytes.
     */
    void send_ubx(uint8_t* msg, uint8_t len);

    /**
     * @brief Request raw measurement data for RTK applications.
     * 
     * Configures the GPS module to output raw measurement data (RXM-RAWX, RXM-SFRBX).
     * Required for Real-Time Kinematic (RTK) positioning applications.
     */
    void request_raw_data();

    /**
     * @brief Set navigation update rate.
     * 
     * Configures how often the GPS module calculates and outputs position solutions.
     * Sends UBX-CFG-RATE command.
     * 
     * @param rate_hz Navigation update rate in Hz (1-10Hz typical).
     */
    void set_navigation_rate(uint8_t rate_hz);

    /**
     * @brief Enable/disable specific NMEA sentences.
     * 
     * Configures which NMEA sentences are output by the GPS module.
     * Sends UBX-CFG-MSG command for each sentence type.
     * 
     * @param msg_id NMEA message ID (e.g., 0xF0 for GGA).
     * @param msg_sub_id NMEA message sub-ID (e.g., 0x00 for GGA).
     * @param rate Output rate (0=disabled, 1=every solution, etc.).
     */
    void configure_nmea_output(uint8_t msg_id, uint8_t msg_sub_id, uint8_t rate);

    // ===============================
    // DATA STRUCTURES
    // ===============================

    /**
     * @brief Structure to hold GPS coordinate data.
     */
    struct Coordinate {
        double latitude;   ///< Latitude in decimal degrees (-90 to +90)
        double longitude;  ///< Longitude in decimal degrees (-180 to +180)
    };

    /**
     * @brief Structure to hold comprehensive GPS status information.
     */
    struct GPSStatus {
        bool fix_valid;           ///< True if GPS has a valid fix
        uint8_t satellites_used;  ///< Number of satellites used in solution
        double hdop;              ///< Horizontal Dilution of Precision
        double altitude;          ///< Altitude above mean sea level (meters)
        double speed_knots;       ///< Ground speed in knots
        double course_deg;        ///< Course over ground in degrees
        uint32_t last_update_ms;  ///< Timestamp of last valid update
    };

    // ===============================
    // DATA HANDLING
    // ===============================

    /**
     * @brief Read and parse available GPS data.
     * 
     * Processes incoming NMEA data using TinyGPS++ library.
     * Updates internal state with latest position, status, and satellite information.
     * Should be called frequently (e.g., in main loop) to maintain current data.
     * 
     * @return true if new data was processed, false otherwise.
     */
    bool read();

    /**
     * @brief Get current GPS coordinate.
     * 
     * Returns the most recently parsed GPS coordinate.
     * Check has_fix() first to ensure data validity.
     * 
     * @return Coordinate struct with latitude and longitude in decimal degrees.
     */
    Coordinate get_coordinate();

    /**
     * @brief Check if a valid GPS fix is available.
     * 
     * Indicates whether the GPS module has acquired a valid position fix.
     * Only trust coordinate data when this returns true.
     * 
     * @return true if valid fix available, false otherwise.
     */
    bool has_fix();

    /**
     * @brief Get comprehensive GPS status information.
     * 
     * Returns detailed status including satellite count, accuracy metrics,
     * and additional navigation data.
     * 
     * @return GPSStatus struct with comprehensive GPS information.
     */
    GPSStatus get_status();

    /**
     * @brief Get the age of the last GPS fix.
     * 
     * Returns the time elapsed since the last valid GPS position update.
     * Useful for determining data freshness and implementing timeouts.
     * 
     * @return Age of last fix in milliseconds.
     */
    uint32_t get_fix_age_ms();

    // ===============================
    // NAVIGATION UTILITIES
    // ===============================

    /**
     * @brief Convert NMEA degrees/minutes to decimal degrees.
     * 
     * Converts GPS coordinate format (DDMM.MMMM or DDDMM.MMMM) to decimal degrees.
     * Handles both latitude (DD format) and longitude (DDD format) inputs.
     * 
     * @param degrees Integer degrees portion.
     * @param minutes Fractional minutes portion.
     * @param direction Cardinal direction ('N', 'S', 'E', 'W').
     * @return Decimal degrees with appropriate sign (negative for S/W).
     */
    double convertToDecimalDegrees(int degrees, double minutes, char direction);

    /**
     * @brief Calculate distance between two coordinates (Haversine formula).
     * 
     * Calculates the great-circle distance between two points on Earth's surface.
     * Uses the Haversine formula for accurate results over short to medium distances.
     * 
     * @param lat1 Latitude of point 1 in decimal degrees.
     * @param lon1 Longitude of point 1 in decimal degrees.
     * @param lat2 Latitude of point 2 in decimal degrees.
     * @param lon2 Longitude of point 2 in decimal degrees.
     * @return Distance in meters.
     */
    double calculateDistance(double lat1, double lon1, double lat2, double lon2);

    /**
     * @brief Calculate heading (bearing) from point 1 to point 2.
     * 
     * Calculates the initial bearing (forward azimuth) from the first point
     * to the second point along the great circle path.
     * 
     * @param lat1 Latitude of starting point in decimal degrees.
     * @param lon1 Longitude of starting point in decimal degrees.
     * @param lat2 Latitude of destination point in decimal degrees.
     * @param lon2 Longitude of destination point in decimal degrees.
     * @return Heading in degrees (0° = North, 90° = East, 180° = South, 270° = West).
     */
    double calculateHeading(double lat1, double lon1, double lat2, double lon2);

    /**
     * @brief Combined distance and heading calculation using NMEA input format.
     * 
     * Convenience function that converts NMEA coordinate format to decimal degrees
     * and calculates both distance and heading between two points.
     * Results are printed to Serial output.
     * 
     * @param lat1Degrees Latitude 1 degrees portion.
     * @param lat1Minutes Latitude 1 minutes portion.
     * @param lat1Direction Latitude 1 direction ('N' or 'S').
     * @param lon1Degrees Longitude 1 degrees portion.
     * @param lon1Minutes Longitude 1 minutes portion.
     * @param lon1Direction Longitude 1 direction ('E' or 'W').
     * @param lat2Degrees Latitude 2 degrees portion.
     * @param lat2Minutes Latitude 2 minutes portion.
     * @param lat2Direction Latitude 2 direction ('N' or 'S').
     * @param lon2Degrees Longitude 2 degrees portion.
     * @param lon2Minutes Longitude 2 minutes portion.
     * @param lon2Direction Longitude 2 direction ('E' or 'W').
     */
    void calculateDistanceAndHeading(
        int lat1Degrees, double lat1Minutes, char lat1Direction,
        int lon1Degrees, double lon1Minutes, char lon1Direction,
        int lat2Degrees, double lat2Minutes, char lat2Direction,
        int lon2Degrees, double lon2Minutes, char lon2Direction);

    /**
     * @brief Convert radians to degrees.
     * 
     * Utility function for angle conversions in navigation calculations.
     * 
     * @param radians Angle in radians.
     * @return Angle in degrees.
     */
    double degrees(double radians);

    /**
     * @brief Convert degrees to radians.
     * 
     * Utility function for angle conversions in navigation calculations.
     * 
     * @param degrees Angle in degrees.
     * @return Angle in radians.
     */
    double radians(double degrees);

} // namespace GPS
} // namespace Velma

#endif // GPS_H