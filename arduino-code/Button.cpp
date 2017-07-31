#include "Button.h"

Button::Button() {
}

Button::~Button() {
}

void Button::setup(byte pin, buttonOnHigh _onHigh) {
  PIN = pin;
  onHigh = _onHigh;
  pinMode(pin, INPUT_PULLUP);
  lastChangeTime = millis();
}

void Button::update() {
  int reading = digitalRead(PIN);
  
  if (reading != lastButtonState) {
    lastChangeTime = millis();
  }

  if ((millis() - lastChangeTime) > debounceDelay) {
    if (reading == LOW) {
      readingWasLowLongEnough = true;
    } else {
      if (readingWasLowLongEnough == true && reading == HIGH) { // Button was just released after behing held long enough
        onHigh();
        readingWasLowLongEnough = false;
      }
    }
  }

  lastButtonState = reading;
}

boolean Button::isOn() {
  int val = digitalRead(PIN);
  return (val == HIGH);
}

  

