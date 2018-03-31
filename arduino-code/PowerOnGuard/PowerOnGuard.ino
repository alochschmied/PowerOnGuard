#include "Controls.h"

const int meterEveryMillis = 2000;
const int readPotisEveryMillis = 1000;

/**
   Potentiometer setting: 0 ... 300s
   Some machines like pumps draw amps for some time and then stop to draw amps, but they are still "on", e.g. pump water.
   This propery is to "virtually" extend the duration where amps are actually drawn to allow the algorithm to accound
   for such machines.
*/
int durationExtensionSeconds;

/**
   Potentiometer setting: 0 ... 10 A
   The minimum amperes that need to drawn such that the algorithm starts to count the time since when this started.
*/
double ampsThreshold;

/**
   Potentiometer setting: 0 ... 1h
   The maximum time in seconds that amps may be drawn. Afterwards the state is toggled and power is switched off.
   The red LED will start to blink. To get power again, the reset switch must be pressed. It will toggle the state.
*/
int maxDurationSeconds;


/**
   The actual amps drawn currently.
*/
double i_rms;

unsigned long i;

Controls controls = Controls();

EnergyMonitor emon;

enum State {
  RUNNING,
  SHUTDOWN
};

unsigned long lastToggleTime;        // Last state change

volatile enum State state = RUNNING; // We start with power turned on

unsigned long tS;
boolean wasPowerConsumed = false;
unsigned long ampsOnSinceSeconds = 0;
unsigned long virtualPowerConsumptionOnUntilS = 0;

void setup() {
  Serial.begin(9600);

  pinMode(PIN_OUT_SSR, OUTPUT);

  controls.setup(onResetButton);

  double scale = 18.00360 * 4 * (4.06 / 4.55);
  emon.current(7, scale);             // Current: input pin, calibration.

}

void onResetButton() {
  unsigned long now = millis();

  if ((now - lastToggleTime) > 150) {
    lastToggleTime = now;

    if (state == SHUTDOWN) {
      state = RUNNING;
      wasPowerConsumed = false;
      ampsOnSinceSeconds = 0;
      virtualPowerConsumptionOnUntilS = 0;
    } else {
      state = SHUTDOWN;
    }
  }
}



void loop() {
  i++;

  unsigned long tMS = millis();
  tS = tMS / 1000;

  // Somtimes read potis
  if ( (tMS % readPotisEveryMillis) == 0 ) {
    controls.readPotis();

    durationExtensionSeconds = controls.getDurationExtensionSeconds();
    ampsThreshold = controls.getAmpsThreshold();
    maxDurationSeconds = controls.getMaxDurationSeconds();
  }

  // Read current amps
  if ( (tMS % (meterEveryMillis)) == 0 ) {
    i_rms = emon.calcIrms(1480);  // Calculate Irms only
    Serial.println(i_rms);
  }

  if (isPowerConsumed()) {
    controls.powerLedOn(true);

    if (wasPowerConsumed == false) {
      Serial.println("Actual power consumption just started ");
      Serial.println(tS);

      if (virtualPowerConsumptionOnUntilS == 0) {
        ampsOnSinceSeconds = tS;
      }
    } else {
      if (tooLong()) {
        // Power consumed for too long
        state = SHUTDOWN;
      }
    }
  } else {
    controls.powerLedOn(false);
    if (wasPowerConsumed == true) {
      Serial.println("Actual power consumption just stopped ");
      Serial.println(tS);
      // We need assume power consumption for additional durationExtensionSeconds
      virtualPowerConsumptionOnUntilS = tS + durationExtensionSeconds;
    } else {
    }
  }

  if (virtualPowerConsumptionOnUntilS > 0) {
    /*
       Virtual mode has priority. No matter if actual power was off we need to wait until
       time is virtualPowerConsumptionOnUntilS.
       If we waited so long and power is (still) consumed we'll shut down
       power as it was used for too long.

       HOWEVER IF ACTUAL POWER IS CONSUMED WE NEED TO EXTAND AAGAIN
    */
    if (tS > virtualPowerConsumptionOnUntilS) {
      if (isPowerConsumed() && tooLong()) {
        state = SHUTDOWN;
      }
      virtualPowerConsumptionOnUntilS = 0;
    }

    // Just for Power LED:
    if (isPowerConsumed()) {
      controls.powerLedOn(true);
    } else {
      controls.powerLedBlink(true);
    }

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

  if ( (tMS % (meterEveryMillis)) == 0 ) {
    Serial.print(tS);
    Serial.print(", ");
    Serial.print(i_rms);
    Serial.print(", ");
    Serial.print(wasPowerConsumed);
    Serial.print(", ");
    Serial.print(isPowerConsumed());
    Serial.print(", ");
    Serial.print(state);
    Serial.print(", ");
    Serial.print(durationExtensionSeconds);
    Serial.print(", ");
    Serial.print(ampsThreshold);
    Serial.print(", ");
    Serial.print(maxDurationSeconds);
    Serial.print(", ");
    Serial.print(ampsOnSinceSeconds);
    Serial.print(", ");
    Serial.print(virtualPowerConsumptionOnUntilS);

    Serial.print(" (");
    Serial.print("tS,");
    Serial.print("i_rms,");
    Serial.print("wasPowerConsumed,");
    Serial.print("isPowerConsumed,");
    Serial.print("state,");
    Serial.print("durationExtensionSeconds,");
    Serial.print("ampsThreshold,");
    Serial.print("maxDurationSeconds,");
    Serial.print("ampsOnSinceSeconds,");
    Serial.print("virtualPowerConsumptionOnUntilS,");
    Serial.print(")");
    Serial.println("");
  }

  wasPowerConsumed = isPowerConsumed();
}

boolean isPowerConsumed() {
  boolean ret = i_rms > ampsThreshold;
  return ret;
}

boolean tooLong() {
  return (tS - ampsOnSinceSeconds) > maxDurationSeconds;
}

