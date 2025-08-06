//EEPROM read/write wrappers, user settings storage, flight parameter persistence, and default resets.
#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include <Arduino.h>

// Initialize EEPROM system
void eeprom_init();

// Read value from EEPROM address
uint8_t eeprom_read(uint16_t address);

// Write value to EEPROM address
void eeprom_write(uint16_t address, uint8_t value);

// Reset EEPROM to default values
void eeprom_reset_defaults();

#endif // EEPROM_MANAGER_H
