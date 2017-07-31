#ifndef Poti_h
#define Poti_h

#include "Globals.h"
 
class Poti {
  private:
    int val;
    byte PIN;

  public:
    Poti();
    ~Poti();
    void setup(byte pin);
    void update();

    void read();
    int get();
};
 
#endif
