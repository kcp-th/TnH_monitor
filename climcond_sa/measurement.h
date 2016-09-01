#ifndef _MEASUREMENT_H_
#define _MEASUREMENT_H_

#define MAX_HOUR  24
#define MAX_INTVL   
class measurement   
{
public:
    measurement();
    void addValue( double val );
    void clrValue();
    
    double min, max, avg;  // since last restart

private:
    double sum;
    int num;
};
#endif
