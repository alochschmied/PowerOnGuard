#include "LED.h"

LED::LED() {
}

LED::~LED() {
}

void LED::setup(byte pin) {
  PIN = pin;
  pinMode(pin, OUTPUT);
  lastBlinkToggleTime = millis();
}

void LED::update() {
  if (blinking == true) {
    unsigned long now = millis();
    if ((now - lastBlinkToggleTime) > blinkTime/2) {
      if(this->isOn()) {
        off();
      } else {
        on();
      }
      lastBlinkToggleTime = now;
    }    
  }
}

void LED::on() {
  blinking = false;
  digitalWrite(PIN, HIGH);
}

void LED::off() {
  blinking = false;
  digitalWrite(PIN, LOW);
}

boolean LED::isOn() {
  return digitalRead(PIN) == HIGH;
}

void LED::blink(boolean enable) {
  blinking = true;
  if (enable == false) {
    off();
  }
}
  

