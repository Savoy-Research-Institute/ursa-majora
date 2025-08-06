//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Error prevention, detection, solution^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Function to setup failsafe mechanisms
void setupFailsafe()
{
    configureFailsafeParameters();
}
// Function to check for failsafe conditions
void checkFailsafeConditions(imuData, gpsData)
{ // checks IC registers, on change, there is an error detection thing that gets turned up...
    if imuData
        is invalid or gpsData is invalid : triggerFailsafe() return True return False
}
// Function to trigger failsafe
void triggerFailsafe()
{
    landDrone();
    Serial.print("Failsafe triggered");
}
void fail_safe()
{ // error detection, parameters being the intended failure
  // gps failure, if serial, not available, failsafes(gps),
}
