#ifndef _IOCONTROL_H_
#define _IOCONTROL_H_

#include <Arduino.h>
#define IO_OFF  false
#define IO_ON   true
  
class ioControl   
{
public:
    ioControl(int);
    void on();
    void off();
    bool isOn();
    bool isOff();
    bool status;   
private:
    unsigned char pin;
};
#endif
