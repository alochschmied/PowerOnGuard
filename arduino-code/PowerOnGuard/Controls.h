#ifndef Controls_h
#define Controls_h

#include "Globals.h"

class Controls {
  private:

  public:
    Controls();
    ~Controls();
    void setup(buttonOnHigh onReset);
    void update();
    
    void signalShutdown(boolean shutdown);
    boolean isResetPressed();
    void resetLedOn(boolean on);
    void powerLedOn(boolean on);
    void powerLedBlink(boolean blink);
    void readPotis();

    int getDurationExtensionSeconds();
    double getAmpsThreshold();
    int getMaxDurationSeconds();

};

#endif
