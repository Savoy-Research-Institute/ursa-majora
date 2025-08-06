#ifndef CONSOLE_INTERFACE_H
#define CONSOLE_INTERFACE_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define VOLUME_PIN A0
const int ROWS = 4; // four rows
const int COLS = 4; // four columns
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

// connect the pins from right to left to pin 2, 3, 4, 5,6,7,8,9
const int rowPins[ROWS] = {5, 4, 3, 2}; // connect to the row pinouts of the keypad
const int colPins[COLS] = {9, 8, 7, 6}; // connect to the column pinouts of the keypad

class ConsoleInterface
{
public:
    ConsoleInterface();
    void begin();
    void update();
    bool runKeypadTest();

private:
    enum class Screen
    {
        Default,
        MainMenu,
        DeviceSettings,
        DroneSettings,
        BrightnessSettings,
        ContrastSettings,
        PidSettings,
        FailsafeSettings,
        CheatCode
    };

    LiquidCrystal_I2C lcd;
    // You'll need to properly initialize the keypad
    Keypad keypad;

    Screen currentScreen;
    unsigned long lastActivityTime;
    int menuPosition;
    int totalMenuItems;
    bool isDroneConnected;
    String cheatCodeBuffer;
    int updateRate; // in milliseconds
    int brightness; // New members for brightness and contrast control
    int contrast;
    bool inBrightnessMode;

    void showDefaultScreen();
    void showMainMenu();
    void showDeviceSettings();
    void showDroneSettings();
    void showCheatCode();

    void handleKeyPress(char key);
    void handleVolumePress();
    void handleVolumeRotation(int direction);
    void checkTimeout();
    bool confirmAction(const String &action);

    // New helper functions
    void updateKeypad();
    void updateVolume();
    void handleSelection();
    void handleCheatCode();
    void scrollMenu(int direction);

    // Helper functions
    void updateGPS();
    void updateBattery();
    void updateHeading();
    const char keypadOrder[16] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D'};
    bool keypadTestStatus[16] = {false};

    // New functions for brightness and contrast control
    void adjustBrightness(int direction);
    void adjustContrast(int direction);
    void toggleBrightnessContrastMode();

    // New function for additional settings
    void showAdditionalSettings();

    // Keypad configuration
    // static const byte ROWS = 4;
    // static const byte COLS = 4;
    // static const char keys[ROWS][COLS];
    // static const byte rowPins[ROWS];
    // static const byte colPins[COLS];
};

ConsoleInterface::ConsoleInterface()
    : lcd(0x27, 16, 2), // Adjust address and dimensions as needed
      currentScreen(Screen::Default),
      lastActivityTime(0),
      menuPosition(0),
      totalMenuItems(0),
      isDroneConnected(false),
      updateRate(1000) // Default update rate: 1 second
      brightness(128),
      contrast(128),
      inBrightnessMode(true)
{
}

void ConsoleInterface::begin()
{
    lcd.init();
    lcd.backlight();
    pinMode(VOLUME_PIN, INPUT_PULLUP);
    // Initialize other components as necessary
}

void ConsoleInterface::update()
{
    updateKeypad();
    updateVolume();
    checkTimeout();

    static unsigned long lastUpdateTime = 0;
    if (millis() - lastUpdateTime >= updateRate)
    {
        switch (currentScreen)
        {
        case Screen::Default:
            showDefaultScreen();
            break;
        case Screen::MainMenu:
            showMainMenu();
            break;
        case Screen::DeviceSettings:
            showDeviceSettings();
            break;
        case Screen::DroneSettings:
            showDroneSettings();
            break;
        case Screen::CheatCode:
            showCheatCode();
            break;
        default:
            Serial.println(F("Error: Unknown screen state"));
            break;
        }
        lastUpdateTime = millis();
    }
}

void ConsoleInterface::updateKeypad()
{
    // You'll need to implement key reading based on your keypad setup
    char key = keypad.getKey();

    if (key)
    {
        handleKeyPress(key);
        lastActivityTime = millis();
    }
}

void ConsoleInterface::updateVolume()
{
    static bool lastVolumeState = HIGH;
    bool currentVolumeState = digitalRead(VOLUME_PIN);
    if (lastVolumeState == HIGH && currentVolumeState == LOW)
    {
        handleVolumePress();
        lastActivityTime = millis();
    }
    lastVolumeState = currentVolumeState;

    static int lastVolumeValue = analogRead(VOLUME_PIN);
    int currentVolumeValue = analogRead(VOLUME_PIN);
    if (currentVolumeValue != lastVolumeValue)
    {
        handleVolumeRotation(currentVolumeValue - lastVolumeValue);
        lastVolumeValue = currentVolumeValue;
        lastActivityTime = millis();
    }
}

void ConsoleInterface::handleKeyPress(char key)
{
    switch (currentScreen)
    {
    case Screen::Default: // first of all, from default screen, pressing D enters menu mode
        if (key == 'D')
            currentScreen = Screen::MainMenu;
        break;
    case Screen::MainMenu:
    case Screen::DeviceSettings:
        if (key == 'A')
            scrollMenu(-1); // A goes up in menu mode
        else if (key == 'C')
            scrollMenu(1); // C goes down in menu mode
        else if (key == 'B')
            handleSelection(); // B selects option
        else if (key == 'D')
            currentScreen = Screen::Default; // pressing D from menu mode exits it
        break;
    case Screen::DroneSettings:
        if (key == 'A')
            scrollMenu(-1); // A goes up in menu mode
        else if (key == 'C')
            scrollMenu(1); // C goes down in menu mode
        else if (key == 'B')
            handleSelection(); // B selects option
        else if (key == 'D')
            currentScreen = Screen::Default; // pressing D from menu mode exits it
        break;
    case Screen::CheatCode:
        if (key >= 'A' && key <= 'Z')
        { // on this mode, pressing any alphabet key adds it to the cursor,
            cheatCodeBuffer += key;
        }
        else if (key == '#')
        { // pressing # after keying in the cheat selects it
            handleCheatCode();
        }
        break;
    case Screen::BrightnessSettings:
    case Screen::PidSettings:
    case Screen::ContrastSettings:
    case Screen::FailsafeSettings:
    }
}

void ConsoleInterface::handleVolumePress()
{
    if (currentScreen == Screen::Default)
    { // if on default screen, pressing digital volume enters cheatcode_mode
        currentScreen = Screen::CheatCode;
    }
    else if (currentScreen == Screen::DeviceSettings || currentScreen == Screen::DroneSettings)
    {
        handleSelection(); // if on device or drone settings, pressing digital volume is pressing OK,
    }

    if (currentScreen == Screen::BrightnessSettings)
    {
        toggleBrightnessContrastMode();
    }
    else if (currentScreen == Screen::ContrastSettings)
    {
        handleSelection(); //////anomaly, code not finished
        adjustContrast();
    }
}

void ConsoleInterface::handleVolumeRotation(int direction)
{
    switch (currentScreen)
    {
    case Screen::MainMenu:
    case Screen::DeviceSettings:
        scrollMenu(direction);
        break;
    case Screen::DroneSettings:
        scrollMenu(direction);
        break;
    case Screen::BrightnessSettings: // explain this in detail............
        adjustBrightness(direction);
    case Screen::ContrastSettings:
        adjustContrast(direction);
        break;
    }
    // Handle variable adjustments in settings screens

}

void ConsoleInterface::checkTimeout()
{
    if (currentScreen != Screen::Default && millis() - lastActivityTime > 5000)
    {
        currentScreen = Screen::Default;
    }
}

bool ConsoleInterface::confirmAction(const String &action)
{
    lcd.clear();
    lcd.print(action);
    lcd.setCursor(0, 1);
    lcd.print("Y:B N:D");

    unsigned long startTime = millis();
    while (millis() - startTime < 10000)
    { // 10 seconds timeout
        char key = keypad.getKey();
        if (key == 'B')
            return true;
        if (key == 'D')
            return false;
    }
    return false; // Timeout or user pressed no
}

void ConsoleInterface::handleSelection()
{
    // Implement selection handling logic
    // This will depend on the current screen and menu position
    switch (currentScreen)
    {
    case Screen::MainMenu:
        // Handle main menu selection
        break;
    case Screen::DeviceSettings:
        // Handle device settings selection
        break;
    case Screen::DroneSettings:
        // Handle drone settings selection
        break;
    default:
        Serial.println(F("Error: Unexpected selection handling"));
        break;
    }
}
bool ConsoleInterface::runKeypadTest()
{
    lcd.clear();
    lcd.print("Keypad Test");
    lcd.setCursor(0, 1);
    lcd.print("Press keys in order");

    unsigned long startTime = millis();
    int currentKeyIndex = 0;
    bool testComplete = false;

    while (!testComplete && (millis() - startTime < 60000))
    { // 1 minute timeout
        char key = keypad.getKey();

        if (key)
        {
            if (key == keypadOrder[currentKeyIndex])
            {
                keypadTestStatus[currentKeyIndex] = true;
                currentKeyIndex++;

                // Update LCD
                lcd.clear();
                lcd.print("Key ");
                lcd.print(key);
                lcd.print(" OK");

                if (currentKeyIndex == 16)
                {
                    testComplete = true;
                }
            }
            else if (key == 'D')
            {
                // Allow exiting the test early
                break;
            }
        }

        // Non-blocking delay
        delay(10);
    }

    // Display results
    lcd.clear();
    lcd.print("Test ");
    lcd.print(testComplete ? "Complete" : "Incomplete");

    // Check for any failed keys
    bool allKeysWorking = true;
    for (int i = 0; i < 16; i++)
    {
        if (!keypadTestStatus[i])
        {
            allKeysWorking = false;
            break;
        }
    }

    lcd.setCursor(0, 1);
    lcd.print(allKeysWorking ? "All keys OK" : "Some keys failed");

    // Reset test status
    memset(keypadTestStatus, 0, sizeof(keypadTestStatus));

    delay(2000); // Display result for 2 seconds

    return allKeysWorking;
}
void ConsoleInterface::handleCheatCode()
{
    // Implement cheat code handling logic
    Serial.print(F("Sending cheat code: "));
    Serial.println(cheatCodeBuffer);
    // Add code to send cheat code to drone
    cheatCodeBuffer = ""; // Clear buffer after sending
}

void ConsoleInterface::showDefaultScreen()
{
    lcd.clear();
    updateGPS();
    updateBattery();
    updateHeading();
}
// Implementation of new functions

void ConsoleInterface::adjustBrightness(int direction)
{
    brightness = constrain(brightness + direction, 0, 255);
    lcd.setBacklight(brightness);
}

void ConsoleInterface::adjustContrast(int direction)
{
    contrast = constrain(contrast + direction, 0, 255);
    lcd.setContrast(contrast);
}

void ConsoleInterface::toggleBrightnessContrastMode()
{
    inBrightnessMode = !inBrightnessMode;
    lcd.clear();
    lcd.print(inBrightnessMode ? "Brightness" : "Contrast");
    lcd.setCursor(0, 1);
    lcd.print(inBrightnessMode ? brightness : contrast);
}

void ConsoleInterface::showMainMenu()
{
    lcd.clear();
    lcd.print("1. Device Settings");
    lcd.setCursor(0, 1);
    lcd.print("2. Drone Settings");
    // Handle menu navigation and selection
}
void ConsoleInterface::showDeviceSettings()
{
    lcd.clear();
    lcd.print("1. brightness Settings");
    lcd.setCursor(0, 1);
    lcd.print("2. contrast Settings");
    // Implement device settings display
}

void ConsoleInterface::showDroneSettings()
{
    lcd.clear();
    lcd.print("1. PID Settings");
    lcd.setCursor(0, 1);
    lcd.print("2. Failsafe Settings");
    // Implement drone settings display
}

void ConsoleInterface::showAdditionalSettings()
{
    lcd.clear();
    lcd.print("Additional Settings");
    // Implement your additional settings logic here
}
void ConsoleInterface::showCheatCode()
{
    lcd.clear();
    lcd.print("Cheat Code: ");
    lcd.setCursor(0, 1);
    lcd.print(cheatCodeBuffer);
}

void ConsoleInterface::scrollMenu(int direction)
{
    menuPosition += direction;
    if (menuPosition < 0)
        menuPosition = totalMenuItems - 1;
    if (menuPosition >= totalMenuItems)
        menuPosition = 0;
}

void ConsoleInterface::updateGPS()
{
    // Implement GPS update logic
}

void ConsoleInterface::updateBattery()
{
    // Implement battery update logic
}

void ConsoleInterface::updateHeading()
{
    // Implement heading update logic
}

#endif // CONSOLE_INTERFACE_H