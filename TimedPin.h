#ifndef TimedPin_H
#define TimedPin_H

#include "DragonIO.h"

class TimedPin
{
private:
    DragonIO _pin;
    // When timer started
    uint32_t _start_time;
public:

    // cast operator to DragonIO
    operator DragonIO() const
    {
        return _pin; 
    }


};

#endif // end of TimedPin_H
