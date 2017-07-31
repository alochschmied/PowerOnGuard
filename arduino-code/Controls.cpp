#include "Controls.h"
#include "LED.h"
#include "Button.h"
#include "Poti.h"

LED redLED;
LED greenLED;
Button resetButton;

Poti durationExtensionPoti;
Poti ampsThresholdPoti;
Poti maxDurationPoti;

Controls::Controls() {
}

Controls::~Controls() {
}

void Controls::setup(buttonOnHigh onReset) {
  durationExtensionPoti.setup(PIN_IN_POTI_EXT);
  ampsThresholdPoti.setup(PIN_IN_POTI_AMPS);
  maxDurationPoti.setup(PIN_IN_POTI_DUR);
  
  redLED.setup(PIN_OUT_LED_RED);
  greenLED.setup(PIN_OUT_LED_GREEN);
  resetButton.setup(PIN_IN_RESET_BUTTON, onReset);
}

void Controls::update() {
  redLED.update();
  resetButton.update();
}

void Controls::signalShutdown(boolean shutdown) {
  redLED.blink(shutdown);
}

boolean Controls::isResetPressed() {
  return resetButton.isOn();
}

void Controls::resetLedOn(boolean on) {
  if (on) {
    redLED.on();
  } else {
    redLED.off();
  }
}

void Controls::powerLedOn(boolean on) {
  if (on) {
    greenLED.on();
  } else {
    greenLED.off();
  }
}

void Controls::readPotis() {
  durationExtensionPoti.read();
  ampsThresholdPoti.read();
  maxDurationPoti.read();
}

double mapDouble(long x, long in_min, long in_max, double out_min, double out_max)
{
  return ((double)x - (double)in_min) * (out_max - out_min) / ((double)in_max - (double)in_min) + out_min;
}

int Controls::getDurationExtensionSeconds() {
  const int minOut = 0;
  const int maxOut = 300;
  // map(value, fromLow, fromHigh, toLow, toHigh)
  int v = durationExtensionPoti.get();
  v = map(v, 0, 1023, minOut, maxOut);
  v = constrain(v, minOut, maxOut);

  return v;
}

double Controls::getAmpsThreshold() {
  const double minOut = 0.0;
  const double maxOut = 30.0;
  // map(value, fromLow, fromHigh, toLow, toHigh)
  int v = ampsThresholdPoti.get();
  double ret = mapDouble(v, 0, 1023, minOut, maxOut);
  //v = constrainDouble(v, minOut, maxOut);

  return ret;
}

int Controls::getMaxDurationSeconds() {
  const int minOut = 0;
  const int maxOut = 60*60; // 1h
  // map(value, fromLow, fromHigh, toLow, toHigh)
  int v = maxDurationPoti.get();
  v = map(v, 0, 1023, minOut, maxOut);
  v = constrain(v, minOut, maxOut);

  return v;
}


