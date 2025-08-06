

#ifndef CUSTOMEEPROM_H
#define CUSTOMEEPROM_H

#include <EEPROM.h>

class CustomEEPROM {
public:
    void name(const String &name);
    String readName();
};

#endif
