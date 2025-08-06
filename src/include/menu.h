//LCD/encoder-based menu system logic with navigable states, user command execution hooks, and integration with cheat codes.
#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

typedef enum {
    MENU_MAIN,
    MENU_SETTINGS,
    MENU_SENSOR_DATA,
    MENU_CHEATCODES,
    MENU_EXIT
} menu_state_t;

// Initialize menu system
void menu_init();

// Display current menu state
void menu_display();

// Handle encoder/button input to navigate menus
void menu_navigate(int input_signal);

// Execute selected menu command
void menu_execute();

#endif // MENU_H
