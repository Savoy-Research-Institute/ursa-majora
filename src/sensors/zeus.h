//this is the cheatcode library it contains the cheat code execution scripts, 
/*each cheatcode must have a toggle and a variable variable, like a strength, */
#ifndef ZEUS_H
#define ZEUS_H

#include <Arduino.h>

class Zeus {
public:
    Zeus();
    void init();
    bool executeCheatCode(const char* code);
    void toggleCheatCodeMode();
    bool isCheatCodeModeActive() const;

private:
    bool cheatCodeMode;
    static const int NUM_CHEAT_CODES = 9;
    const char* cheatCodes[NUM_CHEAT_CODES] = {
        "cruise", "Shepherd", "Turtle_mode", "Cleopatra", 
        "osiris", "anubis", "Earthmagnet", "Pussypower", "Nightcrawler"
    };

    void night_crawler();
    void shepherd();
    void pussy_power();
    void cleopatra();
    void cruise();
    void osiris();
    void anubis();
    void turtle_mode();
    void earth_magnet();
};

#endif // ZEUS_H