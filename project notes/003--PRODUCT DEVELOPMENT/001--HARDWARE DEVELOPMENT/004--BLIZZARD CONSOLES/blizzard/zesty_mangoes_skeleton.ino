// this is the main skeleton file for blizzard,
/*
Blizzard Console Documentation
1. Introduction
Blizzard is an advanced controller designed for the Velma drone system, with applications extending to various delivery drone platforms. Its primary function is to provide a versatile, secure, and user-friendly interface for drone control and management.

Key Features and Capabilities

Multi-Drone Compatibility: A single Blizzard console can interface with multiple drones, using unique access codes for each connection.
Bi-Directional Control: Drones can be operated from different locations, allowing for flexible deployment and retrieval scenarios.
Advanced Navigation: Integrates GPS and magnetometer data for precise location tracking and orientation.
Customizable Control Schemes: Implements a cheat code system for accessing advanced functions and custom control configurations.
Power Management: Actively monitors battery levels of both the console and connected drone, providing timely warnings and status updates.
Augmented Reality Control: Compatible with VR goggles for immersive, head-movement-based drone control.

Practical Application Scenario
In a medical context, Blizzard enables efficient drone-based transport between healthcare facilities. For instance, Aga Khan Hospital could dispatch a drone to Kenyatta National Hospital (KNH). Operators at both locations, 
equipped with Blizzard consoles, can control the drone for takeoff, flight, and landing procedures. This system ensures seamless handover of control and operation between different points in the delivery network.

Overview
Blizzard is the primary controller for the Velma drone, designed for use in delivery drones and other applications. It allows one Blizzard console to connect to different drones, and one drone can connect to multiple Blizzard consoles (one at a time).
Key Features

Interchangeable drone control
Password-protected access
GPS and magnetometer integration
Cheat code system for advanced functions
Battery management and low battery warnings
Integration with goggles for head movement control

Use Case Example
Aga Khan can send a drone to KNH, and if both have a Blizzard console, they can give commands to the drone. Users can land the drone using goggles connected to the onboard VTX. Future versions will include machine vision for autonomous landing on ground tags.
2. Hardware Components

Arduino Nano
9V battery
Radio transmitter
Matrix keypad: For manual input and cheat code entry. initialization, mapping and testing,(testing each key) 
Digital volume control: Multi-function input for menu navigation and parameter adjustment.
I2C LCD screen; Displays menus, status information, and real-time drone data.
HC-12 transceiver
GPS module
Magnetometer
Goggles with gyroscope
GSM module (for receiving instructions via SIM technology)

Hardware Architecture
The Blizzard console integrates various components to create a comprehensive control system:

Central Processing: Arduino Nano serves as the brain, managing all peripheral interactions and core logic.
Power Supply: A 9V battery provides portable power, with voltage regulation for component compatibility.
User Interface:


3. Software Components
Software Architecture
The Blizzard's software is designed with modularity and extensibility in mind:
Core Systems and functionality

Arduino code for Blizzard operation: Custom Arduino-based firmware optimized for real-time drone control and monitoring.
Power Management: Continuously monitors voltage levels, implements smart power-saving modes, and provides predictive battery life estimates.
Key matrix code for keypad; Input Processing: Sophisticated debouncing and input validation for the keypad and volume control, ensuring accurate user input interpretation.
Menu System design: Hierarchical, context-sensitive menu structure allowing quick access to frequently used functions while providing depth for advanced settings.
Wireless Communication Protocol: Custom protocol built on HC-12 capabilities, featuring encryption, error-checking, and automatic retransmission for reliable drone communication.
GPS and magnetometer integration
I2C LCD screen interface
Digital volume control interface
HC-12 communication code
Cheat code system
Drone execution code
Error Handling and Debugging

Advanced Features

Cheat Code Engine: Interprets complex input sequences to unlock or modify drone behaviors, allowing for on-the-fly reconfiguration of control schemes.
Telemetry Processing: Real-time analysis of incoming drone data, with alerting systems for anomalous conditions.
Autonomous Operation Modules: Implements geofencing, return-to-home functionality, and predefined flight patterns.
GSM capabilities for receiving instructions
Integration with goggles for head movement control; Augmented Reality Integration: Translates head movements from VR goggles into drone control commands, with customizable sensitivity and mapping.
Dynamic prompt system for drone commands
Data streaming and monitoring

4. User Interface Design
The Blizzard console prioritizes intuitive operation and information clarity:
Input Methodology

Matrix Keypad: Alphanumeric input for direct command entry, menu navigation, and cheat code activation. Customizable key mapping for user preference.

Digital Volume Control:
Rotation: Scroll through menus, adjust numerical values.
Click: Confirm selections, toggle options.
Long Press: Context-sensitive actions, such as returning to the previous menu.


VR Goggles: When active, head tilts and rotations are mapped to drone movements, with customizable dead zones and sensitivity.

Visual Interface

I2C LCD screen: High-contrast display with dynamic backlighting for optimal visibility in various conditions.

Menu Structure:
Home Screen: Displays critical information (battery levels, GPS coordinates, altitude, active drone UID).
    Screensaver displaying battery level, coordinates, altitude, and drone UID
Quick Access Menu: Customizable shortcuts to frequently used functions.
Settings Hierarchy: Nested menus for detailed configuration of both Blizzard and drone parameters.


Status Indicators: LED array for at-a-glance system status (power, connection strength, alarm conditions).


Cheat Code System;
Locked capabilities accessible via memorized codes
Input via keypad or digital volume, Uses full keypad for complex code entry, with on-screen feedback.
Cheat code mode toggled on/off; Dedicated button combo to enter cheat code mode.
Function Mapping: Successful codes can modify control layouts, adjust flight characteristics, or unlock special features.


5. Communication Protocols
Blizzard employs a multi-layered communication strategy:
Drone Communication
Communication:

HC-12 Transceiver: Long-range, low-power wireless communication with the drone.
Radio Transmitter: Secondary control option for direct flight control.
GSM Module: Enables reception of remote instructions and updates.for receiving external instructions
Potential for inter-hospital communication

Primary: HC-12 transceiver utilizing a custom protocol with the following features:

Frequency Hopping: For resistance against interference.
Adaptive Data Rates: Balances range and data throughput based on signal strength.
Encryption: AES-256 for secure command and telemetry transmission.


Secondary: Traditional radio control as a failsafe, with automatic switchover in case of HC-12 failure.

External Systems Integration

GSM Module: Enables reception of text-based instructions, software updates, and mission parameters from authorized ground stations.
Inter-Console Communication: Secure handshake protocol for transferring drone control between Blizzard units, including authentication and mission data transfer.
    acknowledgement system


6. Power Management
Power Management System
Intelligent power handling ensures reliable operation:

Voltage Monitoring: Continuous sampling of battery voltage with moving average filtering for accurate level estimation.
Power Profiles:

Normal: Full functionality for routine operations.
Eco: Reduced screen brightness, lowered transmission power, and increased sleep intervals for extended operation.
Critical: Essential functions only, activated automatically at low battery thresholds.


Charging System: Integrated charging circuit with overcharge protection and charge status indication.
Drone Power Estimation: Algorithms to predict remaining flight time based on current draw and historical usage patterns.

Battery voltage monitoring
Low battery warnings
Charging functionality

7. Security
Security Framework
Multi-layered approach to ensure authorized access and operation:

Access Control:
Changing passwords for enhanced security
Unique, rotating passwords for each drone-console pairing.
Two-factor authentication option using a combination of password and physical key.
Password-protected drone access

Encryption:

All wireless communications (HC-12 and GSM) use end-to-end encryption.
Local data storage is encrypted to prevent unauthorized access if the console is lost or stolen.


Proximity Awareness:
Proximity-based connection prompts
NFC-based handshaking for initial drone-console pairing.
Continuous proximity monitoring with automatic disconnect if the console moves out of a predefined range from the drone.


Audit Logging: Detailed logs of all control inputs, system changes, and drone operations for accountability and troubleshooting.



8. Future Development

Machine vision for autonomous landing (V4)
Enhanced inter-device communication
Adaptive power management strategies
Dynamic mission allocation
Comprehensive test functions and diagnostics

9. Setup and Configuration
Hardware Setup

Component connections and wiring diagram

Software Setup

Arduino IDE setup
Required libraries
Initial configuration process

10. Troubleshooting and Support

Common issues and solutions
Contact information for support

11. Appendices

Pinout diagrams
Glossary of terms
References and further reading




Navigation and Orientation:

GPS Module: Provides accurate location data for both console and drone.
Magnetometer: Offers precise orientation information.


Augmented Control:

VR Goggles: Equipped with a gyroscope for head-movement drone control.





//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
primary objectives
interfacing code, code to connect, calibrate and test presence of modules gps, magnetometer, accelerometer, timers,
handle keypad and digital volume input, LCD output, both cursors,
//BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
//secondary objectives,
calibrate blizzard modules, read battery
the blizzard can connect to the laptop, to relay drone data wirelessly to the laptop, ground station,




//menu systems https://www.instructables.com/Arduino-Settings-Menu/
//PROFILE ACCESS
A tester can run the tests codes remotely via the blizzard, #202039
duke of retset;
Pre-Flight Checks: Conduct comprehensive pre-flight checks and diagnostics.
an admin has all control, duke of nimda;
*/

/*what it should do*/

/*
it will have a ftdi adapter for usb purposes, push code that way, or thru direct connection with arduino,


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Parent console section,
THROTTLE,YAW, ROLL, PITCH, aux A, aux B, pot X, pot Y, aux C, aux D, sidepot A, sidepot B

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Child_console section
deadman switches
check if it is parent console or child console, run appropriately,
press big_x to turn on, connect to parent console for flight authorization, way to change this to admin via hidden settings,
use wheel to pitch and roll drone?
LB button autopilot, enter cheat codes, it cruises the drone, while the cheat is being entered,
LT trigger
RB button
RT trigger throttle three positions, descent no sudden descent, like 1m/s, this is changeable, hover, ascent,
X  button
Y  button
A  button
B  button
BACK button
START button arm disarm, has an LED, fixture
BIG_X button
B1 aux debounce button
B2  same
B3  same
B4  same
JOY A joysticks #gears yaw
JOY B joysticks pitch roll
D panel button
E same
F same
G same
#define all that shit
adaptive feedback, intensity of vibrations,
the vibratory motors are controlled via motor drivers, l293d to be specific,
    when e.g. drone is too low to the ground, the vibrators are turned on, no pun intended
also data received by accelerometer too on crash, or if the drone bounces on the ground,
can beep and vibrate when changing gears, it needs to be executed well

Parent_console section

the parent can input cheatcodes that the child will mimick, meaning the parent can chose to change child_console button layout,
both can input cheatcodes, determined by ones status, admin, trainer, learner,
*/
/*the drone is to have piloting capabilities via blizzard, the blizzard can choose to transmit data via the radio,
or via onboard transcievers, the blizzard can search and rescue the drone, at day or night time, by providing gps to blizzard
and algorithms,*/

/*
change this such that, if i have one cheat code x, the switches will have functions x1a to x4a,
if i have cheat code d, the switches will have function d1a, im just giving examples for context,
if i write the cheat code  i can access its capabilities and map them to the switches,  i can toggle a switch,  click,
map it to a cheatcode, map a functionality to the digital volume,
*/
void setup()
{
    /*
        on startup, show Blizzard on screen, startup beeps and LEDS, all do one light,
        interfacing code, code to connect, run VEC, read battery capacity,
        calibrate and test presence of modules gps, magnetometer, accelerometer, timers,
        initialize keypad, initialize screen, handle keypad and digital volume input, LCD output, both cursors,
        look for gps fix, from gps time data start onboard clock, this code will also run on the drone
        //the blizzard will calibrate accelerometer and gyro, user should sit comfortably, that will be upright, __more on this @#12344zeus_skeleton
        I don't see why the blizzard shouldn't set home location like the drone  _more on setting home location  @#16877gps_data_skeleton
            can toggle, or automatic, fix to drone, via hc12 and radio too, checkAccess code, password interface
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Drone access


    */
}
void loop(){


/*    via a switch, the blizzard changes from radio to transcievers, or automatic, the blizzard chooses, 

     
  /*On startup, show the blizzard name on screen,
  alone without being connected to the drone,
  the blizzard can operate on its own to find greedy location both lat and long and time, display as screensaver
  screensavers #34566 the drone can flush data to the blizzard, this is then cast to the screen, like drone lat/long, abs altitude, velocity, batt capacity etc, if docked
            if not docked, show the blizzards data, same same, change this in settings, 


  // connected to the drone,
  // if docked, its time to fly, basic no cheatcode, it will fly with its arm and disarm switch as switch A,
  // If button is pressed, enter cheatcodemode,
  on_startup(); // run startup sequence,access granted,
  //assuming access granted, data from the calibration, including home location is fetched, and will be displayed as screensaver by default, 



//while RT is throttle, LT is brake only for child_console
*/
    

}

/*



Universally Applicable (All the time)
Environmental Monitoring: Continuously monitor environmental conditions (temperature, humidity, air quality, light).
Communication Relay: Act as a communication relay between Cube and the ground station or other drones.
Data Analysis: Perform on-board data analysis and preprocessing.
Power Management: Manage power usage to ensure long operational times.
Security Monitoring: Monitor for security breaches and report to the ground station.



Docked (Cube connected to blizzard and ready to fly)
Support Operations: Assist Cube in tasks such as navigation, data collection, and communication.
Real-Time Data Analysis: Analyze data in real-time to support Cube’s mission.
Emergency Assistance: Provide emergency assistance to Cube during flight operations.
Coordinated Mission Planning: Work with Cube to plan and adjust missions dynamically.
Redundancy: Serve as a backup control unit in case of Cube's system failure.




Undocked (Cube not connected)
Charging: Charge Blizzard’s own battery and potentially charge Cube when undocked.
Data Sync: Sync collected data with Cube and ground station.
Maintenance Mode: Enter a low-power state for maintenance or software updates.
Pre-Deployment Configuration: Configure and program for the next deployment.
System Backup: Backup important data and system configurations.



Additional Notes
Inter-Device Communication: Both Cube and Blizzard should be able to communicate with each other to coordinate tasks and share data.
Adaptive Power Management: Implement adaptive power management strategies to extend operational time.
Dynamic Mission Allocation: Capable of dynamically allocating and adjusting missions based on real-time data and conditions.
Test Functions: Integrate test functions to check the status of various components (e.g., sensors, ESCs) and handle anomalies by running diagnostics and substituting erroneous data.
*/
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
    cheatcode mode, toggled on and off by a button, entercheatcodemode, exitcheatcodemode........
        the blizzard handles input, the blizzard checks for entered code, transmits that code to the cube
      if LB cheat button if pressed, or switchA #never changes
        enterCheatCodeMode();
for
    child_console press cheat button while still holding throttle, and the drone will maintain that movement until its pressed again
                                                                   checkCheatCode();
handleCheatCodeInput();
if LB
    pressed again;
exitCheatCodeMode();




## Blizzard schematics and logic, 
whaT connects to what, and how the two bring about nice shit
And the schematic diagrams.
design circuitry via software, print, so much easier than paper,





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

    setting mode toggled via a button on the keypad
Device settings
Brightness and contrast adjustments
Low battery alarm settings
        device settings, if this the blizzard changes its own settings, brighntess and screen contrast, 
            change low power variable, give low power alarm when specified capacity is reached


Drone settings
PID settings (Proportional, Integral, Derivative)//cleopatra and drone PID
Sensitivity and full-scale settings
Dynamic control and failsafes
        drone settings, if this, it connects to cube via hc12, and can change values in the stack and the EEPROM, #87633



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