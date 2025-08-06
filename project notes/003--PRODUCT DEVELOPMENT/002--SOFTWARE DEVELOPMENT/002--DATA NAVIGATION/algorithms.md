# Ursa Minora – Algorithms Documentation

This document details the algorithms used in Ursa Minora for flight stabilization, sensor fusion, and control logic.

---

## 🎯 **1. Purpose and Scope**

Algorithms are used to:

- Fuse raw sensor data into meaningful orientation and motion information  
- Stabilize flight by controlling motor outputs dynamically  
- Enable future autonomous navigation and environmental awareness

---

## 🧠 **2. Implemented Algorithms**

### 🔀 **2.1 Complimentary Filter**

**Purpose:**  
Combines accelerometer and gyroscope data to estimate pitch and roll with minimal drift and noise.

**How it works:**  
- Accelerometer gives absolute orientation but noisy during motion  
- Gyroscope gives relative angular velocity but drifts over time  
- Complimentary filter merges them using a weighted average:

angle = alpha * (previous_angle + gyro_rate * dt) + (1 - alpha) * accelerometer_angle


**Notes:**  
- `alpha` is typically ~0.96 for quick but stable response  
- Used in `read_mpu_gyroscope()` function in the current codebase

---

### 📐 **2.2 Kalman Filter**

**Purpose:**  
Provides an optimal estimate of orientation by modeling measurement noise and system noise statistically.

**How it works:**  
- Maintains an internal estimate and updates it with new measurements  
- Weights measurements based on their uncertainty (covariance matrices)

**Implementation:**  
- Computationally heavier than complimentary filter  
- Not yet implemented in current Ursa Minora code; planned for future for precise stabilization

---

### ⚖️ **2.3 PID Controller**

**Purpose:**  
Controls the motor output to maintain desired orientation or position.

**How it works:**  
- Calculates error = desired - actual  
- Outputs control signal based on Proportional, Integral, and Derivative terms:

output = Kp * error + Ki * integral(error) + Kd * derivative(error)


**Usage:**  
- To be implemented in motor control logic for stabilization and altitude hold

---

## 🔭 **3. Future / Planned Algorithms**

### 🚀 **3.1 Autonomous Navigation**

- Waypoint navigation using GPS (Neo-6M)  
- Obstacle avoidance using ultrasonic sensor  
- Path optimization algorithms (e.g. A*, Dijkstra) for delivery or mapping tasks

---

### 🧩 **3.2 AI-based Path Planning**

- Neural network-based real-time decision making  
- Requires training datasets from flight logs and simulated environments

---

## 📊 **4. Mathematical Foundations**

| **Algorithm** | **Core Concept** | **Application** |
|---------------|------------------|-----------------|
| Complimentary Filter | Weighted average of gyro and accel data | Stabilization |
| Kalman Filter | Bayesian estimation minimizing variance | High-precision orientation |
| PID Controller | Control theory minimizing error over time | Motor control, attitude hold |
| A* / Dijkstra | Graph-based shortest path search | Navigation |

---

## ⚠️ **5. Implementation Notes and Limitations**

- Complimentary filter suffices for basic stabilization; Kalman filter improves precision at computational cost.  
- PID tuning is critical; improper gains can cause oscillation or sluggish response.  
- AI-based algorithms require significant data and off-board training.

---

## 🔗 **6. Integration in Codebase**

| **Function** | **Algorithm Used** | **Description** |
|--------------|--------------------|-----------------|
| `read_mpu_gyroscope()` | Complimentary filter | Fuses gyro and accel data to compute roll and pitch |
| `read_mpu_accelerometer()` | N/A (raw calculation) | Calculates accelerometer angles directly |
| Future PID functions | PID control | Adjusts motor PWM signals to maintain orientation |

---

### ✅ **Next Steps**

- [ ] Draft complimentary filter implementation notes with code snippets  
- [ ] Draft PID controller implementation plan  
- [ ] Plan Kalman filter integration for future sprints

---

