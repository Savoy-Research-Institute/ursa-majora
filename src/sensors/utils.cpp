#include "utils.h"

String float_to_string(float value, uint8_t decimals) {
    return String(value, decimals);
}

void debug_log(String message) {
    Serial.println("[DEBUG] " + message);
}

String split_string(String data, char delimiter, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == delimiter || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }

    return (found > index) ? data.substring(strIndex[0], strIndex[1]) : "";
}
