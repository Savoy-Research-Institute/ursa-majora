//custom EEPROM library for velma, 
/*Arduino Uno: 1 KB (1024 bytes)
Arduino Nano: 1 KB (1024 bytes)
Arduino Mega 2560: 4 KB (4096 bytes)
*/
#include <EEPROM.h>
//memory whose values are kept when the board is turned off, 

//EEPROM.write(address, value) value to write(0-255)byte, an EEPROM.write takes 3.3ms to complete, 100k write/erase cycles
//ADDRESS, location to read/write starting from 0, 
//EEPROM.read(address)... read a byte from the EEPROM, 
//locations never written will have a value of 255
void EEPROMclear(){
    //loop through the entirety of the EEPROM.length, EEPROM.write(i, 0)

}
//EEPROM.update(address, value), write a byte to the EEPROM, only when it differs from the one already saved at the address
//use instead of EEPROM.write to save cycles, 

//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
/*objectives, read from any part of the EEPROM, 
write to any part of the EEPROM, 
update, 
clear the EEPROM
*/
//EEPROM.get(address, data) read any data type or object from the EEPROM, data can be primitive type, or custom structure, 
//returns reference to that data passed in, 

//BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
/****************EEPROM functions********************************************/
void change_access_password()
{
    // on promptchange access password in EEPROM, after receiving admin prompt from blizzard,
}
void store_home_coordinates()
{
    // the drone takes many gps samples, calculates a mean sets that as home location, such that it can always return home, sometimes sets this to EEPROM,
    // due to flight priority settings, or to change batteries, long flights
}
void change_name()
{
    // on admmin prompt, change the drone name on EEPROM,
}
// EEPROM.get
// EEPROM.update
// EEPROM.read
// EEPROM[]
// EEPROM.write
