//Utility functions: string parsing, logging, generic data processing helpers.
#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

// Convert float to formatted string with given decimals
String float_to_string(float value, uint8_t decimals);

// Log debug message over serial
void debug_log(String message);

// Generic string splitter
String split_string(String data, char delimiter, int index);

#endif // UTILS_H

