# Ursa Minora – Hardware Component Inventory

This document lists and describes the hardware components used in Ursa Minora, including their roles, specifications, and integration notes.

---

## 🛠️ 1. Microcontroller

**Component:** Arduino mega
**Role:** Main flight controller and sensor interface  
**Key Notes:**  
- Operates at 5V logic  
- Limited PWM pins (6 total)  
- Requires external power supply for motor load

---

## 🔧 2. Inertial Measurement Unit (IMU)

**Component:** MPU6050  
**Role:** Gyroscope and accelerometer for orientation and stabilization  
**Key Notes:**  
- I2C communication (3.3V logic; use level shifter if MCU is 5V)  
- Addresses: default `0x68`, adjustable via AD0 pin  
- Registers for gyro, accel, temperature, self-test

---

## ⚡ 3. Power System

**Components:**  
- **Battery:** LiPo 3S or 4S (specify your exact pack)  
- **Voltage Regulator/UBEC:** Steps down battery voltage to stable 5V for Arduino and peripherals  
- **Power Distribution Board (PDB):** Distributes battery power to ESCs and FC safely

**Key Notes:**  
- Verify regulator amperage rating supports total load  
- Ensure correct polarity connections to avoid damage

---

## 🚀 4. Motor System

**Components:**  
- **Brushless Motors:** Model TBD (document specific KV ratings per motor)  
- **ESCs (Electronic Speed Controllers):** Compatible with motor amperage draw, PWM input from Arduino

**Key Notes:**  
- Calibrate ESCs before first flight  
- Mount securely to avoid vibration noise in IMU readings

---

## 🛰️ 5. Radio Receiver

**Component:** [Specify model, e.g. FS-iA6B]  
**Role:** Receives control commands from ground transmitter  
**Key Notes:**  
- Outputs PWM or PPM/SBUS (specify connection method)  
- Requires appropriate wiring to Arduino input pins

---

## 🖥️ 6. Display and Interface Modules

**Components:**  
- **Keypad:** For in-field input (e.g. mode selection)  
- **LCD Display (e.g. 16x2):** Shows sensor data, system status

**Key Notes:**  
- Connect via I2C or parallel depending on module  
- Requires appropriate libraries in Arduino code (`LiquidCrystal`, `Keypad`)

---

## 🌐 7. Communication Modules

**Component:** HC-12 (optional future integration)  
**Role:** Long-range serial communication for telemetry or Cube-to-Cube links  
**Key Notes:**  
- 433MHz, range up to 1km LOS  
- Requires SoftwareSerial or dedicated serial port on Arduino

---

## 📏 8. Sensors (Optional or Future)

**Components:**  
- **Ultrasonic Sensor (e.g. HC-SR04):** Altitude hold and proximity detection  
- **Barometer (e.g. BMP280):** Altitude estimation  
- **Magnetometer:** Heading stabilization

---

### ✅ **Integration Notes**

- All components must share a **common ground (GND)** to function reliably.  
- Power planning is critical; calculate **total current draw** to size battery and regulator correctly.  
- Place the MPU6050 away from vibration sources to minimize drift.

---

### 🔗 **Next: `/docs/hardware/schematics.md`**

This will include wiring diagrams and schematic notes for full system integration.

