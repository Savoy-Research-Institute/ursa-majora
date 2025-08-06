This document outlines the power management strategies for the drone system, focusing on efficient energy use, battery monitoring, and thermal management. Effective power management is critical for maximizing flight time and ensuring the reliability and safety of the drone's operations.

1. Power Supply System
  1.1. Battery Type
Lithium Polymer (LiPo) Batteries: The primary power source for the drone, selected for their high energy density and discharge rates.
Specifications: voltage 12.6v, capacity 2200(mAh), and 3 cells.
  1.2. Battery Configuration
the cube and shell are powered by a battery, they connect via an xt60 connector

2. Battery Management System (BMS)
  2.1. Monitoring
Voltage Monitoring: Regular checks on individual cell voltages to prevent over-discharge.
Current Monitoring: Real-time tracking of current draw to ensure it stays within safe limits.
this is done via a voltage divider, because the arduino mega cannot process logic over 5v, 12.6v just kills the board, the battery voltage is used to calculate battery capacity, 
  2.2. Protection Mechanisms
Over-discharge Protection: Automatically disconnects the load when the battery voltage falls below a certain threshold.
Over-current Protection: Prevents excessive current draw that could damage the battery or connected components.
Thermal Protection: Monitoring of battery temperature, utilizing a thermocouple to ensure safe operating temperatures.

3. Energy Efficiency Strategies
  3.1. Motor Control
Speed Regulation: Adjusting motor speeds dynamically based on flight conditions and maneuvers to optimize power usage.(REF; esc_output.cpp)  
PWM Control: Using Pulse Width Modulation (PWM) signals from the flight controller to control motor speed accurately.(REF; CombinedRadioHC.cpp)
  3.2. Power Distribution
Ensuring an efficient layout of power distribution within the drone, minimizing voltage drops and power losses.  apart from the PDB on the shell, the power board has voltage regulators that step down 12v to stable 5 and 3.3 voltages for powering the sensors

4. Thermal Management
  4.1. Cooling Systems
the cube has fins that direct air into the cube, this floods the interior with moving air, the arduino mega also has a fin on its processor, this cools this specific device
  4.2. Temperature Monitoring
Integration of thermocouples to monitor critical components, particularly the LiPo battery, ensuring they operate within safe temperature ranges.

5. Power Consumption Analysis
  5.1. Estimation of Flight Time
Calculating expected flight time based on current draw, battery capacity, and weight of the drone.
  5.2. Data Logging
Implementing data logging for power usage, allowing for post-flight analysis and optimization of power management strategies.

6. User Interface Feedback
  6.1. Visual Indicators
LED indicators for battery status (e.g., green for good, yellow for caution, red for low).
  6.2. Alerts
Audible alerts via buzzers when battery levels drop to critical thresholds.
haptics for low battery,,brr brr
