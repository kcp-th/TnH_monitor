/*
 * Object to maintain statistics data
 * Each object store min, max, average
 * one object for temperature and one for humidity 
 * Functions to save/restore values to/from EEPROM so parameters are saved during power off
 * Extend is object for other measurement ex; ambient light, CO2, power consumption, etc
 */
#include "measurement.h"

measurement::measurement() {
  clrValue();

}

// clear all data to zero
void measurement::clrValue() {
  min=0; 
  max=0;
  sum=0;
  num=0;
  avg=0;
}

// add new value, just measured from sensor, and update statistics
void measurement::addValue( double val ) {
  if (min==0) {
    min = val ; 
    max = val ;
  }
  else {
    if (min > val) 
      min = val ;
    if (max < val) 
      max = val ;
  }
  sum += val ; 
  num ++ ; 
  avg = sum / num ;
}

