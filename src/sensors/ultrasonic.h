#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

class HCSR04 {
  public:
    HCSR04(int trigPin, int echoPin);
    void begin();
    void switch_ultrasonic(bool var);
    float measureDistanceCm();
    float measureDistanceInch();
    
  private:
    int _trigPin;
    int _echoPin;
    long _duration;
    float _measureDistance();
};

#endif