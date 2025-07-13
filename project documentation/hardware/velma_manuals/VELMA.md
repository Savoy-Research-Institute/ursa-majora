# Velma 3.1 Drone – User Manual

## Introduction

Velma 3.1 is a drone system designed for **ease of use, autonomous capability, and creative flight control**. This manual outlines the technical scope, features, control logic, and operational protocols of the Velma V3 Cube – the onboard computer used to develop and execute flight control code when docked into the drone system.

---

## 1. Overview

### What is Velma V3 Cube?

The **V3 Cube** is an onboard computing module acting as the drone’s brain. It:

- Stores and runs flight control code.
- Maintains persistent system data (UID, name, total flight time, home coordinates) in EEPROM to prevent data loss on resets.
- Enables **flight limited only by pilot creativity**, not hardware or software constraints.

---

## 2. Key Features

### 2.1 Unique Identification & Data Persistence

- UID and name stored in EEPROM.
- Total flight time and home coordinates saved across resets.

### 2.2 Ease of Use & Creative Flight

- Designed for intuitive piloting.
- Supports complex autonomous flights with simple commands.

### 2.3 Radio Control Integration

- Reads PPM data via `radioREAD()` on pin 11 for:
  - Throttle, yaw, pitch, roll.
  - Two switches and two potentiometers.
- Blizzard Nano can alternatively read channels via direct wiring and transmit via HC-12 transceivers, enabling integrations like **Cleopatra (head-tracking flight)**.

### 2.4 PID Control

- Calibrated to align drone response with stick inputs while stabilizing against external disturbances.

### 2.5 Gyroscope & Accelerometer

- Provides angular rate and acceleration data on x, y, z axes.
- Enables stabilization algorithms and flight mode switching (Acrobatic, Self-Leveling).

### 2.6 Flight Modes

- **Acrobatic Mode:** Angular rate controls tilt; releasing sticks does not auto-stabilize.
- **Self-Leveling Mode:** Uses angular rate integration to return to upright when sticks are released.

### 2.7 Disarm Pin

- Onboard debounce button to disarm the drone for safe handling.

### 2.8 Shell Pin

- Input pull-up pin goes low when Cube is docked to the shell.

### 2.9 Cheat Codes (Scripted Functions)

| Cheat Code   | Description |
|--------------|-------------|
| Cleopatra | Gyro-based head tracking control. |
| Nightcrawler | Infrared LED night vision simulation. |
| Cruise | Maintains set pitch and altitude for straight or curved segments. |
| Hover | Maintains altitude at a fixed point. |
| Pilgrim | Stabilizes drone orientation in 3D space, including inverted. |
| Shepherd | GPS-based drone location retrieval with alarms and LEDs. |
| Turtle | Reverses motors to flip drone upright after an upside-down crash. |

### 2.10 Voltage Divider

- Measures battery voltage (e.g. 12.6V / 2.5 = 5.04V) to estimate remaining capacity and compensate ESC outputs accordingly.

### 2.11 Battery Management System (BMS)

- Communicates with flight algorithms for:
  - Flight planning.
  - Eco-modes and power conservation.
  - Safe charging and battery health management.

### 2.12 HC-12 Transceivers

- For bidirectional communication with Blizzard Nano.

### 2.13 Data Logger

- For script storage, blackbox logging, and retrieval via SD card integration.

### 2.14 Video Transmitter (VTX)

- Streams live video to goggles; future integration of custom data transmission possible.

### 2.15 Ultrasonic Sensor

- Measures altitude from 2.5 cm to 400 cm, enabling:
  - Low altitude cruising.
  - Altitude locking.
  - Assisted landing.

### 2.16 Barometer

- Provides absolute altitude data for altitude hold functions.

### 2.17 GPS Module

- Supplies position, velocity, and time data for:
  - GPS hold.
  - Blind delivery and waypoint missions.
  - Assisted positioning and local clock calibration.
- Stores last coordinates in EEPROM for rapid lock.

### 2.18 Failsafes

- Return-to-home.
- Crash detection and landing.
- Parachute deployment (planned).

### 2.19 Magnetometer

- Measures orientation relative to magnetic north for heading-based navigation.

### 2.20 Indicator LEDs

- Status indications for:
  - Startup and calibration.
  - Docked/undocked states.
  - Module errors and warnings.

### 2.21 Vitals Examination Checkup (VEC)

- Powers on modules and checks for anomalies at startup.

### 2.22 Redundant Systems

- Dual battery setup for improved reliability.

---

## 3. System Schematics & Logic

### Component Connections

Detailed **schematic diagrams** illustrate how the Cube connects to:

- Sensors (IMU, barometer, magnetometer, ultrasonic, GPS).
- Peripherals (HC-12, VTX, datalogger, indicator LEDs).
- Blizzard Nano for extended control features.

### Pinout Charts

Full **Arduino Mega pinout charts** included for:

- Sensor inputs.
- PWM motor outputs.
- Communication interfaces (UART, SPI, I2C).
- Shell and Cube interface pins.

---

## 4. Development & Integration

### Flight Control Algorithms

- **PID Control:** Proportional, Integral, Derivative adjustments for stable dynamics.
- **Sensor Fusion:** Combining IMU and other sensor data for accurate control.
- **Motor Control:** ESC pulse generation and compensation.

### Software Structure

- **Velma.h:** Function prototypes, classes, constants.
- **Velma.cpp:** Implementation of algorithms and system methods.

### Using Velma in Arduino Projects

- Include Velma library in sketches (e.g. `MainFlightCode.ino`).
- Initialize during setup, update in control loops.

---

## 5. Expansion & Future Enhancements

- **GPS navigation** and waypoint systems.
- **Advanced AI/ML algorithms** for adaptive control.
- **Wireless communication protocol** improvements.
- **Machine vision integration** for obstacle avoidance and visual tracking.

---

## 6. Regulatory Compliance

- Adheres to drone safety and operational standards.
- Military applications for delivery and soldier support under research.

---

## 7. Appendices

- **Glossary of terms.**
- **Example codes and configurations.**
- **ICSP connection guides** for additional Arduino Nano integration (slave communication via SPI/ICSP for modular expansion).

---

## End of Manual

For clarifications, schematic diagrams, or team onboarding briefings, contact the Velma development lead.
