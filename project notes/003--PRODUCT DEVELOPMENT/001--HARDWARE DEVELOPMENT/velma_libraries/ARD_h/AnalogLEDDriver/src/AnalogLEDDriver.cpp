// AnalogLedDriver - a Library for non-blocking led actions.
// Created by dcoded.nl / Arno Fleming, Jan 2023
// MIT License

#include "Arduino.h"
#include "AnalogLEDDriver.h"

LedDriver::LedDriver(int pin) {
  _pin = pin;
  switchTo(LED_OFF);
}

void LedDriver::switchTo(LedState action){
  pinMode(_pin, OUTPUT);
  _action = action;

  if (action == LED_SINGLE_PULSE){
    // millis() is an unsigned long. the calculation
    // `millis() + _pulse_duration`_may_ overflow.
    // In that case, the result will hold a very low value, meaning we will
    // miss displaying a pulse. Given the pulse is for knowing that everything
    // works, we'd rather miss one pulse (scenario above) than 'fix this' and
    // possibly maybe keep the light on for too long. Specifically, worst case
    // scenario: next overflow in 50 days ¯\_(ツ)_/¯, see
    // https://reference.arduino.cc/reference/en/language/functions/time/millis/
    _pulseOffTime = millis() + _pulse_duration;
  }
}

void LedDriver::maintain(){
  const unsigned long milli = millis();
  const uint16_t glow_point = milli % _glow_interval;

  switch (_action) {
    case LED_ON:
      analogWrite(_pin, 255);
      break;
    case LED_BLINK:
      if (milli % _blink_interval < _blink_on_duration) {
        analogWrite(_pin, 255);
      } else {
        analogWrite(_pin, 0);
      }
      break;
    case LED_GLOW:
      if (glow_point < _glow_crescendo_done) {
        // We're in the 'crescendo' phase
        analogWrite(_pin, glow_point >> 2 );
      } else if (glow_point < _glow_diminuendo_done) {
        // We're in the 'diminuendo' phase
        analogWrite(_pin, (_glow_diminuendo_done - glow_point) >> 2 );
      } else {
        // We're in the off phase
        analogWrite(_pin, 0);
      }
      break;
    case LED_SINGLE_PULSE:
      analogWrite(_pin, 50);

      if(milli >= _pulseOffTime){
        _action = LED_OFF;
      }
      break;
    default:
      // LED_OFF is the default
      analogWrite(_pin, 0);
      break;
  }
}
