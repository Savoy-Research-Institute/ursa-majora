# Ursa Minora – Flight Systems Documentation

This document describes the flight systems implemented in Ursa Minora, including stabilization, flight modes, and control logic.

---

## 🎯 **1. Purpose**

The flight system enables the drone to:

- Maintain stable flight using sensor feedback and control algorithms  
- Switch between different flight modes (acro, self-leveling)  
- Prepare for future autonomous operation

---

## ✈️ **2. Stabilization System**

### 🧠 **2.1 How Stabilization Works**

**Goal:** Maintain or return to desired orientation despite disturbances.

**Core Principles:**

- Uses **gyroscope data** for angular velocity  
- Uses **accelerometer data** for absolute orientation reference  
- Combines them via the **complimentary filter** to calculate roll and pitch angles  
- Outputs control signals to motors to correct orientation

---

### 📐 **2.2 Current Implementation**

| **Sensor** | **Function** | **Usage in Code** |
|------------|--------------|-------------------|
| Gyroscope (MPU6050) | Measures rotational speed in deg/s | `read_mpu_gyroscope()` |
| Accelerometer (MPU6050) | Measures tilt relative to gravity | `read_mpu_accelerometer()` |

**Algorithm Used:** Complimentary Filter  
**Output:**  
- `roll` and `pitch` angles used for stabilization  
- `yaw` is read directly from gyro z-axis but not yet stabilized (planned for future)

---

### ⚖️ **2.3 Planned Enhancements**

- **PID Controllers** to adjust motor speeds based on roll, pitch, yaw errors  
- **Kalman Filter** for precise orientation estimation

---

## 🚀 **3. Flight Modes**

Ursa Minora is designed to support multiple flight modes. Currently implemented and planned modes include:

### ✈️ **3.1 Acro Mode (Rate Mode)**

**Description:**  
- Directly maps stick inputs to angular velocity commands  
- Pilot has full manual control without auto-leveling  
- Ideal for advanced maneuvers and agile flying

**Status:**  
- Partially implemented using gyro data for roll and pitch rates

---

### 🌐 **3.2 Self-Leveling Mode (Angle Mode)**

**Description:**  
- Uses accelerometer to automatically return drone to level orientation when sticks are centered  
- Easier for beginner flight and stable hovering

**Implementation Plan:**

1. Read accelerometer angles  
2. Calculate error between desired (level) and current angles  
3. Adjust motor outputs proportionally

---

### 🔭 **3.3 Altitude Hold Mode**

**Description:**  
- Maintains constant altitude using barometer or ultrasonic sensor

**Status:**  
- Planned for future integration with **BMP280 barometer** or **HC-SR04 ultrasonic sensor**

---

### 🤖 **3.4 Autonomous Mode**

**Description:**  
- Navigates to predefined GPS waypoints or executes programmed flight paths

**Status:**  
- Planned for future expansion with GPS (Neo-6M) and navigation algorithms (A*, Dijkstra)

---

## ⚠️ **4. Implementation Notes and Limitations**

- Current stabilization only uses **complimentary filter** without PID control  
- **Yaw stabilization** is not implemented yet; yaw readings are raw gyro z-axis data  
- Flight mode switching logic is not implemented yet; currently hardcoded for testing

---

## 🔗 **5. Integration in Codebase**

| **Function** | **Purpose** |
|--------------|-------------|
| `read_mpu_gyroscope()` | Calculates roll, pitch, yaw using complimentary filter |
| `read_mpu_accelerometer()` | Calculates accelerometer roll and pitch angles |
| PID control functions (future) | Adjust motor PWM signals to maintain stability |
| Mode selection logic (future) | Switches between Acro, Self-leveling, and other modes |

---

### ✅ **Next Steps**

- [ ] Draft PID control algorithm implementation  
- [ ] Implement flight mode switching logic  
- [ ] Integrate altitude hold sensor readings

---

