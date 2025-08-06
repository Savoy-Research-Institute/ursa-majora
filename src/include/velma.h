/*Entry point and high-level coordination logic.

setup(), loop(), system initialization, and runtime control.*/#ifndef VELMA_MAIN_H
#define VELMA_MAIN_H

#include "hc12.h"
#include "cheatcodes.h"
#include "menu.h"
#include "sensors.h"
#include "eeprom_manager.h"
#include "utils.h"

void setup_system();
void main_loop();

#endif // VELMA_MAIN_H
