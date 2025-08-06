#include <AnalogLEDDriver.h>;

// MKR 1010 has analog (PWM) ports on at least pin 0-7
// PWM outputs are the params to the LedDriver
LedDriver redLed(0);
LedDriver yellowLed(1);
LedDriver greenLed(2);

void setup() {
  redLed.switchTo(LED_OFF);
  yellowLed.switchTo(LED_GLOW);
  greenLed.switchTo(LED_BLINK);
}

void loop() {
  if(millis()% 1000 == random(2000)) { redLed.switchTo(LED_SINGLE_PULSE); }

  redLed.maintain();
  yellowLed.maintain();
  greenLed.maintain();
}
