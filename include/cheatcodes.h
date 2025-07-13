//Cheat code parsing engine for command shortcuts, maintenance commands, or developer access codes.
#ifndef CHEATCODES_H
#define CHEATCODES_H

#include <Arduino.h>

// Initialize cheat code parser
void cheatcodes_init();

// Parse input string for cheat codes
void parse_cheatcode(String input);

// Execute action associated with parsed cheat code
void execute_cheatcode(String code);

#endif // CHEATCODES_H
