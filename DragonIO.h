/*
    https://github.com/SpenceKonde/ATTinyCore/blob/master/avr/cores/tiny/wiring_digital.c
*/

#ifndef DragonIO_H
#define DragonIO_H

class DragonIO
{
    public:
        enum callback_type_t { TYPE_NONE, TYPE_LOW, TYPE_HIGH, TYPE_CHANGE, TYPE_RISING, TYPE_FALLING };
        using callback_t = void (*)(uint8_t pin, callback_type_t type);
        
        struct data_t
        {
            volatile uint8_t *port;           // Порт пина.
            uint8_t pin;                      // Номер пина в порте.
            uint8_t state_new:1;              // Текущее состояние пина.
            uint8_t state_old:1;              // Текущее состояние пина.
            callback_type_t callback_type:3;  // Тип псевдопрерывания.
            callback_t callback;              // Колбек псевдопрерывания.
        };

        // Конструктор с регистрами, например DragonIO(&PIND, PD3);
        DragonIO(volatile uint8_t *port, uint8_t pin)
        {
            this->_data.port = port;
            this->_data.pin  = (1 << pin);
            
            return;
        }
        
        // Конструктор с пином, например DragonIO(3);
        DragonIO(uint8_t pin)
        {
            this->_data.port = _pin_to_basereg(pin);
            this->_data.pin  = _pin_to_bitmask(pin);
            
            return;
        }
        
        // Назначить пин на вход.
        void Input()
        {
            _direct_mode_input(this->_data.port, this->_data.pin);
            _direct_write_low(this->_data.port, this->_data.pin);
            
            return;
        }
        
        // Назначить пин на вход с подтяжкой к Vcc.
        void InputPullup()
        {
            _direct_mode_input(this->_data.port, this->_data.pin);
            _direct_write_high(this->_data.port, this->_data.pin);
            
            return;
        }
        
        // Назначить пин на выход и установить начальное состояние.
        void Output(bool state = false)
        {
            _direct_mode_output(this->_data.port, this->_data.pin);
            (state == true) ? this->High() : this->Low();
            
            return;
        }
        
        // Регистрация псевдопрерывания для пина ( INPUT, INPUT_PULLUP ).
        void RegCallback(callback_t callback, callback_type_t type)
        {
            this->_data.callback = callback;
            this->_data.callback_type = type;
        }
        
        // Прочитать состояние пина ( INPUT, INPUT_PULLUP ).
        bool Read()
        {
            this->_data.state_old = this->_data.state_new;
            this->_data.state_new = _direct_read(this->_data.port, this->_data.pin);
            
            return this->_data.state_new;
        }
        
        // Записать высокий уровень в пин ( OUTPUT ).
        void High()
        {
            _direct_write_high(this->_data.port, this->_data.pin);
            
            this->_data.state_old = this->_data.state_new;
            this->_data.state_new = true;
            
            return;
        }
        
        // Записать низкий уровень в пин ( OUTPUT ).
        void Low()
        {
            _direct_write_low(this->_data.port, this->_data.pin);
            
            this->_data.state_old = this->_data.state_new;
            this->_data.state_new = false;
            
            return;
        }
        
        // Инвертировать состояние пина ( OUTPUT ).
        void Toggle()
        {
            (this->_data.state_new == true) ? this->Low() : this->High();
            
            return;
        }
        
        // Псевдопрерывание для работы колбеков ( INPUT, INPUT_PULLUP ).
        void Processing()
        {
            if(this->_data.callback_type != TYPE_NONE)
            {
                this->Read();
                
                switch(this->_data.callback_type)
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


    static volatile uint8_t* _pin_to_basereg(uint8_t pin)             
    {
        return (portInputRegister(digitalPinToPort(pin)));
    }
    static uint8_t _pin_to_bitmask(uint8_t pin)             
    {
        return (digitalPinToBitMask(pin));
    }
    static uint8_t _direct_read(volatile uint8_t* base, uint8_t mask)
    {
        return (((*(base)) & (mask)) ? true : false);
    }
    static uint8_t _direct_mode_input(volatile uint8_t* base, uint8_t mask)
    {
        return ((*((base)+1)) &= ~(mask));
    }
    static uint8_t _direct_mode_output(volatile uint8_t* base, uint8_t mask)
    {
        return ((*((base)+1)) |= (mask));
    }
    static uint8_t _direct_write_low(volatile uint8_t* base, uint8_t mask)
    {
        return ((*((base)+2)) &= ~(mask));
    }
    static uint8_t _direct_write_high(volatile uint8_t* base, uint8_t mask)
    {
        return ((*((base)+2)) |= (mask));
    }

    private:
        data_t _data;

};

#endif // end of DragonIO_H