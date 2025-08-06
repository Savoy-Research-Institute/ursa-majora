This document outlines the hardware architecture of the Ursa Minor drone system, focusing on the two main components: the drone (Velma) and the ground station (Blizzard). It provides a detailed breakdown of each system's hardware components, power management, sensors, and connectivity.

Overview
The hardware architecture of the Ursa Minor project includes:
Velma (the drone): Aerial component responsible for flight, data collection, and navigation.
Blizzard (ground station): A portable control hub providing real-time control, feedback, and data logging capabilities.
Both components have interlinked systems to ensure safe flight, reliable communication, and mission success.

Velma (Drone) Hardware Architecture
1. Main Control Unit:
Flight Controller:
Controls motor speeds using PWM signals to adjust pitch, roll, yaw, and throttle.
Manages flight modes, including manual, autonomous, and self-leveling modes.
Interfaces with various sensors (accelerometer, gyroscope, GPS) to maintain stability and control.
Handles fail-safe functions like Return-to-Home (RTH) in case of signal loss.

2. Sensors:
Accelerometer and Gyroscope:
Provides data on the drone’s movement and orientation.
Used in self-leveling modes to detect angular changes and stabilize the drone.
Ultrasonic Sensor:
Measures altitude at low heights for precision landings.
GPS Module:
Tracks the drone’s location for navigation and telemetry.
Enables Return-to-Home functionality and waypoint-based missions.
Magnetometer:
Provides heading information, ensuring the drone maintains the correct orientation during flight.
Thermocouple:
Monitors the temperature of the LiPo battery cells to prevent overheating.

3. Propulsion System:
Motors (Brushless DC Motors):
Four motors for quadcopter design, each controlled by the flight controller through PWM signals.
Adjusts speed to achieve flight maneuvers (e.g., hovering, rolling, yawing).
Electronic Speed Controllers (ESCs):
Receives signals from the flight controller to manage motor speeds efficiently.

4. Communication Module:
HC-12 Transceiver:
Sends telemetry data to the ground station and receives control commands from Blizzard.
Operates on 433 MHz frequency band for long-range communication.
Video Transmitter (Optional):
Streams real-time video feed to Blizzard or an external monitor.

5. Power Management:
LiPo Battery:
Primary power source for the drone, providing high energy density.
Includes a voltage regulator to ensure stable power delivery to critical components.
Low Battery Alarm:
Alerts the pilot when the battery reaches a critical level, ensuring timely landings.
Voltage Divider:
Measures battery levels and sends data to the flight controller for monitoring.

6. Additional Components:
LEDs and Buzzer:
Provides status indicators and alerts (e.g., flight mode changes, signal loss).
Disarm Pin:
Safely disables motors when on the ground to prevent accidental starts.
Twelve-Pin Connector:
Central hub for connecting sensors and power lines within the drone.


Blizzard (Ground Station) Hardware Architecture
1. Main Control Unit:
Arduino Nano:
Handles input from the user interface (keypad, volume knob) and communicates with the drone via HC-12.
Collects and displays telemetry data on the LCD screen.

2. Sensors and Modules:
GPS Module:
Tracks Blizzard’s location for dynamic Return-to-Home functionality.
Magnetometer:
Assists with heading calibration, ensuring correct orientation during flight.
Accelerometer and Gyroscope:
Provides data for Blizzard’s calibration and internal diagnostics.

3. Communication Module:
HC-12 Transceiver:
Sends control signals to the drone and receives telemetry data from Velma.
Supports redundant communication channels to mitigate signal loss.

4. User Interface:
Keypad and Digital Volume Knob:
Used for mode selection, input commands, and manual overrides.
I2C LCD Screen:
Displays telemetry data (GPS, altitude, speed) and system status in real-time.

5. Power Management:
9V Battery:
Powers the ground station, with a voltage regulator ensuring consistent performance.
Includes a sleep mode to save power during idle periods.

6. Feedback Components:
LED Indicators:
Provide visual feedback for communication status and flight modes.
Buzzer:
Alerts the pilot about signal loss, low battery, and other critical events.
Vibratory Motors:
Provide haptic feedback during critical events (e.g., signal loss, mode changes).

Interfacing Between Velma and Blizzard
Telemetry Transmission:
Drone sends data (altitude, GPS, battery status) to Blizzard in structured packets via the HC-12 transceivers.
Control Signal Reception:
The flight controller on the drone interprets commands sent from Blizzard to adjust motor speeds and perform maneuvers.
Dynamic Return-to-Home (RTH):
The drone can either return to the original take-off point or Blizzard’s current location based on GPS data.

Hardware Design Challenges and Solutions
1. Weight Management:
Challenge: Drone components add significant weight, affecting flight time and agility.
Solution: Use lightweight materials (e.g., carbon fiber frame) and optimize sensor placement.

2. Heat Dissipation:
Challenge: Components like ESCs and the flight controller generate heat during operation.
Solution: Ensure proper ventilation or include heat sinks on critical components.

3. Battery Overheating:
Challenge: LiPo batteries can overheat under high load, posing a safety risk.
Solution: Install a thermocouple to monitor battery temperature and trigger alerts if overheating is detected.

4. Signal Interference:
Challenge: Interference can disrupt communication between the drone and the ground station.
Solution: Implement dynamic frequency switching and use high-quality antennas to maintain signal strength.
Future Expansion and Modularity
Additional Sensors:
Add advanced sensors (e.g., lidar) to improve obstacle detection and navigation capabilities.
Payload Support:
Design modular connectors to attach payloads such as paintball guns or surveying equipment for specialized missions.
Software Integration:
Enhance compatibility with mission planning software to support automated flight paths.
