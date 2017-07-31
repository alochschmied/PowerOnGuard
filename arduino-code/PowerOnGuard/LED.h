#ifndef LED_h
#define LED_h

#include "Globals.h"
 
class LED {
  private:
    unsigned long lastBlinkToggleTime;
    int blinkTime = 200;
    boolean blinking = false;
    byte PIN;

  public:
    LED();
    ~LED();
    void setup(byte pin);
    void update();

    void on();
    void off();
    boolean isOn();
    void blink(boolean enable);


};
 
#endif
