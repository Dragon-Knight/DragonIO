/*
    https://github.com/SpenceKonde/ATTinyCore/blob/master/avr/cores/tiny/wiring_digital.c
*/

#ifndef FastPin_H
#define FastPin_H

class FastPin
{
private:
    // Порт пина.
    volatile uint8_t *_port;
    // Номер пина в порте.
    uint8_t _pin;

public:

    // Конструктор с регистрами, например FastPin(&PIND, PD3);
    FastPin(volatile uint8_t *port, uint8_t pin)
    {
        _port = port;
        _pin  = (1 << pin);
        
        return;
    }
    
    // Конструктор с пином, например FastPin(3);
    FastPin(uint8_t pin)
    {
        _port = portInputRegister( digitalPinToPort(pin) );
        _pin  = digitalPinToBitMask(pin);
    }
    
    // Назначить пин на вход.
    void input()
    {
        _direct_mode_input(_port, _pin);
        low();
    }
    
    // Назначить пин на вход с подтяжкой к Vcc.
    void pullup()
    {
        _direct_mode_input(_port, _pin);
        high();
    }
    
    // Назначить пин на выход и установить начальное состояние.
    void output()
    {
        _direct_mode_output(_port, _pin);
        low();
    }
    void output(bool state)
    {
        _direct_mode_output(_port, _pin);
        state ? high() : low();
    }    
    
    // Прочитать состояние пина ( INPUT, INPUT_PULLUP ).
    bool read()
    {
        return (*_port) & _pin;
    }
    
    // Записать высокий уровень в пин ( OUTPUT ).
    void high()
    {
        _direct_write_high(_port, _pin);
    }
    
    // Записать низкий уровень в пин ( OUTPUT ).
    void low()
    {
        _direct_write_low(_port, _pin);
    }
        
private:

    static void _direct_mode_input(volatile uint8_t* base, uint8_t mask)
    {
        *(base + 1) &= ~mask;
    }
    static void _direct_mode_output(volatile uint8_t* base, uint8_t mask)
    {
        *(base + 1) |= mask;
    }
    static void _direct_write_low(volatile uint8_t* base, uint8_t mask)
    {
        *(base + 2) &= ~mask;
    }
    static void _direct_write_high(volatile uint8_t* base, uint8_t mask)
    {
        *(base + 2) |= mask;
    }

};

#endif // end of FastPin_H