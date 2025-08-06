#include "CustomEEPROM.h"

void CustomEEPROM::name(const String &name) {
    int len = name.length();
    EEPROM.write(0, len); // Store the length of the name at the first byte

    for (int i = 0; i < len; ++i) {
        EEPROM.write(i + 1, name[i]); // Store each character of the name
    }
}

String CustomEEPROM::readName() {
    int len = EEPROM.read(0); // Read the length of the name
    char name[len + 1];

    for (int i = 0; i < len; ++i) {
        name[i] = EEPROM.read(i + 1); // Read each character of the name
    }
    name[len] = '\0'; // Null-terminate the string

    return String(name);
}
