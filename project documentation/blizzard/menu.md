Blizzard Console Menu System Documentation
1. Overview
The Blizzard Console menu system is designed to provide an intuitive and robust user interface for configuring, commanding, and monitoring the Velma drone system. It uses a combination of:

Keypad inputs (matrix keypad)

Digital volume encoder (scroll, select, adjust values)

I2C LCD display for real-time feedback and menu rendering

2. Design Philosophy
Minimal cognitive load: Clear, logically grouped menus with unambiguous labels.

Fail-safe navigation: Ability to back out of any submenu without interrupting critical operations.

Quick access to essential functions like battery status, connection status, and emergency commands.

3. Menu Structure
Root Menu
Home Screen (screensaver)

Displays battery level, GPS coordinates (lat, long, alt), drone UID

Shows connection status with icon or text

Main Menu

Drone Control

Takeoff / Land

Cruise Altitude Settings

PID Tuning

Sensor Calibration

Console Settings

Display Brightness

Display Contrast

Volume Calibration

Battery Status

Cheatcodes

Input Cheatcode

View Saved Cheatcodes

System Info

Console Firmware Version

Connected Drone UID

HC12 Channel/Address

4. Navigation Inputs
Input Device	Function
Keypad	Direct key inputs, cheatcode entry, quick menu jumps
Digital Volume Encoder	Scroll up/down, increment/decrement values, click to select
Encoder Click	Select highlighted menu item or confirm prompt
Long Press Encoder	Return to home screen immediately

5. Screensaver
The screensaver is always active when the console is idle, displaying:

Battery voltage and percentage

GPS Lat, Long, Alt

Drone UID

Status bar (connection status, alerts)

Screensaver Exit
Any keypad press or encoder click exits the screensaver and returns to the last active menu.

6. Menu Item Types
Action Items

Execute immediate commands (e.g. Takeoff, Land).

Value Items

Show current setting with ability to adjust (e.g. Cruise Altitude).

Toggle Items

Enable or disable a feature (e.g. Backlight ON/OFF).

Input Items

Accept text/numeric input (e.g. cheatcodes, access codes).

7. Implementation Notes (For Developers)
The menu is implemented as a linked list or nested switch-case structure for scalability.

The digital encoder input is polled for rotation and click events to update the highlighted menu item.

The keypad matrix input is scanned for direct input handling and shortcut triggers.

LCD updates are buffered to minimize flicker during rapid scrolling.

The menu loop should run non-blocking to maintain real-time communication with the drone over HC12.

8. Future Expansion
Graphical UI: Replace basic text-based display with graphical icons.

Dynamic Menus: Load menu options based on connected drone capabilities (e.g. only show camera controls if drone has a camera module).

User Profiles: Save personalized settings and cheatcode lists per user login.

9. Testing Checklist
✅ All menu items display correctly
✅ Encoder scrolls through items smoothly
✅ Encoder click selects items and enters submenus
✅ Long press returns to home screen
✅ Keypad inputs map correctly to functions and cheatcode entries
✅ LCD updates are flicker-free

