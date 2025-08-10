This document outlines the software architecture of the Ursa Minor drone system, focusing on both the drone (Velma) and the ground station (Blizzard). It provides an overview of the software modules, control logic, communication protocols, data management, and interaction between the systems.

Overview
The software architecture for Ursa Minor ensures smooth operation of the drone system, real-time data exchange, and seamless interaction between Velma and Blizzard. The architecture is modular, separating key functionalities into independent libraries and processes to improve maintainability, scalability, and debugging.

Velma: Responsible for flight operations, navigation, and sensor management.
Blizzard: Handles user input, telemetry display, command transmission, and mission management.

Core Software Modules
1. Flight Controller Module (Velma)
    Functions:
Controls the drone’s flight dynamics (pitch, roll, yaw, and throttle).
Executes PID loops to stabilize flight.
Manages flight modes (e.g., manual, autonomous, Return-to-Home).
    Libraries Used:
PWM Control Library for motor control.
Sensor libraries for accelerometer, gyroscope, and barometer data.
    Key Features:
PID Control: Ensures precise flight stabilization by adjusting motor speeds.
Fail-Safe Logic: Activates automatic landing or Return-to-Home in case of signal loss.

2. Navigation Module (Velma)
    Functions:
Uses GPS data to navigate between waypoints and follow predefined flight paths.
Supports dynamic RTH (Return-to-Home) based on either the original launch location or Blizzard’s current GPS location.
    Key Features:
Waypoint Navigation: Uses a sequence of latitude/longitude coordinates to guide the drone.
Real-Time GPS Tracking: Continuously updates the drone’s position to ensure path accuracy.
Obstacle Avoidance (Future Expansion): Supports the addition of lidar or ultrasonic sensors to avoid collisions.

3. Motor Control Module (Velma)
    Functions:
Receives PWM signals from the flight controller to vary motor speeds.
Adjusts speed dynamically to achieve movement along any axis.
    Control Logic:
Pitch and Roll: Managed by varying motor speeds diagonally across the frame.
Yaw Rotation: Achieved by increasing/decreasing opposite motor pairs.
Throttle (Altitude Control): Controlled by increasing or decreasing all motor speeds uniformly.

4. Sensor Management Module (Velma)
    Functions:
Collects data from onboard sensors, such as the accelerometer, gyroscope, GPS, and barometer.
Provides real-time data to the flight controller for flight stabilization and navigation.
Monitors battery temperature using a thermocouple.

5. Communication Module (Velma and Blizzard)
    Functions:
Handles data exchange between the drone and the ground station using HC-12 transceivers.
Transmits telemetry data (altitude, speed, battery status) from Velma to Blizzard.
Receives control commands and flight mode changes from Blizzard to Velma.
    Communication Protocol:
Uses a packet-based system for reliable data exchange, with redundancy to minimize signal loss.
Commands and telemetry packets contain checksums to detect transmission errors.

Blizzard (Ground Station) Software Architecture
1. User Interface Module
    Functions:
Processes input from the keypad and digital volume knob.
Displays telemetry data on the LCD screen (e.g., GPS coordinates, altitude, battery level).
    Key Features:
Mode Selection: Allows the pilot to switch between different flight modes (manual, RTH, autonomous).
Feedback: Provides visual (LED), auditory (buzzer), and haptic (vibratory motors) feedback to the user.

2. Mission Management Module
    Functions:
Supports predefined mission execution using waypoints.
Manages mission-critical alerts, such as low battery or GPS signal loss.

3. Telemetry Management Module
Functions:
Logs flight data to the SD card for post-flight analysis.
Provides real-time feedback to the pilot on the LCD screen.

4. Power Management Module
    Functions:
Monitors the 9V battery level to prevent unexpected shutdown.
Manages sleep mode to conserve power when the system is idle.
Data Flow Between Velma and Blizzard
    Startup Sequence:
Both systems initialize sensors, communication modules, and user interface components.
Blizzard collects its home location, while Velma initializes its navigation sensors.
    Command Transmission:
The pilot sends a command via the keypad or volume knob on Blizzard.
The command is packaged and transmitted to Velma using the HC-12 transceiver.
    Telemetry Feedback:
Velma sends telemetry data (e.g., altitude, battery status) at regular intervals to Blizzard.
Blizzard displays the data on the LCD and triggers alerts if necessary (e.g., low battery).

Software Design Challenges and Solutions
1. Handling Latency and Delays:
Challenge: Communication delays between Velma and Blizzard can affect real-time control.
Solution: Implement redundant communication channels and timeout-based fail-safes to manage signal loss.

2. Sensor Calibration and Noise Filtering:
Challenge: Sensor data may contain noise or inaccuracies.
Solution: Use Kalman filters or sensor fusion algorithms to enhance data reliability.

3. Modular Code Design:
Challenge: A large codebase can become difficult to maintain.
Solution: Divide the system into independent libraries (e.g., GPS library, motor control library) to improve maintainability.

Libraries and Dependencies
1. External Libraries:
Keypad.h – For handling keypad inputs.
LiquidCrystal_I2C.h – For managing the LCD display.
Wire.h – For I2C communication between components.
Servo.h – For controlling motor angles and other actuators.

2. Custom Libraries:#################################################################to be edited further
GPS_Library.cpp – Handles GPS data parsing and navigation logic.
Motor_Control.cpp – Manages motor speed control using PWM signals.
Telemetry_Packet.cpp – Structures and sends telemetry data packets.

Future Software Expansion
    Advanced Autopilot:
Implement machine learning models for flight path optimization and autonomous obstacle avoidance.
    Cloud Integration:
Enable the system to upload flight data to the cloud for remote access and real-time monitoring.
    Extended Payload Support:
Add software modules for controlling payloads (e.g., paintball gun system, surveying equipment).