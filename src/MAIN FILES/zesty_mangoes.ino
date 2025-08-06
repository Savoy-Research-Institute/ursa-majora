// This is code to run on  the blizzard handheld device
//#include <LiquidCrystal.h>
#include <Wire.h>              // Include the Wire library for I2C communication
#include <LiquidCrystal_I2C.h> // Include the LiquidCrystal_I2C library for I2C LCD
#include <Keypad.h>
#include <SoftwareSerial.h>
#include "BlizzardHC12.h"  // Custom library for HC-12 communication
#include "BlizzardMenu.h"  // Custom library for menu system
#include "BlizzardCheatCodes.h"  // Custom library for cheat code handling
// profiles for different people, status meaning the functionalities one can perform using the drone, i can reduce or increase parent influence to child for flight on a scale of one to ten,,
//this is the main run file for Blizzard//blizzard library on velma
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



// Blizzard Console Main Code

// Pin Definitions
const int VOLUME_PIN = A0;
const int HC12_TX_PIN = 10;
const int HC12_RX_PIN = 11;

// Keypad Setup, Initialize the keypad
// Define the pin connections for the keypad matrix
const int ROWS = 4;
const int COLS = 4;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {9, 8, 7, 6}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD Setup, Define the LCD address and dimensions
LiquidCrystal_I2C lcd(0x27, 16, 2); // Address 0x27, 16 columns, 2 rows
// HC-12 Setup
SoftwareSerial hc12(HC12_RX_PIN, HC12_TX_PIN);
BlizzardHC12 blizzardHC12(hc12);

// Menu System
BlizzardMenu menu;

// Cheat Code System
BlizzardCheatCodes cheatCodes;

// Global Variables
bool accessGranted = false;
bool cheatCodeMode = false;




void setup() {
  Serial.begin(9600);
  
  // Initialize hardware
  initializeHardware();
  
  // Setup menu system
  setupMenu();
  
  // Setup cheat code system
  setupCheatCodes();
  
  // Display startup message
  displayStartupMessage();
}

void loop() {
  // Check for keypad input
  char key = keypad.getKey();
  if (key != NO_KEY) {
    handleKeypadInput(key);
  }
  
  // Check for volume control input
  int volumeValue = analogRead(VOLUME_PIN);
  handleVolumeControl(volumeValue);
  
  // Check for incoming HC-12 messages
  if (blizzardHC12.dataAvailable()) {
    String message = blizzardHC12.receiveMessage();
    handleIncomingMessage(message);
  }
  
  // Update screen saver if needed
  updateScreenSaver();
}

void initializeHardware() {
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Initialize HC-12
  blizzardHC12.begin(9600);
  
  // Initialize volume control pin
  pinMode(VOLUME_PIN, INPUT);
  
  // Blink LED twice on startup
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
}
// Function to initialize hardware components
void initializeHardware(){
    // Initialize keypad
    setupKeypad();//test a random key, 

    // Initialize LCD display
    setupLCD();

    // Initialize digital volume control
    setupVolumeControl();

    // Initialize LED status indicators
    setupLEDs();

    // Initialize HC12 transceiver
    setupHC12();

    // Initialize radio transmitter (FSi6 X)
    setupRadioTransmitter();
    
    setupGPS();
    // Confirm initialization
    Serial.print ("Hardware initialization complete");
}

void setupMenu() {
  menu.addOption("Device Settings");
  menu.addOption("Drone Settings");
  menu.addOption("Cheat Codes");
  menu.addOption("Back");
}

void setupCheatCodes() {
  cheatCodes.addCheatCode("osiris", executeOsiris);
  cheatCodes.addCheatCode("anubis", executeAnubis);
  cheatCodes.addCheatCode("cleopatra", executeCleopatra);
  // Add more cheat codes as needed
}

void displayStartupMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Blizzard Console");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
}

void handleKeypadInput(char key) {
  if (cheatCodeMode) {
    handleCheatCodeInput(key);
  } else if (accessGranted) {
    menu.handleInput(key);
  } else {
    handleAccessCodeInput(key);
  }
}

void handleVolumeControl(int volumeValue) {
  static int previousVolume = 0;
  if (volumeValue != previousVolume) {
    if (volumeValue > previousVolume) {
      menu.scrollUp();
    } else {
      menu.scrollDown();
    }
    previousVolume = volumeValue;
  }
}

void handleIncomingMessage(String message) {
  if (message == "REQUEST_ACK") {
    blizzardHC12.sendMessage("ACK");
  } else {
    // Handle other incoming messages
    Serial.println("Received: " + message);
  }
}

void updateScreenSaver() {
  static unsigned long lastUpdateTime = 0;
  if (millis() - lastUpdateTime > 10000) {  // Update every 10 seconds
    displayScreenSaver();
    lastUpdateTime = millis();
  }
}

void displayScreenSaver() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Blizzard Console");
  lcd.setCursor(0, 1);
  // Display relevant information like battery level, time, etc.
}

void handleCheatCodeInput(char key) {
  if (key == 'B') {
    String enteredCode = cheatCodes.getEnteredCode();
    if (cheatCodes.executeCheatCode(enteredCode)) {
      lcd.clear();
      lcd.print("Cheat activated!");
    } else {
      lcd.clear();
      lcd.print("Invalid cheat!");
    }
    cheatCodeMode = false;
  } else if (key == 'D') {
    cheatCodeMode = false;
    lcd.clear();
    lcd.print("Cheat mode exit");
  } else {
    cheatCodes.addKeyToCode(key);
    lcd.clear();
    lcd.print(cheatCodes.getEnteredCode());
  }
}

void handleAccessCodeInput(char key) {
  static String enteredCode = "";
  if (key == 'B') {
    if (checkAccessCode(enteredCode)) {
      accessGranted = true;
      lcd.clear();
      lcd.print("Access granted!");
    } else {
      lcd.clear();
      lcd.print("Access denied!");
    }
    enteredCode = "";
  } else if (key == 'D') {
    enteredCode = "";
    lcd.clear();
    lcd.print("Code cleared");
  } else {
    enteredCode += key;
    lcd.clear();
    lcd.print(enteredCode);
  }
}

bool checkAccessCode(String code) {
  // Implement your access code checking logic here
  return code == "1234";  // Example access code
}

// Cheat code execution functions
void executeOsiris() {
  // Implement Osiris cheat code functionality
}

void executeAnubis() {
  // Implement Anubis cheat code functionality
}

void executeCleopatra() {
  // Implement Cleopatra cheat code functionality
}






char lastKey = '\0';
unsigned long lastPressTime = 0;
int pressCount = 0;
bool cheatCodeMode = 0;
const unsigned long pressTimeout = 1000; // 1 second timeout for cycling characters
bool parent == 1;                        // change to zero for child console
bool accessGranted = false;              // Variable to track access status
bool accessGranted = false;              // Variable to track access status
enum InputMode
{
  NUMERIC,
  ALPHABETIC,
  SYMBOLS
};
// Menu options
enum MenuOption{
    DEVICE_SETTINGS,
    DRONE_SETTINGS,
    CHEAT_CODES,
    BACK
}
// Define menu options
enum MenuOption
{
  OPTION_1,
  OPTION_2,
  OPTION_3,
  OPTION_4,
  BACK
};

InputMode currentMode = NUMERIC;
const unsigned long keyPressTimeout = 1000; // 1 second timeout
const char *keyMap[10] = {
    " ",    // 0
    ".,?!", // 1
    "ABC",  // 2
    "DEF",  // 3
    "GHI",  // 4
    "JKL",  // 5
    "MNO",  // 6
    "PQRS", // 7
    "TUV",  // 8
    "WXYZ"  // 9
};

/*on startup, the blizzard will calibrate its gyro, and when tasked via cleopatra, it will use the tilt data to control the fine movements of the drone
 */
int A_level_password = 555555; // Admin password,
int B_level_password = 444444; // Parent_console
int C_level_password = 333333; // Child_console

MenuOption currentOption = OPTION_1; // Initial menu option



// Define pin for digital volume control
const int volumePin = A0;

void setup()
{
  // Initialize Serial communication for debugging
  Serial.begin(9600);
  // Blink LED twice on startup
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 2; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH); // Turn LED on
    delay(500);                      // Wait for 500 milliseconds
    digitalWrite(LED_BUILTIN, LOW);  // Turn LED off
    delay(500);                      // Wait for 500 milliseconds
  }
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Drone Control");

  // Initialize radio transmitter
  // (Assuming you have already configured this part)
//initialize keypad
  // Set up digital volume control pin as input
  pinMode(volumePin, INPUT);
}

void loop()
{

}
function mainLoop():
    initializeHC12();
    initializeKeypad();
    initializeLCD();


//LEVEL A CODE, this is code that the blizzard uses to interface with hardware, and to basically run without doing much, 

// Initialize the keypad
void initializeKeypad(){
    setupKeypadPins();
    configureKeypadMatrix();
}

void read_keypad()
{
  // Get keypad input
  char key = keypad.getKey(); // Get key pressed
  if (key != NO_KEY)
  {
    handleKeypadInput(key); // Handle keypad input
  }
}
// Function to read keypad input
void readKeypad(){
    key = getKeypadInput();
    return key

}

void handleKeypadInput(char key)
{ // when a key is pressed, what do we do, (menu commands)
  switch (key)
  {
  case 'A':
    if (currentOption > OPTION_1)
    {
      currentOption = static_cast<MenuOption>(currentOption - 1);
    }
    break;
  case 'C':
    if (currentOption < BACK)
    {
      currentOption = static_cast<MenuOption>(currentOption + 1);
    }
    break;
  case 'B':
    // Select current option
    // Implement logic for selected option
    break;
  case 'D':
    currentOption = BACK;
    break;
  case '#':
    switchInputMode();
    break;
  default:
    handleCharacterInput(key);
    break;
  }
}

// Function to handle keypad input
void handleKeypadInputB(key){
    switch (key):
        case 'A':
            if (currentOption > DEVICE_SETTINGS){
                currentOption -= 1}
        case 'C':
            if currentOption < BACK:
                currentOption += 1
        case 'B':
            if currentOption == CHEAT_CODES:
                handleCheatCodeInput()
            else:
                // Implement logic for other selected options
        case 'D':
            currentOption = BACK

}
void switchInputMode()
{
  switch (currentMode)
  {
  case NUMERIC:
    currentMode = ALPHABETIC;
    break;
  case ALPHABETIC:
    currentMode = SYMBOLS;
    break;
  case SYMBOLS:
    currentMode = NUMERIC;
    break;
  }
}
void handleCharacterInput(char key)
{ // divides the keypad inputs to numeric, alphabetic, symbols,
  switch (currentMode)
  {
  case NUMERIC:
    if (key >= '0' && key <= '9')
    {
      Serial.print(key); // or store the number as needed
    }
    break;
  case ALPHABETIC:
    handleAlphabeticInput(key);
    break;
  case SYMBOLS:
    handleSymbolInput(key);
    break;
  }
}
void handleAlphabeticInput(char key)
{ // handle what comes out when keys are pressed, and input format is ALPHABET
  unsigned long currentTime = millis();

  // Reset press count if a different key is pressed or timeout occurred
  if (key != lastKey || (currentTime - lastPressTime) > pressTimeout)
  {
    lastKey = key;
    pressCount = 0;
  }

  lastPressTime = currentTime;

  // Define the character sets for each key
  const char *chars = nullptr;
  int charCount = 0;

  switch (key)
  {
  case '2':
    chars = "ABC";
    charCount = 3;
    break;
  case '3':
    chars = "DEF";
    charCount = 3;
    break;
  case '4':
    chars = "GHI";
    charCount = 3;
    break;
  case '5':
    chars = "JKL";
    charCount = 3;
    break;
  case '6':
    chars = "MNO";
    charCount = 3;
    break;
  case '7':
    chars = "PQRS";
    charCount = 4;
    break;
  case '8':
    chars = "TUV";
    charCount = 3;
    break;
  case '9':
    chars = "WXYZ";
    charCount = 4;
    break;
  default:
    return; // Ignore other keys
  }

  void handleAlphabeticInput_2(char key)
  {
    if (key < '0' || key > '9')
      return;

    unsigned long currentTime = millis();

    if (key == lastKeyPressed && (currentTime - lastKeyPressTime) < keyPressTimeout)
    {
      charIndex = (charIndex + 1) % strlen(keyMap[key - '0']);
    }
    else
    {
      charIndex = 0;
    }

    char letter = keyMap[key - '0'][charIndex];
    Serial.print(letter); // or store the letter as needed

    lastKeyPressed = key;
    lastKeyPressTime = currentTime;
  }

  // Cycle through characters
  char letter = chars[pressCount % charCount];
  Serial.print(letter); // or store the letter as needed

  pressCount++;
}
void handleSymbolInput(char key)
{ // whichever key is pressed when he input format is SYMBOLS,
  // Map keys to symbols
  char symbol = ' ';
  switch (key)
  {
  case '1':
    symbol = '!';
    break;
  case '2':
    symbol = '@';
    break;
  case '3':
    symbol = '#';
    break;
  case '4':
    symbol = '$';
    break;
  case '5':
    symbol = '%';
    break;
  case '6':
    symbol = '^';
    break;
  case '7':
    symbol = '&';
    break;
  case '8':
    symbol = '*';
    break;
  case '9':
    symbol = '(';
    break;
  case '0':
    symbol = ')';
    break;
  }
  Serial.print(symbol); // or store the symbol as needed
}
void handleVolumeControl(int volumeValue)
{ // handle digital volume movement at the hardware level, right and left movement, and clicker
  static int previousVolume = 0;
  static unsigned long previousVolumeChangeTime = 0;

  // Detect clockwise rotation
  if (volumeValue > previousVolume)
  {
    unsigned long currentTime = millis();
    if (currentTime - previousVolumeChangeTime > 500)
    { // Anti-bounce delay
      if (currentOption > OPTION_1)
      {
        currentOption = static_cast<MenuOption>(currentOption - 1);
      }
      previousVolumeChangeTime = currentTime;
    }
  }

// Function to handle digital volume control input
void readVolumeControl(){
    value = getVolumeControlInput()
    return value
}
void read_digital_volume()
{
  // Read digital volume control
  int volumeValue = analogRead(volumePin);
  handleVolumeControl(volumeValue); // Handle volume control
}
  // Detect anti-clockwise rotation
  else if (volumeValue < previousVolume)
  {
    unsigned long currentTime = millis();
    if (currentTime - previousVolumeChangeTime > 500)
    { // Anti-bounce delay
      if (currentOption < BACK)
      {
        currentOption = static_cast<MenuOption>(currentOption + 1);
      }
      previousVolumeChangeTime = currentTime;
    }
  }
  // Detect click on volume button
  if (volumeValue == 1023)
  {
    // Implement logic for selecting current option
  }
  // Detect double-click on volume button
  if (volumeValue == 0 && previousVolume == 0)
  {
    unsigned long currentTime = millis();
    if (currentTime - previousVolumeChangeTime < 500)
    { // Check if second click occurred within 500 milliseconds
      currentOption = BACK;
    }
    previousVolumeChangeTime = currentTime;
  }
  previousVolume = volumeValue; // Update previous volume value
}
// Arduino nano interface code
void write_lcd_screen()
{
  // I2C connected screen
}
// Function to display messages on LCD
void displayMessage(String message){
    clearLCD()
    Serial.print("message on LCD", message)
}
lcd.init() {}
lcd.backlight() {}
lcd.clear() {}
lcd.setCursor(0, 0) {}
lcd.print("Drone Control") {}
// Initialize the LCD screen
void initializeLCD(){
    setupLCDPins();
    configureLCD();
}

void screen_saver_on()
{
  // turn screensaver on,
  // on device menu, choose what to display,
}
void screen_saver_off()
{
}
void generate_local_time()
{
  // run timers from gps time data, keep a local clock and remind(update) every 10 minutes,
  // capacitors needed,
  // trigger pin, read pin,
  // a 16bit adder,
}
void read_console(){
// Function to handle keypad input for menu navigation
}



// Function to check for hardware errors
function checkHardwareErrors():
    if any hardware initialization fails:
        print "Error: Hardware initialization failed"
        return False
    return True

// Function to log debug information
function logDebugInfo(message):
    print "Debug: " + message

    while (true):
        key = readKeypadInput()
        handleKeypadInput(key)
// Function to handle cheat code input
void handleCheatCodeInputB(){
    cheatCode = ""
    print "Enter cheat code: " on LCD

    while (true){
        key = readKeypadInput();
        if key{
            if (key == 'B'){
                // Confirm and send cheat code
                sendCheatCode(cheatCode)
                print "Cheat code sent: " + cheatCode on LCD
                break}
            else if (key == 'D'){
                // Cancel input
                print "Cheat code input cancelled" on LCD
                break}
            else:
                // Append key to cheat code
                cheatCode += key
                print key on LCD
    }}}


            
//LEVEL B CODE, this is code that the actual code that everything else runs on, flight, stabilization, gps etc,  and my theory is, this code will build upon level A code, 
//kinda like, sherpherd uses LED(ON,OFF) from LEVEL A this is all the dirty stuff, all the AI stuff is here too
//more complexity and functionality than LEVEL A
void calibrate_console(){
  //calibrate the sticks and buttons, the max and min values are assigned to these, the user places them in their extreme positions when prompted, then the blizzard uses these as default, 
}
void change_UID()
{
  // this interface is part of the menu system, allows admins to change drone password,
}
void change_drone_name()
{//toggle the user to change the drone EEPROM settings, like name and unique ID
}
void on_startup()
{
  lcd.init(); // initialize LCD screen and print blizzard logo beeps and leds, red orange and green, in that  sequence, red, just woken up, orange calibrating, green connected, 
  calibrate_mpu6050();
  keep_local_clock();
  initiate_screen_saver();  //display that time on the blizzard screen during screensaver
  //wait for access
  password_interface(); // access_granted is true, the drone does the same thing, on that cube, red not connected, orange calibrating, green for ok, 
  //and I can arm and disarm the drone if docked, the blizzard has a profile system, that checks who is using it and what status hey are on
}

void initiate_screen_saver()
{ // with parameters as on and off, this toggles screensavers on the blizzard screen, its settings equivalent choose what is to be displayed
  // while idle, 10sec on inactivity of blizzard, show screensaver, and when button is pressed, go to menu, etc, or enter cheatcode
}

void keep_local_clock()
{
  //    find time data from gps and initiate a local timer, the blizzard as the drone need local IC timers,
  // if time is different and some data too, encryption gone wrong, later, update at a sample rate chosen by the user,
}
void calibrate_mpu6050()
{ // calculate and account for errors in gyroscope and accelerometer IMUs
  /*on startup, the blizzard will calibrate its gyro, it will rely on that data, such that if the roll is fully deflected and the gyro is not tilted,
      it will not do a full roll deflection, this is why the blizzaed needs a gyro, i might forego using the goggles gyro,  */
}


void stream_()
{
  // this function streams data to the blizzard screen, whichever, because all functions have get parts to retrieve individual data pieces,
}
void prompt_()
{
  // this function changes variables for the drone, or tells the drone what to do basically,
}
void fetch_()
{
  // this function streams data like stream, but once,
}

void prompt_()
{ // duplex communication and instruction to the drone
  // prompt the drone flight characteristics, change height for alt_lock
  // prompt cheat codes,
  // change
  // prompt user for failsafes and errors
  // notify user of low battery
}
void stream_()
{
  // continuously, timed, flow of desired data
}
void fetch_()
{
  // get variable from drone and flush data to screen,
}
void access_password_interface()
{
  // run this then the drone asks for authentication before flight, this password will usually be sent by the sender delivery blizzard to the receiver delivery blizzard
}

void accessMenu() // this toggles menu mode on and off
{
  MenuOption currentOption = OPTION_1; // Initial menu option

  while (true)
  {
    // Update LCD based on current menu option
    switch (currentOption)
    {
    case OPTION_1:
      // Update LCD to display option 1
      break;
    case OPTION_2:
      // Update LCD to display option 2
      break;
    case OPTION_3:
      // Update LCD to display option 3
      break;
    case OPTION_4:
      // Update LCD to display option 4
      break;
    case BACK:
      // Update LCD to indicate going back
      break;
    }

    // Check if 'D' is pressed to exit menu
    if (key == 'D')
    {
      break;
    }

    delay(100); // Delay for stability
  }
}
// Function to initialize menu
function initializeMenu():
    currentOption = MAIN_MENU
function handleMenuInput(key):
    switch key:
        case 'A': // Move up in the menu
            if (currentOption > FIRST_OPTION)
                currentOption = currentOption - 1;
        case 'C': // Move down in the menu
            if currentOption < LAST_OPTION:
                currentOption = currentOption + 1;
        case 'B': // Select current option
            executeMenuOption(currentOption);
        case 'D': // Go back or exit
            currentOption = BACK_OPTION;

// Function to execute selected menu option
void executeMenuOption(option){
    switch option:
        case DEVICE_SETTINGS:
            showDeviceSettings();
        case DRONE_SETTINGS:
            showDroneSettings();
        case EXIT:
            exitMenu();
}



// Function to display menu on LCD
void displayMenu(){
    clearLCD()
    switch (currentOption):
        case DEVICE_SETTINGS:
            print "Device Settings" on LCD
        case DRONE_SETTINGS:
            print "Drone Settings" on LCD
        case CHEAT_CODES:
            print "Cheat Codes" on LCD
        case BACK:
            print "Back" on LCD
}


//Periphanelia, code running for blizzard hardware, 

void access_settings_menu()//this turnes 
{ // dynamic settings, i can change what switch does what,
  //  this accesses the menu than a certain button is pressed
  //  only admins can change password, the code runs once and terminates the variableit used, because PUK is same,
  // you can always unblock it afterwards
}
 // reads channels of the controller
{                   // kinda like radio_read, but the console to the radio device,
  //  if parent, read_parent(){}
  //  if child, read_child(){}
  //  print channel values
  //  get, return each variable separately if need be
}
// childblizzard code
void adaptive_trigger()
{
  // toggle static strength or dynamic strength when the drone is too low to the ground, it vibrates proportionally
  // toggle left_motor_on, left_motor_off, right_motor_on, right_motor_off pattern
  // switch_case for different scenario
}
void switch_left_motor_on()
{
  // if off, turn left vibration motor on
}
void switch_left_motor_off()
{
  // if on, turn left vibration motor on
}
void switch_right_motor_on()
{
  // if off, turn right vibration motor on
}
void switch_right_motor_off()
{
  // if on, turn right vibration motor on
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
// Initialize the HC-12 module
void initializeHC12(){
    setupSerialCommunication();
    }
void read_hc12()
{ // read data from transciever when available
  if (mySerial.available())
  {
    String message = mySerial.readStringUntil('\n');
    Serial.println("Message received: " + message);
  }
}
void write_hc12(const String &message)
{
  // Write data to the drone through the transceiver,
  mySerial.println(message);
  Serial.println("Message sent: " + message);
  delay(1000); // Optional delay after sending the message
}
// Function to send data via HC12
void sendDataHC12(data){
    write to HC12(data)
    print "Message sent: " + data
}
// Function to receive data via HC12
void receiveDataHC12(){
    if data available in HC12 buffer:
        message = read from HC12
        print "Message received: " + message
        return message
}
// Function to send cheat code via HC-12
void sendCheatCode(cheatCode){
    transmitViaHC12(cheatCode);
    }
void sendAccessCode(const char *receivedCode) // the user types in a password, which the drone compares to what is stored in the EEPROM,
{
  // Check received access code
  checkAccessCode(receivedCode);
}

void transmitAccessCode()
{
  // Transmit access code via HC12
  // Implement HC12 transmission of access code
}
  /*cheatcodetoggling
  sherpherd code, the blizzard takes drone coordinates, and its own coordinates,
  and calculates heading, it uses the greedy heading from a magnetometer, and gives a heading one should follow,
  */
  

SoftwareSerial hc12(10, 11); // RX, TX

void setup() {
  Serial.begin(9600);  // Serial monitor for debugging
  hc12.begin(9600);    // HC-12 baud rate
  Serial.println("Blizzard Initialized.");
}

void loop() {
  // Check if data is available from Cube
  if (hc12.available()) {
    String incomingMessage = hc12.readStringUntil('\n');
    Serial.print("Received from Cube: ");
    Serial.println(incomingMessage);

    // Check if the message is an ACK request
    if (incomingMessage == "REQUEST_ACK") {
      sendAck();
    }
  }

  // Example: Blizzard requesting ACK from Cube
  bool ackReceived = requestAck();
  if (ackReceived) {
    Serial.println("ACK received from Cube");
  } else {
    Serial.println("Failed to receive ACK after retries");
  }

  delay(5000); // Delay before the next request

void handleCheatCodeInput(char key) // toggle cheatcode mode on and off
{                                   // when key is pressed, LB on child blizzard, switchA on the parent, enters cheatcode mode,
  if (key == 'B')
  {
    // User confirms entered cheat code
    enteredCode[cursorPosition] = '\0'; // Null terminate entered code
    if (accessGranted)
    {
      checkCheatCode(enteredCode); // Check entered cheat code if access is granted
    }
    else
    {
      // Access denied, display error message on LCD or take appropriate action
    }
    exitCheatCodeMode(); // Exit cheat code mode
  }
  else
  {
    // Append pressed key to entered cheat code
    enteredCode[cursorPosition++] = key;
    // Display entered code on LCD
    // Implement LCD display functionality
  }
}
}
// GSM code








//LEVEL C code this is security code, Profile making code, communication encryption etc etctra
void password_interface()
{ // accesspassword,
  /*password interface,
  #001 password access before access granted, the switchA will be used to toggle password interface, after access granted, something else, like arming and shit
  on startup, the blizzard will send a request to connect signal looking for the drone, then it will prompt the user to enter password, the drone will then give access to the blizzard based on rank,
  three passwords admin, parent and child,
  the drone allows full or partial access to the device based on level,
  show error when password in wrong, if wrong 4 times, lock, and require PuK PIN, LOCK BLIZZARD...
  */
  request_to_connect();
}
void request_to_connect()
{
}
//**************************************cheatcode management functions********************************************
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
// Function to setup cheat code system
void setupCheatCodes()
{
    initializeHC12();
    Serial.println("Cheat code system initialized");
}

// Function to read and process cheat codes
void readCheatCode()
{
    if hc12Available ()
    {
        cheatCode = readHC12();
        processCheatCode(cheatCode);
    }
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

// Function to process cheat code
void processCheatCode(String cheatCode)
{
    switch
    cheatCode:
    case "osiris":
        executeOsiris();
case "anubis":
    executeAnubis();
case "testmode";
    toggleTestMode();
// Add more cheat codes as needed
}
void handleCheatCodeInput(char key) // make it handle many cheats concurrently
{                                   // this is one cheatcode per time,
static char enteredCode[20];    // Array to store entered cheat code
static int cursorPosition = 0;  // Cursor position in entered code

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

/*Ultrasonic sensor
Design hand to hold sensor,
Record height above ground.
Altitude lock using height above ground.
*/


