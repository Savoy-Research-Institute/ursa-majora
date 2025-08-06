<!-- notes to the technician, on the  workings of the device -->
this is a catch me up file for the blizzard console, 
This is a manual for Blizzard and what it is supposed to achieve for velma
Blizzard is the primary controller for the velma drone,the blizzard is made such that one blizzard can connect to different drones, 
as long as the access code is known by the user, a drone can connect to many blizzard consoles, one at a time, 
such that Aga Khan can send a drone to KNH and if both have a blizzard console to give commands to the drone, 
as an mvp, before autonomy, the users will land the drone, they will have goggles that will connect to the onboard vtx, 
V4 will have machine vision thus will land on tags on the ground, still, they can authorize landing, and taking off, with blind delivery, it can even land on a field, and takes off from where it is turned on from, which doesnt have to occupy large space,  
the passwords always changing if need be,

Parts list;
Arduino nano, 9v battery, radio tx, a matrix keypad, a digital volume, an I2C screen, a transciever, a gps module, a magnetometer, 
the goggles will have a gyroscope,

The blizzard needs code run by an arduino nano, battery management code to read battery voltage,  and give low battery warnings, 
A voltage divider will cut this voltage to 5v, read via an analog pin
the nano is connected to a keypad, write key matrix code, design the menu,

gps and mag, these devices will give the blizzard magnetic orientation and PVT data, used for location, and drone rescue,

an I2C LCD screen, blizzard menu system, (use buttons and digital volume to go thru them)
design the menu system, screensaver, show batt level, lat and long, altitude, and drone UID on the screen at all times during the screen saver,

a digital volume to scroll the menus 
and to change brightness and contrast, and variables, 
back it turn back  and foward and click, tuns back a menu

gsm capabilities, thus every blizzard can receive instructions from other sources via SIM technology, 
a hospital will send an incoming drone message to the receiving blizzard, way before it arrives, 

a transciever, 

drone execution code, flush/fetch data straight from drone to the LCD screen battery capacity, current altitude and coordinates, magnetometer orientation etc, prompt the data to change flight data like PID and cruise altitude, and gps hold coordinates, etc
prompt gives the drone a command to execute, e.g. cruise, cheatcodes are prompts, but can prompt more than just cheatcodes, can prompt height lock at 3m, etc, this is a dynamic prompt, stream the blizzard can prompt the drone to stream data, like the coordinates, the blizzard will print the values on the screen, until stopped, the blizzard can monitor the values, and give an alarm when its out of the set range, 


HC12, the blizzard uses  HC12 to communicate with the drone, write code for this 


cheat code scripts, the blizzard console will have more capabilities than the onboard switches can handle, 
these capabilities will be locked behind cheatcodes that the user will memorize, the blizzard will hold these codes while the execution script in on the drone data logger, input is either via the keypad, which at all times displays the cheats, starting with A, if i press K, it goes to the start of K, then E, then etc, the menu goes with the, new cheats, or old ones from a list, a new one, it displays the entered, keys only, with a cursor, or the digital volume, clicking via it too..


battery management, the blizzard notifies the user when the drone is low on power

charging, 

blizzard proximity connection, the drone should prompt a connction to the blizzard once both are on and near to each other,

the drone has a static or dynamic password, that the blizzard will have to match, to gain access to a drone, 

## Blizzard schematics and logic, 
whaT connects to what, and how the two bring about nice shit
And the schematic diagrams.
design circuitry via software, print, so much easier than paper,




This is a manual for Blizzard and what it is supposed to achieve for velma
Blizzard is the primary controller for the velma drone, this technology will then be used in delivery drones, 
The blizzard user will need a password, to connect to the drone, always, the passwords always changing if need be, 
The blizzard needs code run by an arduino nano connected to a keypad, an I2C LCD screen, a digital volume to scroll the menus 
and to change brightness and contrast, a transciever, drone execution code, cheat code scripts, battery management, charging, 

blizzard connects to goggles to control drone using head movements, 
MVP for Blizzard
Blizzard connects a user to a drone, such that AgaKhan can send a drone to kikuyu, and someone in kikuyu who has a blizzard can send it back, the blizzard will hold these coordinates and user IDs, so that the user wont have to key them in, 
Table of Contents: Blizzard Console Documentation
Introduction
Overview of the Blizzard module
Purpose and goals
Target audience

Setup and Configuration
Hardware requirements
Software requirements
Installation instructions

Basic Functionality
Powering on and initialization
Navigation interface (Keypad and Volume Control)
LCD screen management
LED status indicators
Menu system
Structure and navigation
Device settings
Brightness and contrast adjustments
Low battery alarm settings
Drone settings
PID settings (Proportional, Integral, Derivative)//cleopatra and drone PID
Sensitivity and full-scale settings
Dynamic control and failsafes

Advanced Features
Cheat code system
Activation and input handling
Integration with Radio Transmitter (FSi6 X)
Communication via HC12 Transceiver
Access code security mechanism

Integration and Expansion
Connecting additional modules or devices
Extending functionality through software updates
Future expansion plans

Troubleshooting and Support
Common issues and solutions
Contact information for support
Appendices

Pinout diagrams
Glossary of terms
References and further reading
This table of contents outlines a structured approach to documenting the Blizzard Console, covering setup and configuration, basic and advanced functionalities, integration possibilities, troubleshooting, and support. Each section is designed to provide comprehensive information and guidance for users and developers interested in utilizing the Blizzard Console for drone control and other applications. Adjust and expand each section with specific details and updates as needed to suit the ongoing development and requirements of your project. If you have any further questions or need additional details on any section, feel free to ask!



For the Blizzard Console project, dividing the tasks into two parts—basic functions and advanced algorithms—can help in structuring development effectively. Here’s how you can organize it:

Part A: Foundational Functions and Basic Connectivity
Hardware Setup and Initialization

Keypad Initialization: Setting up the keypad and mapping the keys.
LCD Display Initialization: Configuring the I2C LCD and setting up initial display settings.
Digital Volume Control: Setting up the digital volume encoder and reading its values.
LED Status Indicators: Controlling LEDs for status indication.
HC12 Transceiver Communication: Setting up and initializing the HC12 module for wireless communication.
Radio Transmitter (FSi6 X): Initializing and interfacing with the FSi6 X radio transmitter.
Basic Input/Output Functions

Reading Keypad Input: Handling keypad presses and converting them to usable input.
Displaying Messages: Sending data to the LCD screen for display.
Volume Control Handling: Reading and interpreting the digital volume control input.
Basic Communication

Sending Data: Writing functions to send data via HC12.
Receiving Data: Reading and parsing data received via HC12.
Serial Communication: Basic functions for debugging and logging via serial communication.
Menu Navigation

Basic Menu Structure: Implementing the foundational menu structure and navigation logic using the keypad and volume control.
Displaying Menu Options: Functions to display different menu options on the LCD.
Error Handling and Debugging

Basic Error Checks: Implementing error checks for hardware communication (e.g., ensuring HC12 is connected).
Debugging Output: Functions to output debug information to the serial console.
Part B: Complex Algorithms and Advanced Functionality
Advanced Menu System

Nested Menus: Implementing sub-menus for device settings and drone settings.
Dynamic Menu Updates: Functions to update the menu options dynamically based on user input.
Settings Adjustment: Functions to adjust settings (e.g., brightness, contrast, PID values) using the digital volume control.
Cheat Code System

Cheat Code Input: Implementing a system for entering cheat codes via the keypad.
Cheat Code Validation: Functions to validate and execute cheat codes.
HC12 Transmission: Sending validated cheat codes via the HC12 transceiver.
Security Mechanisms

Access Code Verification: Implementing a security mechanism to verify access codes before allowing certain operations.
Encrypted Communication: Optional encryption for data transmitted via HC12 for enhanced security.
Advanced Control Algorithms

PID Control Tuning: Functions to adjust and fine-tune PID control parameters for drone stabilization.
Sensor Integration: Advanced handling and fusion of sensor data for more accurate control.
Failsafe Mechanisms: Implementing failsafe operations (e.g., return-to-home, low battery alarms).
User Interface Enhancements

Graphical Display: Enhancing the LCD output with graphical elements (e.g., progress bars, icons).
Interactive Feedback: Providing real-time feedback to user actions through the interface.
Extended Functionality

Firmware Updates: Implementing a mechanism for updating the console firmware over-the-air (OTA) or via USB.
Logging and Data Storage: Functions to log important data and store settings in non-volatile memory.
Strategy for Development:
Begin with Part A: Ensure that all foundational functions and basic connectivity are in place and thoroughly tested. This includes setting up hardware components, basic I/O operations, and establishing reliable communication channels.

Progress to Part B: Once the basic functions are stable, move on to implementing advanced algorithms and features. Test each new feature incrementally to ensure it integrates well with the existing system.

Iterative Testing and Refinement: Continuously test and refine both basic and advanced features to ensure reliability and performance.

By structuring the development of the Blizzard Console in this manner, you can systematically build up from a solid foundation of basic functionality to a sophisticated system with advanced features and algorithms. This approach ensures that each component is well-integrated and functional, paving the way for a robust and feature-rich console.







Common Functions:

requestAck() sends an ACK request and waits for a response. It retries if an ACK is not received within the timeout.
sendAck() sends an ACK response.
Cube Code:

Listens for incoming messages from Blizzard.
If an ACK request is received, it responds with an ACK.
Periodically sends an ACK request to Blizzard and waits for a response.
Blizzard Code:

Listens for incoming messages from Cube.
If an ACK request is received, it responds with an ACK.
Periodically sends an ACK request to Cube and waits for a response.
Testing and Debugging
Serial Monitor:

Use the Serial Monitor to observe the communication between Cube and Blizzard.
Ensure that ACK requests and responses are correctly sent and received.
Timeout and Retries:

Adjust the ackTimeout and maxRetries values based on the reliability of the HC-12 communication in your environment.
By implementing this system, both Cube and Blizzard can reliably communicate and confirm the receipt of important messages, enhancing the overall robustness of the drone's communication protocol.Implementing a bidirectional acknowledgment system for the Cube and Blizzard using HC-12 transceivers is a great way to ensure reliable communication between the drone's components. Here’s an implementation plan for both sides to handle acknowledgment requests.

Implementation Plan
Define Protocol:

Both Cube and Blizzard can send messages requesting acknowledgment.
The receiver will treat acknowledgment requests as priority tasks and respond with an ACK.
Implement retries and timeouts for acknowledgment requests.
Error Handling:

Implement retries if an ACK is not received within a specified timeout.
Ensure that both Cube and Blizzard prioritize handling acknowledgment requests.