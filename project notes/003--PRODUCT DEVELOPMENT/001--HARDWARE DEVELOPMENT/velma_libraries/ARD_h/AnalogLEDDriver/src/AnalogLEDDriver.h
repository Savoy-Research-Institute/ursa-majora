// AnalogLedDriver - a Library for non-blocking led actions.
// Created by dcoded.nl / Arno Fleming, Jan 2023
// MIT License

#include "Arduino.h"

#ifndef AnalogLEDDriver_h
#define AnalogLEDDriver_h

enum LedState {
  LED_OFF = 0,
  LED_ON = 1,
  LED_BLINK = 2,
  LED_GLOW = 3,
  LED_SINGLE_PULSE = 4
};

class LedDriver {
public:
  LedDriver(int pin);
  void switchTo(LedState action);
  void maintain();
private:
  int _pin;
  unsigned long _pulseOffTime;
  LedState _action;

  // Led timings are all in milliseconds
  const uint8_t  _pulse_duration        = 8;
  const uint8_t  _blink_on_duration     = 128;
  const uint16_t _blink_interval        = 1024;
  const uint16_t _glow_interval         = 4096;
  const uint16_t _glow_crescendo_done   = 1024;
  const uint16_t _glow_diminuendo_done  = 2048;
};
#endif
