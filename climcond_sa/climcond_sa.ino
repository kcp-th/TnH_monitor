/* 
 * Climate Conditioner 
 * The idea was originated by Prof Teerawat at Srinakarinwirote University
 * To use microcontroller to monitor temperature and humidity 
 * and then control evaporator (to reduce temperature) and fogger (to increase humidity) to 
 * the appropriate level for the mushroom. 
 * This idea may be applied to other kind plant by adding another type of sensor and equipment
 * for example:
   - measure CO2 and control air flow in close environment
   - measure ambient light and turn on/off eletrical light or shade in green house

 * This is the first prototype: single microcontroller in stand alone environment
 * HW  
  Microcontroller :          Arduino Nano (compatible)
  Temp and Humidity sensor : DHT22 (AM2302)
  Output :                   2 channels relay 220V 10A 
  User interface:            16x2 LCD with 3 button switch
 
 * Created 2016-06-30
 * By Kriangsak Chanpiwat (kcp)
 * Repository : github 
 * Last modified : 2016-07-05 (kcp)
 */
 
// Hardware connection, 
// Arduino Nano (compatible) in this prototype
#define DHT22_PIN  5   // Port to communicate to Temp&Humdity sensor
#define RLTEMP_PIN 8   // Relay port that control evaporator
#define RLHUMD_PIN 9   // Relay port that control fog generator
#define BTUP_PIN   2   // Up button
#define BTST_PIN   3   // Set button
#define BTDN_PIN   4   // Down button
#define LED_PIN   13   // LED

// Watchdog to automatic reset in case of unexpected hang
#include <avr/wdt.h>                          // Watchdog Timer
#define ENABLE_WATCHDOG   wdt_enable(WDTO_4S) // longest error display is 3 sec
#define DISABLE_WATCHDOG  wdt_disable()
#define SIGNAL_WATCHDOG   wdt_reset()

#include <dht.h>
dht DHT;
#define tCur  DHT.temperature
#define hCur  DHT.humidity

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// On board LED, now used only during initialize 
#include <LED.h>
LED led(LED_PIN);
#define BLINK   led.blink(400,1)   // blink time 400 ms, once

// Module (Object) used in this program
#include "measurement.h"
#include "ioControl.h"
#include "parameter.h"
#include "buttonInput.h"

// Statistics data, for the past temp and humidity
measurement tStat, hStat;

parameter param;

// Relay control 
ioControl  relayTemp(RLTEMP_PIN);
ioControl  relayHumd(RLHUMD_PIN);

// Timing
long tmPrvSpl=1000000 ;   // prev sampling time, force 1st reading 
long tmPrvBt=0 ;          // time of prev button interact

// Button 
buttonInput btUP(BTUP_PIN);
buttonInput btST(BTST_PIN);
buttonInput btDN(BTDN_PIN);

#define NOBT_PRESS    0
#define BTUP_PRESS    1
#define BTDN_PRESS    2
#define BTST_PRESS    3
#define BTST_HOLD     4
#define BTWAIT_TMOUT  5


// ================== User Interface  ================= 
#define MODE_CURRENT    0
#define MODE_HISTORY    1
#define MODE_THRESH     2
#define MODE_TIMING     3
#define MAX_MODE        4
#define DISP_INF_DELAY   1000    // delay 1 sec after display information message
#define DISP_ERR_DELAY   3000    // delay 3 sec after display error message
#define BTWAIT_DELAY    30000    // 30 sec

// Function Declaration
void lcdInit();
void sensorRead();

void chkThres(long t, long h) ;
unsigned char UImain();

void manualCtrl();
void histClear();
void thresSet();
void timingSet();
void dispCurrent();
void dispHistory();
void dispThreshold();
void dispTiming();
void dispMsg(char *msg1,char *msg2, int delayms);

// ================== LCD : main display ================= 
// initialize the LCD
void lcdInit()
{
  lcd.begin();
  dispMsg("Startup", "Temp&Humd ctrl", DISP_INF_DELAY);
}

// ================== Sensor : read temp and humd ================= 

void serialInit() {   // For debug
  Serial.begin(115200);
  Serial.print("DHT LIBRARY VERSION: ");
  Serial.println(DHT_LIB_VERSION);
  Serial.println();
}


// read data from DHT22 sensor
void sensorRead() {      
    int rc = DHT.read22(DHT22_PIN);
    if (rc != DHTLIB_OK) {  // error occur, write err msg somewhere
      dispMsg("Sensor error","Check DHT22 ",DISP_ERR_DELAY);
//      Serial.print("DHT error code ");
//      Serial.print(rc,1);
    }
// DEBUG: Serial.print(" t : "); Serial.print(tCur); Serial.print(" h : "); Serial.print(hCur); Serial.print("\n");
}

// ================== LOGIC to trigger relay ================= 
// This is for mushroom, modify the logic for other type of plant and environment
// assumption : outside temperature is higher than the inside
//              outside humidity is lower than the inside
// Logic : reduce temperature using evaporator, turn on when temperature rise above upper threshold
//                                              turn off when temperature drop to lower threshold
//         increase humidity using fogger, turn on when humidity fall below lower threshold
//                                         turn off when humidity reach upper threshold
void chkThres(long t, long h) {
    //    take action if temp or humd fall beyond threshold 
    if (relayTemp.isOff() && (tCur > param.tHi))  // temp reach upper threshold, turn on evap
      relayTemp.on(); 
    if (relayTemp.isOn()  && (tCur < param.tLo))  // temp drop below lower threshold, turn off evap 
      relayTemp.off() ; 
    if (relayHumd.isOff() && (hCur < param.hLo))  // humd drop below lower threshold,turn on fogger
      relayHumd.on(); 
    if (relayHumd.isOn()  && (hCur > param.hHi))  // humd reach upper threshold, turn off fogger 
      relayHumd.off() ; 
}


// ================== User Interface  ================= 

char mode ; 
struct modeFunc {
  void (* display)() ;
  void (* setting)() ;
};
modeFunc modeTab[MAX_MODE];

// mode 0=current, 1=history, 2=threshold, 3=timing
void UIInit() {
  modeTab[MODE_CURRENT].display = dispCurrent;   modeTab[MODE_CURRENT].setting = manualCtrl;
  modeTab[MODE_HISTORY].display = dispHistory;   modeTab[MODE_HISTORY].setting = histClear;
  modeTab[MODE_THRESH].display  = dispThreshold; modeTab[MODE_THRESH].setting  = thresSet;
  modeTab[MODE_TIMING].display  = dispTiming;    modeTab[MODE_TIMING].setting  = timingSet;
  mode = MODE_CURRENT ;
  modeTab[mode].display();
}

// check if any button is pressed ? 
// if any button pressed, wait until release, return short or long (hold)
unsigned char UIchkButton() {
  if (btUP.isPress()) {
    btUP.waitRelease();
    return BTUP_PRESS ; 
  }
  if (btDN.isPress()) {
    btDN.waitRelease();
    return BTDN_PRESS ; 
  }
  if (btST.isPress())
    if (btST.waitRelease()==BUTTON_LONG)
      return BTST_HOLD ; 
    else
      return BTST_PRESS ;  
  return NOBT_PRESS ;
}

// wait until any button is pressed or timeout
// this may wait longer that watchdog timer. Don't forget to disable watchdog before calling this function
unsigned char UIwaitButton() {
  long tmCur=0, tmStart ;
  unsigned bt ; 
  tmStart = millis();
  while((tmCur - tmStart) < BTWAIT_DELAY) {
    if ((bt=UIchkButton()) != NOBT_PRESS) 
      return bt ;  // any button is pressed, 
    tmCur = millis();                  // get current time
  }   
  // if wait longer than timeout -> return with code
  return BTWAIT_TMOUT ;
}
unsigned char UImain() {
  unsigned char uiSt;
  if ((uiSt=UIchkButton())==NOBT_PRESS)
    return uiSt ;
  lcd.backlight();  // turn on backlight (in case wake up from ilde state
    switch (uiSt) {
      case BTUP_PRESS :   // shift to previous mode
        if (mode==0) 
          mode = MAX_MODE - 1; 
        else
          mode -= 1; 
        break;
      case BTDN_PRESS :   // shift to next mode
        if (mode==(MAX_MODE-1)) 
          mode = 0;
        else
          mode += 1; 
        break;
      case BTST_HOLD :   // goto setup function of each mode
        DISABLE_WATCHDOG;         // disable Watchdog temporary since it may wait for user input > 4 sec
        modeTab[mode].setting();  // call setting menu of the current mode
        ENABLE_WATCHDOG;          // re-enable Watchdog
        break;
      default:           // ST short press, consider : display help message
        dispMsg("U/D chg screen","hold S to set",DISP_ERR_DELAY);;
        break;
    }  
    modeTab[mode].display();  // refresh LCD screen
  return uiSt;
}

void manualCtrl() {
// not implemented yet
}

// clear all statistics data
void histClear() {
  dispMsg("Clear history? ","Hold SET confirm",0);
  if(UIwaitButton()==BTST_HOLD) { // ask to confirm  
    tStat.clrValue();
    hStat.clrValue();
  }
}

// LCD display for threshold
// Temp Lo:XX Hi:XX
// RH%  Lo:XX Hi:XX
#define NUM_THRESSET  4
// where to position cursor for each value
unsigned char posThresSet[NUM_THRESSET][2] = {{8,0},{14,0},{8,1},{14,1}};

// LCD display for timing
// Interval XX
// Disp.Off XXX
#define NUM_TIMINGSET  2
unsigned char posTimingSet[NUM_TIMINGSET][2] = {{9,0},{9,1}};

void thresSet() {
  paramSet( param.val, TEMPLO, posThresSet, NUM_THRESSET);
}
void timingSet() {
  paramSet( param.val, INTVL, posTimingSet, NUM_TIMINGSET);
}

// set parameters
// Up/Down button to change value of each parameters
// Set button to shift between parameters
// assume the compiler allocate contiguous memory for all parameters !!
void paramSet(int *ptr, unsigned char start, unsigned char posArray[][2], int numPos) {
  unsigned char bt;
  lcd.cursor();  lcd.blink();
  for (int i=start; i< start+numPos; i++) {
    do {
      delay(50);
      lcd.setCursor(posArray[i][0],posArray[i][1]);   
      bt=UIwaitButton();
      switch (bt) {           // up/down to change value
        case BTUP_PRESS : 
          if (ptr[i] < param_max[i])
            ptr[i] ++ ; 
          lcd.print(ptr[i],1);  
          break ; 
        case BTDN_PRESS : 
          if (ptr[i] > param_min[i])
            ptr[i] -- ; 
          lcd.print(ptr[i],1);  
          break ;        
        case BTWAIT_TMOUT :
          param.useDefault();
          return ;
      }
    } while (bt!=BTST_PRESS);  // push SET button to next value
    btST.waitRelease();
  }
  lcd.noCursor();  lcd.noBlink();
  if (param.validate()==PARAM_OK) // check if new values are valid
    param.save(); // save value to EEPROM
  else
    param.useDefault();
}

void dispCurrent() {
  lcd.clear();
  lcd.print("Temp ");  lcd.print(tCur,1); lcd.print("  "); 
  if (relayTemp.isOn()) lcd.print("ON ");  else    lcd.print("OFF");
  lcd.setCursor(0,1);
  lcd.print("RH%  ");  lcd.print(hCur,1);    lcd.print("  "); 
  if (relayHumd.isOn()) lcd.print("ON ");  else    lcd.print("OFF");
}

// Display statistics
// Temperature min - average - max
// Humidity    min - average - max
void dispHistory() {
  lcd.clear();
  lcd.print("T ");  lcd.print(tStat.min,1); lcd.print("-"); lcd.print(tStat.avg,1);lcd.print("-"); lcd.print(tStat.max,1); 
  lcd.setCursor(0,1);
  lcd.print("H ");  lcd.print(hStat.min,1); lcd.print("-"); lcd.print(hStat.avg,1);lcd.print("-"); lcd.print(hStat.max,1);
}

void dispThreshold() {
  lcd.clear();
  lcd.print("Temp ");  lcd.print("Lo:"); lcd.print(param.tLo,1);lcd.print(" Hi:"); lcd.print(param.tHi,1); 
  lcd.setCursor(0,1);
  lcd.print("RH%  ");  lcd.print("Lo:"); lcd.print(param.hLo,1);lcd.print(" Hi:"); lcd.print(param.hHi,1);     
}

void dispTiming() {
  lcd.clear();
  lcd.print("Interval ");  lcd.print(param.intvl,1);
  lcd.setCursor(0,1);
  lcd.print("Disp.Off ");  lcd.print(param.tmout,1);  
}

void dispMsg(const char *msg1, const char *msg2, int delayms) {
  lcd.clear();        lcd.print(msg1);
  lcd.setCursor(0,1); lcd.print(msg2);
  delay(delayms);
}

// ================== Initialize each components  ================= 
void setup() {
  DISABLE_WATCHDOG ;
  BLINK ;
  lcdInit();        // LCD
  serialInit();
  // Relay was initialized by creating object ioControl
  // buttonInit();     // Input button switches
  // sensorInit();     // Sensor - no need
  // Statistics data was initialized when creating object
  if (param.restore()!=PARAM_OK)
    dispMsg("Error","Load parameters",DISP_ERR_DELAY);  // load threshold and timing from EEPROM
  UIInit();
  BLINK ;
  ENABLE_WATCHDOG ;   // set Watchdog Timer to reset if no response within 1 sec
}

// ================== Main Loop  ================= 
void loop() {
  long tmCur = millis();                  // get current time in millisecond
  // MONITOR & CONTROL
  if ((tmCur < tmPrvSpl) ||               // if millis wrap to zero (several days), or
     (((tmCur-tmPrvSpl)/1000) > param.intvl)) { // if sampling interval reach
    sensorRead();                         // fetch value from sensor and store value to DHT variable
    tStat.addValue(tCur);                 // add new value and calculate statistics
    hStat.addValue(hCur);
    chkThres(tCur,hCur);                  // if Temp or Humd threshold is reached, take action
    if ((mode==MODE_CURRENT) || (mode==MODE_HISTORY)) // refresh LCD for current & history mode
      modeTab[mode].display();
    tmPrvSpl = tmCur ; 
  }
  // UI
  if (UImain() != NOBT_PRESS) {       // call UImain, if button press
    tmPrvBt = tmCur ;                 // mark the last interact time
  }
  if (((tmCur-tmPrvBt)/1000) > param.tmout) { // if no interact more than tmout period
    lcd.noBacklight();                // turn off LCD backlight to save energy
    mode=MODE_CURRENT;                // get back to current mode
  }
  SIGNAL_WATCHDOG;                    // Tell Watchdog that system is ok
  delay(5);                           // sleep with micro interval
}

