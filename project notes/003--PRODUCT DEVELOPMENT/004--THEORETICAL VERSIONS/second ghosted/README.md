# Velma

this is an explanatory file for each of the modules, their test results, and quality validation for each where necessary. this is what someone reads to understand what everything does, without understanding how everything is done.. this is for the user....... the user who uses the drone the way it is. and the protocols needed to run velma at a user complex level, while maintaining ease of use
<!-- Table of Contents -->
<!-- notes to the customer, on what velma realy is, the user wanting to use the platform -->
## Table of Contents

- [Velma](#velma)
  - [Overview](#overview) 🔍
  - [Scope](#scope) 🔭
    - [Features](#features)
    - [Functionality](#functionality)
      - [Battery](#battery)
      - [GPS](#gps)
      - [Gyroscope](#gyro)
      - [Datalogger](#data-logger)
      - [HC12](#hc-12)
  - [Goal](#goal) 🎯
- [Architecture and Design](#architecture-and-design)
  - [System Requirement](#system-requirement) ⚙️
  - [Module](#module) 🧩
  - [Design](#design) 🖌️
    - [MicroProcessor](#1-microprocessor)
    - [Libraries](#2-libraries)
    - [Schematics](#3-schematics)
    - [HardwareRequirements](#4-hardware-requirements)
    - [SoftwareRequirements](#5-software-requirements)
  - [Logic](#logic) 💡
- [Setup](#setup)

<!-- Content -->
## Overview 🔍

<a id="overview"></a>

Velma serves as an open-source flight algorithm platform, designed to control unmanned aerial vehicles (UAVs) autonomously. It acts as a learning tool, with contributors being the users themselves. By integrating various sensors, software systems and algorithms, Velma enables UAVs to operate without direct human control, performing tasks such as navigation, obstacle avoidance, and mission planning and execution.

## Scope of velma 3 🔭

<a id="scope"></a>

the feasibility scope of velma 3 and what is is supposed to achieve, features it bring to the table,
this should explain, everything from manual control, to autonomy,
#######################################################################
Savoy, building upon the foundation laid by Velma, will innovate and create additional products and solutions. These products include advanced drone systems, software applications, or integration tools, aimed at addressing specific industry needs and challenges. By harnessing the capabilities of Velma, Savoy will continue to drive innovation in the field of drone technology, paving the way for future advancements and applications.

## Goal 🎯

<a id="goal"></a>

Velma, at its core, embodies a vision of innovation and education within the drone technology realm. It aspires to transcend mere hardware and software, serving as a beacon for enthusiasts and learners alike. Velma's mission revolves around empowering individuals through exploration, learning, and community engagement.

The primary goal of Velma V3 is to create a comprehensive path of drone development, and the development of the curriculum, from the basics and leading to a reasonable level of understanding. This version aims to demystify the workings of drones in the software and the physical world and to demonstrate how coding unleashes true creativity. The development of Velma V3 fills a crucial gap in educational materials, providing detailed hardware and software information, along with schematics and physical products.
Cube V4 represents a significant advancement, offering distinct variations of the cubelike structure tailored for different applications. These versions serve as curriculum drones, seamlessly integrating with various chassis made by Savoy for software development purposes. Through the Savoy Research Institute, these curriculum drones will serve as educational tools, teaching programming and problem-solving skills to students. Each iteration of Cube V4 presents new challenges, covering topics such as complex manual control, mapping and navigation, battery management systems, and machine vision.

Additionally, Cube V4 uses firmware to integrate with a wide range of drone brands, utilizing a master-slave communication protocol. This versatility enables users to harness the power of Cube V4 across different available drone platforms, enhancing their capabilities through complex control algorithms and customization. The inclusion of a Raspberry Pi running Linux further expands Cube V4's autonomy, enabling autonomous drills and advanced functionalities such as cruise control and machine vision.

Ultimately, the goal is to establish infrastructure that enables drones to perform complex tasks, such as deliveries, with ease. in the near future, by leveraging Cube V4 and its associated software, individuals will have the ability to send personal drones to pick up parcels, revolutionizing the concept of personal deliveries. This ambitious vision underscores Velma's commitment to pushing the boundaries of drone technology and fostering innovation in the field.

encourage testability and reusability for embedded systems code

### Features

<a id="features"></a>

this is a collection of everything the drone can do, both as basic and complex configurations, these will then be explained in the Hard and soft readme's

### Functionality

<a id="functionality"></a>

#### 1. Velma, the drone

cube vs blizzard

##### Manual Control
THe droe is capable of normal flight

The primary functionality of Velma V3 revolves around manual control of the drone. Users utilize a ground station called Blizzard, comprising of the radio transmitter, an arduino nano and the peripheral devices, to manually conduct the drone's flight maneuvers in real-time.

##### Camera

This is a camera with IR and light LEDS around it, toggled on and off via a transistor, via a cheatcode. enables light amplification night vision
THe camera is useful when performing FPV flight, 

##### GPS

<a id="gps"></a>

The Neo6M GPS module integrated into the Velma drone computer provides essential location data for navigation and waypoint tracking. Here's how the GPS coordinates are utilized:

Data Acquisition: The GPS module receives PVT information in NMEA sentences through a hardware Serial pin on the Arduino Mega. These sentences include parameters like latitude, longitude, time, velocity, altitude, and satellite information.

Parsing NMEA Sentences: Relevant information is parsed from NMEA sentences, specifically GGA and RMC sentences, which provide latitude, longitude, and time data.

Destination Coordinates Input: Coordinates inputted by the user through the Blizzard remote control interface represent the desired destination point for the drone.

Calculating Distance: The Haversine formula calculates the distance between the current position and destination coordinates.

Calculating Destination Heading: Trigonometric functions calculate the destination heading using the atan2 function.

Incorporating Magnetometer Heading: The magnetometer provides more accurate drone orientation. Its readings are used to calculate the greedy heading.

Blizzard Integration: Utilizes GPS coordinates for various functionalities using the cheat code [Shepherd_r]. This mode guides the drone back to a designated home location and provides heading and distance information in case of a crash.

Waypoint System: Implements a waypoint system using GPS coordinates via the cheat code [Oversee_r]. This enables waypoint navigation and route planning.

NMEA-0183 Sentences for GPS Receivers
The NMEA-0183 standard formats GPS information into sentences. Some commonly used sentences include GGA, GSV, and RMC sentences. They provide data like position, fix type, number of satellites, speed, heading, and more.

##### Gyroscope

<a id="gyro"></a>
The MPU6050 module measures angular motion over time across the x, y, and z axes to determine the drone's rotational position. Additionally, it includes an onboard accelerometer to measure acceleration along the same axes. These gyroscope and accelerometer data are crucial for both Acrobatic and Self-Leveling Modes.

Motion Tracking: The combined data enables Velma V3 to dynamically track the drone's motion, velocity, and orientation during flight, forming the basis for flight control algorithms and maneuvering.

Derived Parameters: Velma V3 calculates pitch, roll, and yaw angles using gyroscope and accelerometer data, essential for flight stabilization, navigation, and control.


Data Analysis
Angular Velocity: The gyroscope measures the drone's rotational speed in degrees per second (°/s) around the X, Y, and Z axes.

Gyroscope Drift: Over time, gyroscopes may experience drift, causing errors in angular velocity measurements. Velma V3 compensates for this drift to maintain accurate orientation estimation.

Acceleration: The accelerometer measures linear acceleration in meters per second squared (m/s^2) along the X, Y, and Z axes.

Orientation: By integrating accelerometer data over time, Velma V3 determines the drone's orientation relative to the Earth's surface, crucial for stable flight attitudes and self-leveling.

Combined Data Analysis
Inertial Measurement Unit (IMU) Fusion: Velma V3 combines gyroscope and accelerometer data using complementary filtering techniques to achieve accurate and stable orientation estimation in real-time.


##### Data logger

<a id="Datalogger"></a>
Records flight data and performance metrics

##### HC-12

<a id="HC12"></a>
Facilitates communication between ground control station and drone

##### Radio TX [BLIZZARD]

<a id="Blizzard"></a>
Blizzard incorporates a radio transmitter for controlling the drone's functions and receiving telemetry data, providing a seamless user interface for piloting and monitoring.

##### ULTRASONIC

<a id="Ultrasonic"></a>
An ultrasonic sensor is integrated into the system for precise altitude measurement and obstacle detection, enhancing flight safety and autonomy.

##### Flight Modes

 1. Acrobatic Mode
The drone toggles Acrobatic Mode, The drone uses angular velocity only to fly in acrobatic Mode to perform agile maneuvers such as flips, rolls, and spins with precision and responsiveness.

Implementation:
Gyroscope data processing integrates raw angular velocity data over time to calculate the drone's orientation.
Control inputs from the radio transmitter adjust the drone's angular velocities for dynamic maneuvering.
Real-time feedback ensures precise execution of acrobatic maneuvers using PID loops.

2. Self-Leveling Mode
Self-Leveling Mode utilizes both gyroscope and accelerometer modules to maintain a stable and level flight attitude, By combining data from both sensors, the drone calculates its orientation relative to the horizon and applies corrective actions to maintain stability.

Implementation:
Gyroscope and accelerometer data fusion compensates for drift and provides stable flight.
A complementary filter combines data to create a robust IMU, ensuring accurate orientation estimation.
Real-time adjustment of motor outputs based on IMU data maintains stability during flight, even in the presence of external disturbances.
Perfect IMU Integration
Velma V3 integrates gyroscope and accelerometer data fusion techniques to create a "perfect" IMU. This ensures accurate angle calculation and compensation for drift and vibration-induced errors.

Gyroscope Control
Blizzard incorporates gyroscope systems for intuitive control:

Upright Mode (P.Power): Toggles a feature enabling the drone to return to an upright orientation from any position in 3D space, enhancing stability and control.
SeeMeStill: Enables control of the drone's direction based on the user's head movements, utilizing onboard gyroscopes within goggles for an immersive flying experience.

##### Module(Cube) Overview 🧩

The existing modules include:

###### 1. Microprocessor

Arduino Mega: Serving as the core of Velma V3, the Arduino Mega provides the necessary processing power and interfacing capabilities for controlling the drone's operation.

###### 2. Libraries

List of Libraries:

MPU6050 Library: Facilitates interfacing with the gyroscope and accelerometer module.
HC12 Library: Enables wireless communication between the drone and ground station.
Servo Library: Controls drone motors and servos.
GPS Library: Parses NMEA sentences and obtains GPS data.
Complementary Filter Library: Implements sensor fusion algorithms.

###### 3. Schematics

Detailed schematics illustrate the connections between components such as the Arduino Mega, MPU6050 module, HC12 transceiver, GPS module, and servo motors. Both logic and stripboard schematics are provided, recommended for beginners.

###### 4. Hardware Requirements

To build Velma V3, you'll need:

Arduino Mega microcontroller board.
MPU6050 gyroscope and accelerometer module.
HC12 transceiver modules for drone and ground station communication.
GPS module (e.g., NEO-6M) for obtaining location data.
Servo motors for controlling drone movement and stability.

###### 5. Software Requirements

To program and operate Velma V3, you'll need:

Arduino IDE: for writing, compiling, and uploading firmware to the Arduino Mega.
Required Libraries: MPU6050, HC12, Servo, GPS, Complementary Filter.
Ground Control Software: for receiving telemetry data and sending commands to the drone during flight.

### 2. Ground Control Station: Blizzard

Central hub for drone operations
Components:
Arduino Nano
Keypad
I2C LCD Screen
Digital Volume (Interface Key)
Switches for power control
Capable of switching between direct radio data transmission and HC12 transceiver communication

Velma V3 seamlessly integrates with the Blizzard system, leveraging HC12 transceivers onboard both the drone and the Blizzard controller. This integration expands beyond basic flight control channels, enabling transmission of complex commands and data between the drone and the ground station.

#### Cheat Codes

the user wil read this, provide a list of cheatcodes for the user,

Cheat codes, often associated with video games, are sequences of commands or inputs that unlock specific features or abilities within the game. In the context of drones, cheat codes refer to pre-defined commands or shortcuts that streamline various functionalities of the drone, making it easier for users to access and utilize advanced features.

Here's an overview of their benefits in the context of drones:

1. Ease of Use: Cheat codes simplify the operation of drones with numerous functionalities by providing users with quick shortcuts to access specific features. Instead of navigating through complex menus or commands, users can input a cheat code to instantly activate a desired function.

2. Efficiency: With cheat codes, users can perform tasks more efficiently and effectively, saving time and effort. For example, a cheat code might enable a drone to automatically return to its home point, perform a specific flight maneuver, or adjust camera settings without the need for manual input.

3. Accessibility: Cheat codes make advanced drone functionalities more accessible to a wider range of users, including beginners and those with limited technical knowledge. By simplifying complex operations into easy-to-use commands, cheat codes lower the barrier to entry for drone enthusiasts of all skill levels.

4. Versatility: Drones often come equipped with a wide array of functionalities, ranging from basic flight controls to advanced camera features and autonomous flight modes. Cheat codes enhance the versatility of drones by allowing users to access and utilize these functionalities in customizable ways.

5. Customization: Some cheat codes can be customized or programmed by users to suit their specific needs or preferences. This flexibility enables users to tailor their drone's capabilities to match their unique requirements, whether it's for recreational flying, professional photography, or research purposes.

6. Safety: Certain cheat codes are designed to enhance safety during drone operation by implementing fail-safe mechanisms or emergency procedures. For example, a cheat code might trigger an automatic return-to-home function in the event of low battery or signal loss, helping to prevent accidents or lost drones.

7. earning and Exploration: Cheat codes can serve as educational tools, allowing users to experiment with different drone functionalities and learn how they work. By providing a hands-on way to explore the capabilities of their drones, cheat codes encourage users to become more proficient pilots and developers.

Blizzard features cheat codes to activate specific functionalities:

    Shepherd Mode: Guiding the drone back to a designated location
    Nightcrawler: Activating infrared night vision
    G.Power: Toggling Upright Mode feature
    Overseer: Initiating waypoint system
    SeeMeStill: Enabling gyroscope-based control

### Parent console

has admin priviledges over the child console, limiting speed, stabilization, etc
can change blizzard levels for child console,

### Child console

### Password interface

### 3. Battery Management Systems

<a id="battery"></a>

Measures voltage for battery percentage
Implements power management and predictive consumption methods

## Architecture and Design

Document both

### System Merit ⚙️

<a id="system-requirement"></a>

Velma V3 System Requirements

### 4. Enhanced Transmission Capabilities

The HC12 transceivers enhance transmission capabilities beyond traditional throttle, yaw, pitch, and roll channels. Users can explore advanced control functionalities, telemetry data exchange, and sensor feedback, fostering a deeper understanding of drone operations.

### 5. Educational Infrastructure

Velma V3 serves as a foundational educational infrastructure for drone technology. It provides a safe and controlled environment for students to gain hands-on experience, experiment with different flight scenarios, and develop proficiency in drone piloting and control.

### 6. Scalability and Expandability

The Velma V3 system is designed with scalability and expandability in mind. As students progress in their learning journey, the system can accommodate additional modules, sensors, and peripherals to explore more advanced concepts such as autonomous flight, computer vision, and payload delivery.

### 7. Safety and Reliability

Safety and reliability are paramount in Velma V3's design. The system incorporates robust fail-safes, emergency stop mechanisms, and redundant communication protocols to ensure safe operation during training sessions and experiments.

### 8. Accessibility and Affordability

Velma V3 prioritizes accessibility and affordability, making drone education accessible to a wide range of students and educational institutions. The system's cost-effective design and open-source nature facilitate widespread adoption and participation.

### 9. Collaboration and Community Engagement

Velma V3 fosters collaboration and community engagement within the drone education ecosystem. Students are encouraged to share their experiences, collaborate on projects, and contribute to the ongoing development and improvement of the Velma platform.

### 10. Learning Outcomes

At its core, Velma V3 aims to achieve specific learning outcomes, including proficiency in manual drone control, understanding of basic flight dynamics, familiarity with transmission protocols, and appreciation for the broader applications and implications of drone technology.

## Setup

<a id="setup"></a>

Insert your content for Setup here.

use programming standards, whitespacing etc,
