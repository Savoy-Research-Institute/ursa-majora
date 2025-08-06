#include "flight_controller.h"

pid_controller drone;

void loop() {
    drone.calculate_pid_setpoint();
    drone.calculate_pid();
    
    float roll_output = drone.getRollOutput();
    float pitch_output = drone.getPitchOutput();
    float yaw_output = drone.getYawOutput();
    
    // Use these outputs to control your drone's motors
}