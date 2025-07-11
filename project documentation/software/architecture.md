# Ursa Minora – Software Architecture Documentation

This document describes the software architecture of Ursa Minora, including its design philosophy, structure, and core modules.

---

## 🎯 **1. Purpose**

The software architecture defines how code modules, functions, and hardware interfaces integrate to achieve:

- Real-time data acquisition and processing  
- Stable and responsive flight control  
- Modularity for scalability and future expansion (e.g. autonomous navigation)

---

## 🏗️ **2. Design Philosophy**

✔️ **Modular:** Separate concerns into clear modules (sensors, control, communication)  
✔️ **Scalable:** Easy to add features (PID, GPS navigation, AI modules) without disrupting existing systems  
✔️ **Readable:** Documentation and naming conventions prioritize clarity for future collaborators

---

## 🔧 **3. System Overview**

### **3.1 Core Components**

| **Module** | **Description** | **File/Folder** |
|------------|-----------------|-----------------|
| **Sensor Interface** | Reads raw data from MPU6050, BMP280, Neo-6M GPS, etc. | `/src/sensors/` |
| **Data Processing** | Filters, converts, and fuses sensor data (complimentary/Kalman filters) | `/src/processing/` |
| **Control Algorithms** | Implements PID controllers, stabilization logic | `/src/control/` |
| **Motor Control** | Sends PWM signals to ESCs to adjust motor speeds | `/src/motors/` |
| **Flight Modes** | Logic for Acro, Self-Leveling, Altitude Hold, Autonomous | `/src/modes/` |
| **Communication** | Handles serial, telemetry, and potential radio comms | `/src/comms/` |
| **Utilities** | Helper functions, constants, math utilities | `/src/utils/` |

---

### **3.2 Code Flow**

Setup()
└── Initialize hardware modules (sensors, motors, comms)
Loop()
├── Read sensor data
├── Process/filter data
├── Run control algorithms
├── Update motor outputs
└── Handle communication and mode switching


---

## 📂 **4. File Structure**

/src
├── sensors/
│ ├── mpu6050.cpp/.h
│ ├── bmp280.cpp/.h
│ └── gps_neo6m.cpp/.h
├── processing/
│ ├── filters.cpp/.h
│ └── kalman.cpp/.h
├── control/
│ ├── pid.cpp/.h
│ └── stabilization.cpp/.h
├── motors/
│ └── motor_control.cpp/.h
├── modes/
│ └── flight_modes.cpp/.h
├── comms/
│ └── serial_comms.cpp/.h
└── utils/
└── math_utils.cpp/.h


---

## ⚙️ **5. Implementation Details**

### 🔍 **5.1 Sensor Interface**

- Uses I2C to communicate with MPU6050 and BMP280  
- Uses UART for GPS modules  
- Functions: initialization, read, reset, self-test

---

### 🧠 **5.2 Data Processing**

- Complimentary filter (current)  
- Kalman filter (planned)  
- Normalizes and scales raw data to physical units

---

### ⚖️ **5.3 Control Algorithms**

- **PID Controllers** (planned): Controls roll, pitch, yaw outputs  
- **Stabilization Logic**: Uses processed sensor data to calculate correction outputs

---

### ✈️ **5.4 Flight Modes**

- Acro Mode: Manual angular rate control  
- Self-Leveling Mode: Automatic leveling via accelerometer data  
- Altitude Hold (future): Maintains constant altitude using barometer/ultrasonic  
- Autonomous Mode (future): Executes GPS waypoint navigation

---

### 🔌 **5.5 Motor Control**

- Generates PWM outputs via ESC interfaces  
- Converts control outputs to motor speed adjustments

---

### 📡 **5.6 Communication**

- Serial monitor debugging  
- Telemetry integration (planned) for live data streaming to GCS

---

## 🚀 **6. Planned Extensions**

- **Modular Scheduler:** For task management and timing precision  
- **RTOS Integration:** If ported to more capable microcontrollers  
- **AI Modules:** For environmental awareness, path planning, and fault detection

---

## ⚠️ **7. Limitations**

- Current implementation is single-threaded loop-based  
- Hardware abstraction is minimal; future refactoring into Hardware Abstraction Layers (HAL) is recommended for scalability to STM32 or other platforms

---

## 🔗 **8. Integration with Hardware**

| **Software Module** | **Hardware Interface** | **Notes** |
|----------------------|-------------------------|-----------|
| Sensor Interface | I2C (MPU6050, BMP280) | 3.3V level shifting required |
| GPS Module | UART | Uses NMEA sentence parsing |
| Motor Control | PWM to ESCs | Ensure correct calibration |
| Communication | USB Serial / Radio | Future telemetry integration planned |

---

## ✅ **Next Steps**

- [ ] Finalize module directory structure in repository  
- [ ] Document PID controller codebase  
- [ ] Integrate flight mode switching logic  
- [ ] Plan telemetry communication module

---

