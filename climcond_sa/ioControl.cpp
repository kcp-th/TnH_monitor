/*
 * Object to manage output (Relay)
 * Function to turn on/off, check current status
 * One for evap, one for fogger
 * Extend is object for other control ex; blower (air flow), heater, water cooler,  etc
 */
#include "ioControl.h"

ioControl::ioControl(int hwPin) {
  pin = hwPin ;
  pinMode(pin, OUTPUT);
  off();                // turn off at startup
}

void ioControl::on() {
  digitalWrite(pin, HIGH);  //turn off the relay
  status = IO_ON ;   
}
void ioControl::off() {
  digitalWrite(pin, LOW); //turn on the relay
  status = IO_OFF ; 
}
bool ioControl::isOn() {
  return status ;
}
bool ioControl::isOff() {
  return (!status) ;
}


