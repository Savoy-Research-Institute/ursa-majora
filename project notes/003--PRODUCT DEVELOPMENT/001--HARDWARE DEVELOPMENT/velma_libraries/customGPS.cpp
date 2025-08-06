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
