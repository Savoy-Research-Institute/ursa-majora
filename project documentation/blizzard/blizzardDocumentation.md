Blizzard Console – Project Documentation
📖 1. Introduction
Blizzard is the primary handheld controller for the Velma drone system. It is designed to provide versatile, secure, and user-friendly control for drone operations, enabling:

Multi-console compatibility: One Blizzard can connect to multiple drones (one at a time) and vice versa, using dynamic access codes.

Remote mission execution: Operators can send or receive drones across different locations with seamless handoff between Blizzard consoles.

Pre-autonomous MVP control: Before full autonomy is implemented, Blizzard enables human operators to guide landing, take-off, and mission execution, with future versions integrating vision-based autonomous landing on ground tags.

🎯 2. Project Goals
Develop a robust controller capable of mission-critical operations.

Implement dynamic security protocols for drone-console authentication.

Provide advanced interaction features such as cheatcodes, PID tuning, and dynamic drone prompts.

Integrate real-time data monitoring for safe operations (battery, GPS, orientation, flight data).

🧑‍🔧 3. Core Design Philosophy
✔️ Security: Dynamic password authentication between Blizzard and drones.
✔️ Modularity: Hardware and software scalable for different drone models.
✔️ Usability: Intuitive menu navigation via keypad and digital volume knob.
✔️ Interoperability: Ability for hospitals, delivery centers, or operators to share drones seamlessly.

🏗️ 4. Hardware Components
Component	Purpose
Arduino Nano	Main microcontroller running Blizzard code
9V Battery + Voltage Divider	Power supply with voltage scaling for safe reading
HC-12 Transceiver	Wireless communication with the drone
Matrix Keypad	Input interface for menu navigation and cheatcode entry
Digital Volume Encoder	Scroll and select options, adjust variables, brightness, contrast
I2C LCD Screen	Display menu, flight data, battery status, user prompts
GPS Module	Provides Blizzard’s position for coordination and drone rescue
Magnetometer	Determines magnetic orientation
Radio TX (FSi6 X)	Standard RC communication (planned integration)
Goggles (with gyroscope)	For drone head-tracking in future versions

⚙️ 5. Software Functionalities
🔹 5.1 Basic Functions (Part A)
✅ Hardware initialization (keypad, LCD, encoder, HC-12)
✅ Basic menu system for navigation
✅ Real-time battery voltage monitoring with alarms
✅ Display of flight data (lat, long, altitude, drone UID)
✅ Communication protocols setup with drone via HC-12

🔹 5.2 Advanced Features (Part B)
✅ Nested dynamic menu system with interactive controls
✅ Cheatcode system for advanced drone commands
✅ Access code security with dynamic password updates
✅ Drone execution prompts for PID tuning, altitude lock, mission commands
✅ Real-time data streaming from drone to Blizzard with alarms for out-of-range values
✅ GSM integration (planned) for remote drone instructions
✅ Bidirectional acknowledgment system for reliable data transfer

🔑 6. Security and Access
Each Blizzard-console–drone connection is secured with a dynamic password system, updated as needed.

Proximity-based pairing prompts ensure user intention before granting access.

✈️ 7. Use Case Example
✔️ Scenario: Aga Khan Hospital sends a drone to KNH.
✔️ Both sites have Blizzard consoles.
✔️ Operators authenticate using their Blizzard’s password, command landing, unload, and send the drone back.

📝 8. Development Strategy
Phase 1 – Foundational Functions

Hardware interfacing

Basic menu and data display

Establish HC-12 communication

Phase 2 – Advanced Algorithms & Security

Cheatcode system implementation

Dynamic password and access security

Drone execution prompts integration

Bidirectional acknowledgment protocol

Phase 3 – Expansion & Integration

GSM module integration

Autonomous landing/tag detection

Goggles head-tracking control

OTA firmware updates

📂 9. Appendices (To be developed)
Schematics and wiring diagrams

Pinout tables

Glossary of terms

Full function list with inputs, outputs, and flowcharts

Troubleshooting guide

