This skeleton file outlines the altitude control logic and the role of sensors, flight algorithms, and PID tuning for maintaining and adjusting altitude.

Purpose
Altitude control ensures that the drone:
Maintains a specific altitude during flight.
Adjusts altitude smoothly in response to user inputs.
Automatically compensates for disturbances (e.g., wind) or changes in pressure.
Handles takeoff, hover, and landing phases safely and efficiently.

Functional Components
1. Altitude Sensors and Inputs
Barometer (Primary Sensor):
Measures atmospheric pressure to determine the drone’s height above sea level.
Requires calibration on startup to define the home altitude.

Ultrasonic Sensor (For Low-Altitude Control):
Used for precise height detection near the ground, especially during landing.
Effective in the 0–4.5m range, where barometers can be less reliable.

GPS Altitude Data:
Provides supplementary altitude readings for high-altitude flights.
Less accurate than barometers but useful as a backup during emergencies.

2. Flight Controller Logic
The flight controller uses PID control to manage the drone’s altitude based on sensor inputs.

Throttle Control Loop:
Converts pilot commands or programmed instructions into throttle adjustments.
If altitude error = desired altitude - current altitude, the drone increases or decreases throttle to correct the difference.

Hover Mode:
In hover mode, the flight controller tries to maintain a fixed altitude with minimal pilot input.
Uses barometer readings and PID loops to ensure stability.

Altitude Control Modes
Manual Mode (Throttle-Only)
The pilot controls the throttle directly, without any automated altitude stabilization.
Ideal for expert pilots who want full control over the drone’s altitude.

Altitude Hold Mode
The flight controller maintains the current altitude without needing continuous throttle input from the pilot.
Uses the barometer as the primary input and adjusts motor speeds automatically to maintain altitude.

Auto Takeoff and Landing Mode
Auto Takeoff:
The drone increases throttle gradually to reach a safe hover altitude (~2 meters).
Uses barometer data and smooth PID adjustments to avoid sudden thrust spikes.

Auto Landing:
When commanded, the drone decreases altitude smoothly.
Uses ultrasonic sensors for precision in the final few meters to ensure a soft landing.

PID Tuning for Altitude Control
Proportional (P): Corrects altitude errors based on how far the drone is from the target altitude.
Integral (I): Compensates for persistent altitude drift over time.
Derivative (D): Prevents overshoot by predicting future altitude changes based on current speed.

Error Handling and Fail-Safes
    Barometer Drift Compensation:
If the barometer detects an unrealistic change in altitude (e.g., due to pressure changes), the system triggers a recalibration.
    GPS Fall-back Mechanism:
If barometer readings become unreliable, the system switches to GPS altitude data for basic control.
    Low Altitude Detection for Safe Landing:
If the drone detects a critical battery level, it descends automatically using ultrasonic sensors to ensure a smooth landing.
    Failsafe Return to Home:
In case of signal loss, the drone returns to either the home location or the Blizzard’s current location, maintaining safe altitude throughout.

Feedback Mechanisms and Notifications
LED and Buzzer Feedback:
LEDs flash if the drone reaches a predefined altitude or if the altitude hold mode is activated.
A buzzer sounds during descent to warn nearby people or in case of emergency auto-landing.

Future Considerations
Terrain Following Mode:
Use LIDAR or additional ultrasonic sensors to adjust altitude dynamically based on the ground’s topography.
the production of a cube that can connect to all these devices, such that you only need produce two, one for blizzard and one for velma that runs flight mode, if the cube has a screen and a keypad, hover tests etc could be run on the drone without needing a ground station, this will be a source of material when showing how intelligent the drone actually is 

Altitude Data Logging:
Store altitude data for analysis and post-flight diagnostics.
Useful for assessing flight performance or tuning PID values.

