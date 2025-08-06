//blizzard library on velma
/*what it should do
*/

/*Cheat codes
Cruise use height above ground and absolute altitude to perform autopilot, maintain heading, velocity etc,
Shepherd beepers help locate at close distances, onboard flashlights and LEDs help locate at night, GPS data used to locate during a crash
Nightcrawler the camera brick uses onboard IR LEDs to simulate light amplification night vision
Earthmagnet use height above ground to lock altitude via a switch,
Ppower, uses gyroscope and accelerometer data,
Turtle_mode
Osiris
Cleopatra


change this such that, if i have one cheat code x, the switches will have functions x1a to x4a,
if i have cheat code d, the switches will have function d1a, im just giving examples for context,
if i write the cheat code  i can access its capabilities and map them to the switches,  i can toggle a switch,  click,
map it to a cheatcode, map a functionality to the digital volume,
*/
bool cheatCodeMode = false; // Variable to track cheat code mode
// couple this with the hc12, such that it toggles immediately,
//  Define cheat codes and corresponding actions
// make a list store all cheat codes, the index in that list, defines the switch case to use, so that checkCheatcode runs received strings, thru this list

const char *cheatCode1 = "osiris"; // Example cheat code
const char *cheatCode2 = "anubis"; // Another example cheat code
const char *cheatCode3 = "Cleopatra";
const char *cheatCode4 = "Turtle_mode";
const char *cheatCode5 = "Pussypower";   // stabilization algorithms to calculate error between greedy orientation and upright, correction kinda like how a cat can
const char *cheatCode6 = "Earthmagnet";  // use height above ground to lock altitude
const char *cheatCode7 = "Nightcrawler"; // the camera brick uses onboard IR LEDs to simulate light amplification night vision
const char *cheatCode8 = "Cruise";       // use height above ground and absolute altitude to perform autopilot, maintain heading, velocity etc,
const char *cheatCode9 = "Shepherd";     // beepers help locate at close distances, onboard flashlights and LEDs help locate at night, GPS data used to locate during a crash
const char *cheatCodes[] = {"cruise", "Shepherd", "Turtle_mode", "Cleopatra", "osiris", "anubis", "Earthmagnet", "Pussypower", "Nightcrawler"};




void start_clock()
{
    // run timers from gps time data, keep a local clock and remind(update) every 10 minutes,
    // capacitors needed,
    // trigger pin, read pin,
    // a 16bit adder,
}

bool accessGranted = false; // Variable to track access status

// Define access code
const char *accessCode = "access123"; // Example access code

void checkAccessCode(const char *receivedCode)
{
    // Compare received code with predefined access code
    if (strcmp(receivedCode, accessCode) == 0)
    {
        // Access code matches, grant access
        accessGranted = true;
        // Implement access granted functionality (e.g., display message on LCD)
    }
    else
    {
        // Access code does not match, deny access
        accessGranted = false;
        // Implement access denied functionality (e.g., display error message on LCD)
    }
}

void receiveAccessCode(const char *receivedCode)
{
    // Check received access code
    checkAccessCode(receivedCode);
}

/*HC12 transciever connect to blizzard, the read pin is under an interrupt,
Terminal communication with blizzard, the terminal receives text, inteprets for the drone, CMD, cursor placement
*/
//~~~~~~~~~~~~~~~~~~~~~HC12 functions~~~~~~~~~~~~~~~~~~~~~~~~~
void switch_hc12_on()
{
    // Turn the transistor on, only when off
    if (!digitalRead(hc12_transistor_pin))
    {
        digitalWrite(hc12_transistor_pin, HIGH);
    }
}

// Function to switch the module off
// hardware serial on interrupt, it always listens, if manual controlled and docked, it starts to receive 8channels,
// controls the drone manually, enter cheatcodes, etc,
void switch_hc12_off()
{
    // Turn the transistor off, only when on
    if (digitalRead(hc12_transistor_pin))
    {
        digitalWrite(hc12_transistor_pin, LOW);
    }
}

void enterCheatCodeMode()
{
    // Activate cheat code mode
    cheatCodeMode = true;
    // Display blinking cursor on LCD
    // Implement LCD blinking cursor functionality
}
void exitCheatCodeMode()
{
    // Deactivate cheat code mode
    cheatCodeMode = false;
    // Clear LCD
    // Implement LCD clear functionality
}

void checkCheatCode(const char *enteredCode)
{
    // Compare entered code with predefined cheat codes
    if (strcmp(enteredCode, cheatCode1) == 0)
    {
        // Execute corresponding action for cheat code 1
        // For example, toggle Blizzard and transmit message via HC12
        // Implement Blizzard toggling and HC12 transmission
    }
    else if (strcmp(enteredCode, cheatCode2) == 0)
    {
        // Execute corresponding action for cheat code 2
        // Implement desired functionality
    }
    else if (strcmp(enteredCode, cheatCode3) == 0)
    {
        // Execute corresponding action for cheat code 3
        // Implement desired functionality
    }
    else if (strcmp(enteredCode, cheatCode4) == 0)
    {
        // Execute corresponding action for cheat code 4
        // Implement desired functionality
    }
    else if (strcmp(enteredCode, cheatCode5) == 0)
    {
        // Execute corresponding action for cheat code 5
        // Implement desired functionality
    }
    else if (strcmp(enteredCode, cheatCode6) == 0)
    {
        // Execute corresponding action for cheat code 6
        // Implement desired functionality
    }
    else if (strcmp(enteredCode, cheatCode7) == 0)
    {
        // Execute corresponding action for cheat code 7
        // Implement desired functionality
    }
    else if (strcmp(enteredCode, cheatCode8) == 0)
    {
        // Execute corresponding action for cheat code 2
        // Implement desired functionality
    }
    else if (strcmp(enteredCode, cheatCode9) == 0)
    {
        // Execute corresponding action for cheat code 2
        // Implement desired functionality
    }
    else
    {
        // Invalid cheat code entered
        // Display error message on LCD or take appropriate action
    }
}

void handleCheatCodeInput(char key)
{                                  // this is one cheatcode per time,
    static char enteredCode[20];   // Array to store entered cheat code
    static int cursorPosition = 0; // Cursor position in entered code

    if (key == 'B')
    {
        // User confirms entered cheat code
        enteredCode[cursorPosition] = '\0'; // Null terminate entered code
        checkCheatCode(enteredCode);        // Check entered cheat code
        exitCheatCodeMode();                // Exit cheat code mode
    }
    else
    {
        // Append pressed key to entered cheat code
        enteredCode[cursorPosition++] = key;
        // Display entered code on LCD
        // Implement LCD display functionality
    }
}

void handleAuxSwitchInput(int switchNumber)
{
    if (cheatCodeMode)
    {
        // Handle switch input only if in cheat code mode
        // Map switch number to corresponding key press (e.g., A, B, C, D)
        char key;
        switch (switchNumber)
        {
        case 1:
            key = 'A'; // Replace code with radio switches code,
            break;
        case 2:
            key = 'B';
            break;
        case 3:
            key = 'C';
            break;
        case 4:
            key = 'D';
            break;
        default:
            // Invalid switch number
            return;
        }
        handleCheatCodeInput(key); // Handle cheat code input
    }
}


