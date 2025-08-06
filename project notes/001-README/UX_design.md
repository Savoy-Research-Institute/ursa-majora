This document outlines the user experience (UX) design principles, objectives, and interactions for the Ursa Minor drone system, including both the drone (Velma) and the ground station (Blizzard). The goal is to ensure an intuitive, responsive, and seamless experience for users during setup, operation, and flight.

UX Design Philosophy
The UX design of Ursa Minor prioritizes usability, feedback, and efficiency to ensure that both expert and novice users can interact with the system smoothly. Whether piloting the drone in real-time or executing autonomous missions, the design ensures:
Clarity in presenting essential data.
Responsiveness in system feedback (visual, auditory, and haptic).
Error Prevention and Recovery by making complex features easy to use and failures manageable.

User Personas
Pilots / Hobbyists:
Require intuitive flight controls and real-time telemetry data.
Expect responsive feedback (visual, sound, and haptic) to make flight adjustments.
Field Technicians / Testers:
Need quick access to diagnostics and system health metrics.
Must monitor hardware performance (battery temperature, motor health) efficiently.
Developers / Engineers:
Require easy access to configuration menus for calibrating sensors and setting waypoints.
Need to perform software tests and validate flight logic without friction.

Key UX Elements
1. System Startup Feedback
Purpose: Ensure the user knows the system is powered on and ready to use.
Implementation:
Blizzard:
LEDs flash in sequence and the buzzer beeps.
Vibratory motors activate briefly to confirm startup completion.
Velma:
LEDs blink in a pattern, and the motors spin momentarily to confirm startup.
Calibration Feedback:
Blizzard shows calibration progress on the LCD.
Audible feedback (buzzer) confirms sensor calibration success or failure.

2. Real-Time Feedback System
Visual Feedback:
Telemetry data (altitude, battery percentage, GPS position) is shown on Blizzard's LCD screen.
LED Indicators:
Green LED: Normal operation.
Red LED: Critical error (e.g., low battery).
Yellow LED: Warning (e.g., weak GPS signal).
Auditory Feedback:
Buzzer alerts for specific events (e.g., low battery, lost signal, RTH triggered).
Haptic Feedback:
Vibratory motors in Blizzard give tactile feedback for critical alerts.

3. User Interface on Blizzard (Ground Station)
Keypad Navigation: Keypad allows users to select flight modes, set waypoints, and adjust settings.
Clear Input Feedback: Button presses are acknowledged with sound or LED blinks.
Digital Volume Knob: Adjusts real-time parameters like drone speed, altitude targets, or display
brightness.
LCD Screen UI: Displays multiple screens:
Home Screen: Displays drone status (altitude, battery level, mode).
Settings Screen: Sensor calibration, GPS settings, and flight mode selection.
Mission Screen: Waypoints and flight path status.

4. Navigation and Flight Mode Selection
Flight Mode Switching:
User selects flight mode (manual, autonomous, RTH) using the keypad.
Current mode is displayed on the LCD screen and indicated by LED color.
Haptic feedback confirms mode changes.
Waypoint Navigation:
Users can input or adjust waypoints using the keypad and digital volume knob.
Each waypoint confirmation is followed by visual feedback on the LCD and a short beep.
Dynamic Home Location:
Users can set a new home location (where Blizzard is) during the flight.
A message appears on th
