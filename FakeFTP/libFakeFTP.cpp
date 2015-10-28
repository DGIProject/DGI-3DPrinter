#include "Arduino.h"
#include "libFakeFTP.h"

FakeFTP::FakeFTP(int pin)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
}
