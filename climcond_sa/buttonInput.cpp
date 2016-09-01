/*
 * Object to manage input button
 * Function to check current status, software debounce
 * One for each button
 */
#include "buttonInput.h"

buttonInput::buttonInput(unsigned char hwPin) {
  pin = hwPin ;
  pinMode(pin, INPUT);
}

bool buttonInput::isPress() {
  if ((digitalRead(pin)==BUTTON_PRESS) &&
      ((state = debounce())==BUTTON_PRESS)) {
       tmChg = millis();  // stamp time of last state change
       return true;      
   }
   else   // button is not pressed
      return false;
}

unsigned char buttonInput::waitRelease() {
  long tmEnd, tmDiff ;
  while ((digitalRead(pin)==BUTTON_PRESS)||
         (debounce()==BUTTON_PRESS)) {
    // wait until button is released
  }
  tmEnd = millis() ;
  tmDiff = tmEnd - tmChg ; 
  tmChg = tmEnd ; 
  if (tmDiff > TM_HOLD_DELAY)
    return BUTTON_LONG ; 
  else
    return BUTTON_SHORT;
}

// software debounce  
unsigned char buttonInput::debounce() { 
  long tmCur, tmPrv;            // time (milliseconds)
  unsigned char stCur,stPrv;    // state, signal read from input pin
  stCur = digitalRead(pin);
  stPrv = stCur;
  tmPrv = millis();
  while(((tmCur=millis())-tmPrv) < TM_DEBO_DELAY) { 
      // if signal not stable, keep reading and stay in loop
      if ((stCur = digitalRead(pin)) != stPrv) {
        tmPrv = tmCur;
        stPrv = stCur;
      }
  } // signal stable, exit loop
  return stCur ;  // return current reading
}

/*
  unsigned char buttonInput::buttonChk() {
  unsigned bt;
  long tmBeg, tmEnd ;
  if ((bt = digitalRead(pin))==BUTTON_RELEASE) 
    return bt;
  else {  // button is pressed
     bt=debounce();
     tmBeg = millis();
  }
  while((bt = digitalRead(pin))==BUTTON_PRESS) {
    // wait until button is released
  }
  bt=debounce();
  tmEnd = millis();
  if ((tmEnd-tmBeg) > TM_HOLD_DELAY)
    return BUTTON_LONG ; 
  else
    return BUTTON_SHORT;
}
*/
