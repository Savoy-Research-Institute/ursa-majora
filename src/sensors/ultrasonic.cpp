//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

//BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
//~~~~~~~~~~~~~~~~~~~~~~~Ultrasonic functions~~~~~~~~~~~~~~~~~~~~~~~
#include "ultrasonic.h"

HCSR04::HCSR04(int trigPin, int echoPin) {
  _trigPin = trigPin;
  _echoPin = echoPin;
}

void HCSR04::begin() {
  pinMode(_trigPin, OUTPUT);
  pinMode(_echoPin, INPUT);
}

float HCSR04::measureDistanceCm() {
  return _measureDistance() * 0.034 / 2;
}

float HCSR04::measureDistanceInch() {
  return _measureDistance() * 0.0133 / 2;
}

float HCSR04::_measureDistance() {
  // Clear the trigPin
  digitalWrite(_trigPin, LOW);
  delayMicroseconds(2);
  
  // Set the trigPin high for 10 microseconds
  digitalWrite(_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigPin, LOW);
  
  // Read the echoPin
  _duration = pulseIn(_echoPin, HIGH);
  
  return _duration;
}

void HCSR04::switch_ultrasonic(bool var)
{
    if (var == 1)
    {
        // Turn the transistor on
        digitalWrite(ultrasonic_t_PIN, HIGH);
    }
    else if (var == 0)
    {
        // Function to switch the module off
        // Turn the transistor off
        digitalWrite(ultrasonic_t_PIN, LOW);
    }
}

