//https://www.youtube.com/watch?v=xwsT-e1D9OY
#include "../../modules/behavior_hiding/shared_services/sensors_coordination_module.h"

const int trigPin = 9;
const int echoPin = 10;

SensorsCoordinationModule sensorsModule;

void setup() {
  Serial.begin(9600);
  sensorsModule.initialize();
  sensorsModule.addSensor(SensorType::ULTRASONIC, 0, trigPin);
}

void loop() {
  sensorsModule.readSensor(0); // Read ultrasonic sensor
  float distanceCm = sensorsModule.getUltrasonicDistance();
  float distanceInch = distanceCm * 0.393701; // Convert cm to inches
  
  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.print(" cm, ");
  Serial.print(distanceInch);
  Serial.println(" inches");
  
  delay(1000);
}