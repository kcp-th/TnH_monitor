#ifndef _BUTTON_IO_H_
#define _BUTTON_IO_H_

#include <Arduino.h>  
#define TM_DEBO_DELAY 50      // debounce delay time 50ms 
#define TM_HOLD_DELAY 1000    // hold, longer than 1 sec  

#define BUTTON_PRESS    LOW  
#define BUTTON_RELEASE  HIGH 
#define BUTTON_SHORT    's' 
#define BUTTON_LONG     'l' 

class buttonInput   
{
public:
    buttonInput(unsigned char pin);
    bool isPress();
    unsigned char waitRelease();
private:
    unsigned char pin, state;
    long  tmChg;
    unsigned char debounce();
};
#endif
