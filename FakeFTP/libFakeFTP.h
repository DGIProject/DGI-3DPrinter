#ifndef FakeFTP_h
#define FakeFTP_h

#include "Arduino.h"

class FakeFTP
{
  public:
    Morse(int pin);
    void dot();
    void dash();
  private:
    int _pin;
};

#endif
