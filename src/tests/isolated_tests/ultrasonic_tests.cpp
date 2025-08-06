//https://www.youtube.com/watch?v=xwsT-e1D9OY
#include "ultrasonic.h"

const int trigPin = 9;
const int echoPin = 10;

HCSR04 ultrasonic(trigPin, echoPin);

void setup() {
  Serial.begin(9600);
  ultrasonic.begin();
}

void loop() {
  float distanceCm = ultrasonic.measureDistanceCm();
  float distanceInch = ultrasonic.measureDistanceInch();
  
  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.print(" cm, ");
  Serial.print(distanceInch);
  Serial.println(" inches");
  
  delay(1000);
}