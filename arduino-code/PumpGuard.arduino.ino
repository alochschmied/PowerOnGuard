#include "Controls.h"

const int meterEveryMillis = 2000;
const int readPotisEveryMillis = 1000;

/**
 * Potentiometer setting: 0 ... 10 A
 * The minimum amperes that need to drawn such that the algorithm starts to count the time since when this started.
 */
double ampsThreshold;

/**
 * Potentiometer setting: 0 ... 1h
 * The maximum time in seconds that amps may be drawn. Afterwards the state is toggled and power is switched off.
 * The red LED will start to blink. To get power again, the reset switch must be pressed. It will toggle the state.
 */
int maxDurationSeconds;

/**
 * Potentiometer setting: 0 ... 300s
 * Some machines like pumps draw amps for some time and then stop to draw amps, but they are still "on", e.g. pump water.
 * This propery is to "virtually" extend the duration where amps are actually drawn to allow the algorithm to accound for such machines.
 */
int durationExtensionSeconds;

/**
 * The actual amps drawn currently.
 */
double i_rms;

Controls controls = Controls();

EnergyMonitor emon;

enum State {
  RUNNING,
  SHUTDOWN
};

volatile enum State state = RUNNING; // We start with power turned on
unsigned long lastToggleTime;        // Last state change

boolean virtualAmpsOn = false;
unsigned long ampsOnSinceSeconds = 0;
unsigned long ampsOffSinceSeconds = 0;
int remainingSeconds = 0;

void setup() {
  Serial.begin(9600);

  pinMode(PIN_OUT_SSR, OUTPUT);

  controls.setup(onReset);

  double scale = 18.00360 * 4 * (4.06/4.55);
  emon.current(7, scale);             // Current: input pin, calibration.

}

void onReset() {
  unsigned long now = millis();

  if ((now - lastToggleTime) > 150) {
    lastToggleTime = now;
    if (state == SHUTDOWN) {
      state = RUNNING;
      virtualAmpsOn = false;
      ampsOnSinceSeconds = 0;
      ampsOffSinceSeconds = 0;
      remainingSeconds = 0;
    } else {
      state = SHUTDOWN;
    }
  }
}



void loop() {

  unsigned long tMS = millis();
  unsigned long tS = tMS / 1000;

  // Somtimes read potis
  if( (tMS % readPotisEveryMillis) == 0 ) {
    controls.readPotis();
  
    durationExtensionSeconds = controls.getDurationExtensionSeconds();
    ampsThreshold = controls.getAmpsThreshold();
    maxDurationSeconds = controls.getMaxDurationSeconds();
  
    Serial.print("ampsThreshold,durationExtensionSeconds,maxDurationSeconds=(");
    Serial.print(ampsThreshold);
    Serial.print(",");
    Serial.print(durationExtensionSeconds);
    Serial.print(",");
    Serial.print(maxDurationSeconds);
    Serial.print(")");
    Serial.println("");
  }

  // Read current amps
  if( (tMS % (meterEveryMillis)) == 0 ) {
    i_rms = emon.calcIrms(1480);  // Calculate Irms only
    //double i_rms = emon.calcIrms(4000);  // Calculate Irms only
  }

  /**
   * Logic to turn off SSR after power is consumed for too long
   */
  if (powerIsConsumed()) {
    controls.powerLedOn(true);
    if (virtualAmpsOn == false) {
      Serial.println("############");
      Serial.println(tS);
      
      virtualAmpsOn = true;
      ampsOnSinceSeconds = tS;
    }
  } else {
    controls.powerLedOn(false);
    if (virtualAmpsOn == true) {
      ampsOffSinceSeconds = tS;
    }
        
    // Case 1: Amps "just" went off, so let's pretend they are still on for durationExtensionSeconds
    // We just use ampsOnSinceSeconds and virtualAmpsOn to compare with maxDurationSeconds to decide if SSR must be switched off
    if (virtualAmpsOn == true) { // amps must have been on at least ONCE
      if ((ampsOffSinceSeconds + durationExtensionSeconds) <= (tS + maxDurationSeconds)) {
        virtualAmpsOn = true;
        //Serial.println("!!!!!!");
      } else {
        //virtualAmpsOn = false;
      }
    }
  }

  remainingSeconds = maxDurationSeconds + durationExtensionSeconds;
  if (virtualAmpsOn == true) {
    remainingSeconds = remainingSeconds - (tS - ampsOnSinceSeconds);
  }

  //if (virtualAmpsOn == true && (ampsOnSinceSeconds - tS) > maxDurationSeconds) { // no more power consumption allowed
  if (remainingSeconds <= 0) {
    state = SHUTDOWN;
    virtualAmpsOn = false;
    ampsOffSinceSeconds = tS;
  }

  controls.update();

  if (state == SHUTDOWN) {
    controls.signalShutdown(true);
    //Serial.println("SHUTDOWN");
    digitalWrite(PIN_OUT_SSR, LOW);
  } else {
    controls.signalShutdown(false);
    digitalWrite(PIN_OUT_SSR, HIGH);
  }

  if( (tMS % (meterEveryMillis)) == 0 ) {
    Serial.print("Amps=");
    Serial.print(i_rms);
    Serial.print(", ");
    Serial.print("powerIsConsumed=");
    Serial.print(powerIsConsumed());
    Serial.print(", ");
    Serial.print("state=");
    Serial.print(state);
    Serial.print(", ");
    Serial.print("remainingSeconds=");
    Serial.print(remainingSeconds);
    Serial.print(", ");
    Serial.print("tS=");
    Serial.print(tS);
    Serial.print(", ");
    Serial.print("ampsOnSinceSeconds=");
    Serial.print(ampsOnSinceSeconds);
    Serial.print(", ");
    Serial.print("virtualAmpsOn=");
    Serial.print(virtualAmpsOn);

    
    Serial.println("");
  }

}

boolean powerIsConsumed() {
  boolean ret = i_rms > ampsThreshold;
  return ret;
}




void testVoltGeneration() {


//  pinMode(9, OUTPUT); // pwm out
//  pinMode(6, INPUT); // read generated volts by poti
//  pinMode(6, INPUT); // read amps
/*
  int durationExtensionSeconds = controls.getDurationExtensionSeconds();
  int durationExtensionPWM = map(durationExtensionSeconds, 0, 300, 0, 255);
  analogWrite(9, durationExtensionPWM);
  int voltsRead = analogRead(6);
*/
/*
  Serial.print("Volts=");
  Serial.print(voltsRead);
  Serial.print(", ");
  Serial.println("");
*/
}

