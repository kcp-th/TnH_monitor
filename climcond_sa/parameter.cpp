/*
 * Object to manage configuration parameters
 * Currently include
 *   temperature threshold (low/high, 0-99C default 23-32C
 *   humidity threshold    (low/high) 0-99% default 70-85%
 *   sampling interval                0-600 default 2 sec
 *   timeout for screen saver         0-3600 default ?
 * Functions to save/restore values to/from EEPROM so parameters are saved during power off
 */
#include "parameter.h"

parameter::parameter() {    
  useDefault();
}

// write parameters to EEPROM
void parameter::save() {
  int eeAddr = EE_START_ADDR ;
  int crc ; 
  EEPROM.put(eeAddr, val);
  eeAddr += sizeof(val);
// calculate check sum and write to the end, to prevent error
  crc = calcrc((char *)val,sizeof(val));
  EEPROM.put(eeAddr, crc);
}

// read parameters from EEPROM
unsigned char parameter::restore() {
  int eeAddr = EE_START_ADDR ;
  int crc;
  unsigned char rc;
  EEPROM.get(eeAddr, val);
  eeAddr += sizeof(val);
  EEPROM.get(eeAddr, crc);
  if (crc != calcrc((char *)val, sizeof(val))) {
    useDefault();
    return PARAM_CHKSUM_ERR;
  }
  if ((rc=validate())==PARAM_OK) 
    return PARAM_OK ;
  else {
    useDefault();
    return rc;
  }
}

// hard code: factory default values
void parameter::useDefault() {
  tLo = 28 ;  tHi = 32 ;    // default temperature between 28-32
  hLo = 70 ;  hHi = 85 ;    // default humidity between 70-85%
  intvl = 2 ;               // default sampling interval: 1 minute
  tmout = 300 ;             // default LCD off timeout: 5 minutes
}

// check if parameters are valid
unsigned char parameter::validate() {
  if ((tLo >= tHi) ||                                           // Lo must be lower than Hi
      (tLo < param_min[TEMPLO]) || (tHi > param_max[TEMPHI]))   // within allowed min - max 
                                                                return PARAM_INV_TEMPTHRES ;      
  if ((hLo >= hHi) || 
      (hLo < param_min[HUMDLO]) || (hHi > param_max[HUMDHI])) 
                                                                return PARAM_INV_HUMDTHRES ;
  if ((intvl < param_min[INTVL]) || (intvl > param_max[INTVL]))    // no negative, within min-max
                                                                return PARAM_INV_INTVL ;
  if ((tmout < param_min[TMOUT]) || (tmout > param_max[TMOUT]))
                                                                return PARAM_INV_TMOUT ;    
  return PARAM_OK ;  
}

// Calculate CRC for the block of data
// Routine taken from
// http://web.mit.edu/6.115/www/amulet/xmodem.htm
int parameter::calcrc(char *ptr, int count) 
{
    int  crc;
    char i;

    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    return (crc);
}
