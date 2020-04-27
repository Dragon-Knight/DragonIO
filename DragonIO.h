/*
    https://github.com/SpenceKonde/ATTinyCore/blob/master/avr/cores/tiny/wiring_digital.c
*/

#ifndef DragonIO_H
#define DragonIO_H

#include "FastPin.h"

class DragonIO
{
public:
    enum callback_type_t { TYPE_NONE, TYPE_LOW, TYPE_HIGH, TYPE_CHANGE, TYPE_RISING, TYPE_FALLING };
    using callback_t = void (*)(uint8_t pin, callback_type_t type);
    
    struct data_t
    {
        uint8_t state_new:1;              // Текущее состояние пина.
        uint8_t state_old:1;              // Текущее состояние пина.
        callback_type_t callback_type:3;  // Тип псевдопрерывания.
        callback_t callback;              // Колбек псевдопрерывания.
    };

    // Конструктор с регистрами, например DragonIO(&PIND, PD3);
    DragonIO(volatile uint8_t *port, uint8_t pin) :
        _pin(port, pin)
    {}
    
    // Конструктор с пином, например DragonIO(3);
    DragonIO(uint8_t pin) :
        _pin(pin)
    {}
    
    // Назначить пин на вход.
    void input()
    {
        _pin.input();
    }
    
    // Назначить пин на вход с подтяжкой к Vcc.
    void pullup()
    {
        _pin.pullup();
    }
    
    // Назначить пин на выход и установить начальное состояние.
    void output()
    {
        _pin.output();
    }
    void output(bool state)
    {
        _pin.output(state);
    }    
    
    // Прочитать состояние пина ( INPUT, INPUT_PULLUP ).
    bool read()
    {
        _data.state_old = _data.state_new;
        _data.state_new = _pin.read();
        
        return _data.state_new;
    }
    
    // Записать высокий уровень в пин ( OUTPUT ).
    void high()
    {
        _pin.high();
        _data.state_old = _data.state_new;
        _data.state_new = true;
    }
    
    // Записать низкий уровень в пин ( OUTPUT ).
    void low()
    {
        _pin.low();
        _data.state_old = _data.state_new;
        _data.state_new = false;
    }

    // Инвертировать состояние пина ( OUTPUT ).
    void toggle()
    {
        _data.state_new ? low() : high();
    }
    
    // Регистрация псевдопрерывания для пина ( INPUT, INPUT_PULLUP ).
    void regCallback(callback_t callback, callback_type_t type)
    {
        _data.callback = callback;
        _data.callback_type = type;
    }
    
    // Псевдопрерывание для работы колбеков ( INPUT, INPUT_PULLUP ).
    void processing()
    {
        if(_data.callback_type != TYPE_NONE)
        {
            read();
            
            switch(_data.callback_type)
            {
                case TYPE_LOW:
                {
                    if(_data.state_new) {
                        return;
                    }
                }
                case TYPE_HIGH:
                {
                    if(!_data.state_new) {
                        return;
                    }
                }
                case TYPE_CHANGE:
                {
                    if(_data.state_new == _data.state_old) {
                        return;
                    }
                }
                case TYPE_RISING:
                {
                    if(_data.state_old || !_data.state_new) {
                        return;
                    }
                }
                case TYPE_FALLING:
                {
                    if(!_data.state_old || _data.state_new) {
                        return;
                    }
                }
                case TYPE_NONE:
                {
                    return;
                }                    
            }
            _data.callback(0, _data.callback_type);
        }
        
        return;
    }

private:
    FastPin _pin;
    data_t _data;

};

#endif // end of DragonIO_H