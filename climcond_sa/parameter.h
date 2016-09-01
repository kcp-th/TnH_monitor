#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#include <EEPROM.h>
#define EE_START_ADDR       0
#define PARAM_OK            0
#define PARAM_CHKSUM_ERR    1   // fail to restore from checksum error
#define PARAM_INV_TEMPTHRES 2   // invalid temperature threshold ex: low > high
#define PARAM_INV_HUMDTHRES 3   // invalid humidity threshold ex: low > high
#define PARAM_INV_INTVL     4   // invalid interval
#define PARAM_INV_TMOUT     5   // invalid timeout

#define TEMPLO     0
#define TEMPHI     1
#define HUMDLO     2
#define HUMDHI     3
#define INTVL      4
#define TMOUT      5
#define NUM_PARAMETERS  6


#define tLo   val[TEMPLO]
#define tHi   val[TEMPHI]
#define hLo   val[HUMDLO]
#define hHi   val[HUMDHI]
#define intvl val[INTVL]
#define tmout val[TMOUT]

// min and max value allowed for each parameters
const int param_min[NUM_PARAMETERS] = {  0,  0,  0,  0,   0,    0 };
const int param_max[NUM_PARAMETERS] = { 99, 99, 99, 99, 600, 3600 };

class parameter   
{
public:
  parameter();
  void useDefault();
  void save();
  unsigned char restore();
  unsigned char validate();

  int val[NUM_PARAMETERS];
private:
  int calcrc(char *ptr, int count);
};
#endif
