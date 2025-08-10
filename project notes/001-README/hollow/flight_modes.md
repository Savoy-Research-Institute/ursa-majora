Overview
The drone system supports multiple flight modes, each designed for different use cases and pilot skill levels. These modes allow flexibility in control, ranging from fully manual inputs to autonomous behavior using sensor data.

1. Manual Mode (Acro Mode)
Description: This mode offers full manual control over the drone. It is primarily used by experienced pilots who want to maneuver the drone without any automatic stabilization.
Gyro Usage: In this mode, only raw rotational data from the gyroscope is used to assist in smooth movements, but the drone does not self-level.
Characteristics:
No angle correction.
Stick input directly translates to rotation rate.
Perfect for aerobatics or acrobatic stunts.
Use Case: Recommended for expert pilots practicing freestyle maneuvers or high-speed flights.

2. Self-Leveling Mode (Angle Mode)
Description: This mode ensures the drone maintains a level attitude when the pilot releases the pitch and roll sticks. It calculates the angle traveled using gyroscope and accelerometer data to stabilize the drone.
Gyro and Accelerometer Usage:
Gyro data tracks rotational movement.
Accelerometer ensures the drone returns to a stable, level position when the controls are centered.
Characteristics:
Great for beginner pilots learning basic controls.
Safer for indoor or low-speed flights.
Prevents the drone from tilting excessively.
Use Case: Ideal for beginners or when precision flying is needed.

3. Altitude Hold Mode
Description: The drone maintains a consistent altitude by adjusting throttle automatically using barometer and accelerometer data.
Sensor Usage:
Barometer detects atmospheric pressure to determine altitude.
Accelerometer helps in maintaining stable flight.
Characteristics:
Reduces the need for continuous throttle management.
Useful for aerial photography and video capturing.
Use Case: When stable altitude is crucial, such as photography or surveying.

4. GPS Hold Mode (Position Hold)
Description: The drone locks into a position using GPS coordinates and maintains that location despite wind or external forces.
Sensor Usage:
GPS for location.
Magnetometer for orientation stabilization.
Characteristics:
Reduces pilot workload by holding position automatically.
Useful for operations that require hovering in a specific spot.
Use Case: Recommended for stationary operations or during strong wind conditions.

5. Dynamic Control Mode
Description: This mode offers complex, adaptive control using multiple sensor inputs. It combines GPS, gyroscope, magnetometer, and throttle data to ensure precise, intelligent control.
Special Feature: When the throttle is set to mid-peak, the drone hovers automatically.
Sensor Usage:
GPS for dynamic navigation.
Gyroscope and magnetometer for stability.
Throttle, pitch, yaw, and roll are actively monitored.
Use Case: Best suited for advanced missions requiring both manual and autonomous behavior.

6. Return to Home (RTH) Mode
Description: The drone autonomously returns to a preset home location or to the Blizzard's current location when triggered.
Sensor Usage:
GPS to navigate back to the home location.
Barometer and gyroscope for safe altitude management.
Behavior:
The drone automatically adjusts its altitude to avoid obstacles during the return.
Can return to either the initial takeoff location or the Blizzard’s current location.
Use Case: Used in low battery situations or when the signal between the drone and Blizzard is lost.

7. Failsafe Mode
Description: In the event of critical errors or signal loss, the drone enters Failsafe Mode to prevent damage or loss.
Behavior:
Can initiate RTH or land safely based on the predefined settings.
LEDs flash a specific error pattern to indicate the failsafe trigger.
Use Case: Emergency situations, signal loss, or low battery.

8. Stabilization Algorithms
Onboard Stabilization (Standalone): The drone can run stabilization algorithms directly on the cube, even without a Blizzard connection.
If placed on the ground and detected to be idle, the drone can hover to one meter automatically before landing.
Blizzard-Controlled Stabilization: When connected to the Blizzard, additional algorithms can run for optimized flight performance.

9. Transitioning Between Modes
Mode Switch: The drone supports switching between flight modes mid-flight using pre-defined controls.
Indicators:
LEDs indicate the current flight mode through color codes or flashing sequences.
Audio signals from the buzzer provide feedback on successful mode transitions.
