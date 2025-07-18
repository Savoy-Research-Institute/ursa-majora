// In your main code:
const int gps_transistor_pin = 12;

void setup() {
    Serial.begin(115200);
    
    // Initialize GPS on Serial1 at 9600 baud
    Velma::GPS::init(Serial1, 9600);
    
    // Optional: Configure for better performance
    Velma::GPS::set_navigation_rate(5); // 5Hz updates
    Velma::GPS::enable_assistnow_autonomous();
}

void loop() {
    // Read GPS data
    if (Velma::GPS::read()) {
        if (Velma::GPS::has_fix()) {
            auto coord = Velma::GPS::get_coordinate();
            auto status = Velma::GPS::get_status();
            
            Serial.print("Lat: "); Serial.println(coord.latitude, 8);
            Serial.print("Lon: "); Serial.println(coord.longitude, 8);
            Serial.print("Satellites: "); Serial.println(status.satellites_used);
            Serial.print("HDOP: "); Serial.println(status.hdop);
        }
    }
}