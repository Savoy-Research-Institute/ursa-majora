Blizzard Console – Project Documentation
📖 1. Introduction
Blizzard is the primary handheld controller for the Velma drone system, designed to provide secure, versatile, and intuitive control for mission-critical drone operations.

Key capabilities include:

Multi-console compatibility: One Blizzard can connect to multiple drones (one at a time), and each drone can connect to multiple Blizzard consoles via dynamic access codes.

Remote mission execution: Operators at different locations can seamlessly hand off drones between Blizzard consoles.

Pre-autonomy control: Enables human-guided take-off, landing, and mission execution before full autonomy is implemented, with future versions integrating vision-based autonomous landing on ground tags.

🎯 2. Project Goals
✅ Develop a robust handheld controller for mission-critical operations.
✅ Implement dynamic security protocols for drone-console authentication.
✅ Provide advanced interaction features, including:

Cheatcode entry system

PID tuning interface

Dynamic execution prompts

✅ Integrate real-time data monitoring for safe operations (battery, GPS, orientation, flight data).

🧑‍🔧 3. Core Design Philosophy
Security: Dynamic password authentication for Blizzard-drone connections.

Modularity: Hardware and software scalable across different drone models.

Usability: Intuitive keypad and digital encoder-based menu navigation.

Interoperability: Seamless sharing of drones between hospitals, delivery centers, and operators.

🏗️ 4. Hardware Components
Component	Purpose
Arduino Nano	Main microcontroller running Blizzard code
9V Battery + Voltage Divider	Power supply with voltage scaled safely for Nano readings
HC-12 Transceiver	Wireless communication with the drone
Matrix Keypad	Input interface for menu navigation and cheatcode entry
Digital Volume Encoder	Scroll/select options, adjust variables, brightness, and contrast
I2C LCD Screen	Display menu, flight data, battery status, and prompts
GPS Module	Provides Blizzard’s position for coordination and drone rescue
Magnetometer	Determines magnetic orientation
Radio TX (FSi6 X)	Planned standard RC communication integration
Goggles (with gyroscope)	For drone head-tracking in future versions

⚙️ 5. Software Functionalities
🔹 5.1 Basic Functions (Part A)
Hardware initialization: keypad, LCD, encoder, HC-12

Basic menu system for navigation

Real-time battery voltage monitoring with alarms

Display of drone data (latitude, longitude, altitude, UID)

Establishing communication protocols with the drone

🔹 5.2 Advanced Features (Part B)
Nested dynamic menu system

Cheatcode input, validation, and execution system

Dynamic password-based security and authentication

Drone execution prompts (e.g. PID tuning, altitude lock, mission commands)

Real-time data streaming with alarms for out-of-range values

Planned GSM integration for remote mission instructions

Bidirectional acknowledgment system for reliable data transfer

🔑 6. Security and Access
Dynamic password system securing each Blizzard-drone connection.

Proximity-based pairing prompts to ensure intentional access.

Encrypted communication (planned) to protect transmitted data.

✈️ 7. Example Use Case
Scenario: Aga Khan Hospital sends a drone to KNH.
Both hospitals have Blizzard consoles.
Operators authenticate using their Blizzard’s password, command landing, unload supplies, and send the drone back.

📝 8. Development Strategy
Phase 1 – Foundational Functions
Hardware interfacing and initialization

Basic menu and data display

HC-12 communication setup

Phase 2 – Advanced Algorithms & Security
Cheatcode system implementation

Dynamic password authentication

Drone execution prompts integration

Bidirectional acknowledgment protocol

Phase 3 – Expansion & Integration
GSM module integration

Autonomous landing with tag detection

Head-tracking control via goggles

OTA firmware updates

📂 9. Appendices (To be Developed)
Schematics and wiring diagrams

Pinout tables

Full function lists (inputs, outputs, flowcharts)

Troubleshooting guide

Glossary of terms

⚡ Technical Notes Summary
Passwords: Always required for connection; dynamically updateable.

Voltage divider: Scales 9V battery voltage to 5V for safe Nano reading via analog pin.

Keypad: Matrix input for cheatcodes and menu navigation.

GPS + Magnetometer: Provides Blizzard’s PVT data and orientation for drone rescue and navigation.

Menu System: Always displays battery level, GPS data, altitude, and drone UID in screensaver mode.

Digital Encoder: Scrolls menus, adjusts brightness/contrast/variables, and navigates back or forward via click.

GSM Module: Enables receiving drone mission instructions from remote systems (e.g. hospitals).

Transceiver: HC-12 manages drone communication; requires robust send/receive scripts.

Cheatcodes: Unlock advanced functions not mapped to standard controls; stored within Blizzard and validated for execution.

Battery Management: Provides user alarms for low battery conditions.

Proximity Pairing: Drone prompts connection to Blizzard upon mutual detection.

Execution Prompts: Blizzard sends commands to the drone to execute tasks (e.g. altitude lock, PID tuning), streams real-time data for user monitoring with threshold alarms.

Bidirectional ACK System: Ensures reliable data transfer by implementing acknowledgment requests, responses, retries, and timeouts.

🗂️ 10. Table of Contents Outline
Introduction

Setup and Configuration

Hardware and software requirements

Installation instructions

Basic Functionality

Powering on, initialization

Navigation interface (Keypad & Encoder)

LCD management

Menu structures

Advanced Features

Cheatcodes

HC-12 communication

Security mechanisms

Integration & Expansion

GSM, goggles, OTA updates

Troubleshooting & Support

Appendices

Schematics

Pinouts

Glossary

References

