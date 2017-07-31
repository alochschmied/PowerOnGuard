#include "Poti.h"

Poti::Poti() {
}

Poti::~Poti() {
}

void Poti::setup(byte pin) {
  PIN = pin;
  pinMode(pin, INPUT);
  read();
}

void Poti::update() {
}

void Poti::read() {
  val = analogRead(PIN);
}

int Poti::get() {
  return val;
}

