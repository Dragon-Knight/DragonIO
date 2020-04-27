#ifndef TimedPin_H
#define TimedPin_H

#include "FastPin.h"

class TimedPin
{
private:
    FastPin _pin;
    // When timer will finished
    uint32_t _end_time;
public:

    // Constructors
    TimedPin(volatile uint8_t *port, uint8_t pin) :
        _pin(port, pin)
    {}
    
    TimedPin(uint8_t pin) :
        _pin(pin)
    {}

    // cast operator to FastPin
    operator FastPin() const
    {
        return _pin; 
    }
    
    // Set pin state to high with duration
    void high(uint16_t duration)
    {
        _pin.high();
        _update_timer(duration);
    }

    // Set pin state to low with duration
    void low(uint16_t duration)
    {
        _pin.low();
        _update_timer(duration);
    }
    
    // Invert pin state
    void toggle(uint16_t duration)
    {
        _pin.toggle();
        _update_timer(duration);
    }

    // Call this method every iteration
    // Will toogle pin automaticly if duration was expired
    void update()
    {
        // If timeout is expired
        if(_end_time <= millis())
        {
            _pin.toggle();
        }
    }

private:
    void _update_timer(uint16_t duration)
    {
        _end_time = millis() + duration;
    }

};

#endif // end of TimedPin_H
