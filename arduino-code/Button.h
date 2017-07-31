#ifndef Button_h
#define Button_h
 
#include "Globals.h"
#include <Bounce2.h>

class Button {
  private:
    byte PIN;
    buttonOnHigh onHigh;

    int lastButtonState = LOW;   // the previous reading from the input pin
    unsigned long lastChangeTime;  // the last time the output pin was toggled
    unsigned long debounceDelay = 30;    // the debounce time; increase if the output flickers
    boolean readingWasLowLongEnough = false;

  public:
    Button();
    ~Button();
    void setup(byte pin, void (*_onHigh)(void));
    //void setup(byte pin);
    void update();

    boolean isOn();
};
 
#endif
