#include "velma_main.h"

void setup_system() {
    // Initialize all modules
    hc12_init(9600);
    cheatcodes_init();
    menu_init();
    sensors_init();
    eeprom_init();
}

void main_loop() {
    // Main program loop
    menu_display();
    // Add other periodic tasks here
}
