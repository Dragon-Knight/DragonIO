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
            this->_data.pin = (1 << pin);
            
            return;
        }
        
        // Конструктор с пином, например DragonIO(3);
        DragonIO(uint8_t pin)
        {
            this->_data.port = PIN_TO_BASEREG(pin);
            this->_data.pin = PIN_TO_BITMASK(pin);
            
            return;
        }
        
        // Назначить пин на вход.
        void Input()
        {
            DIRECT_MODE_INPUT(this->_data.port, this->_data.pin);
            DIRECT_WRITE_LOW(this->_data.port, this->_data.pin);
            
            return;
        }
        
        // Назначить пин на вход с подтяжкой к Vcc.
        void InputPullup()
        {
            DIRECT_MODE_INPUT(this->_data.port, this->_data.pin);
            DIRECT_WRITE_HIGH(this->_data.port, this->_data.pin);
            
            return;
        }
        
        // Назначить пин на выход и установить начальное состояние.
        void Output(bool state = false)
        {
            DIRECT_MODE_OUTPUT(this->_data.port, this->_data.pin);
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
            this->_data.state_new = DIRECT_READ(this->_data.port, this->_data.pin);
            
            return this->_data.state_new;
        }
        
        // Записать высокий уровень в пин ( OUTPUT ).
        void High()
        {
            DIRECT_WRITE_HIGH(this->_data.port, this->_data.pin);
            
            this->_data.state_old = this->_data.state_new;
            this->_data.state_new = true;
            
            return;
        }
        
        // Записать низкий уровень в пин ( OUTPUT ).
        void Low()
        {
            DIRECT_WRITE_LOW(this->_data.port, this->_data.pin);
            
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
        void Processing(uint32_t time = millis())
        {
            if(this->_data.callback_type != TYPE_NONE)
            {
                this->Read();
                
                bool is_run = false;
                switch(this->_data.callback_type)
                {
                    case TYPE_LOW:
                    {
                        if(this->_data.state_new == false){ is_run = true; break; }
                    }
                    case TYPE_HIGH:
                    {
                        if(this->_data.state_new == true){ is_run = true; break; }
                    }
                    case TYPE_CHANGE:
                    {
                        if(this->_data.state_new != this->_data.state_old){ is_run = true; break; }
                    }
                    case TYPE_RISING:
                    {
                        if(this->_data.state_old == false && this->_data.state_new == true){ is_run = true; break; }
                    }
                    case TYPE_FALLING:
                    {
                        if(this->_data.state_old == true && this->_data.state_new == false){ is_run = true; break; }
                    }
                }
                if(is_run == true)
                {
                    this->_data.callback(0, this->_data.callback_type);
                }
            }
            
            return;
        }
    private:


    static volatile uint8_t* PIN_TO_BASEREG(uint8_t pin)             
    {
        return (portInputRegister(digitalPinToPort(pin)));
    }
    static uint8_t PIN_TO_BITMASK(uint8_t pin)             
    {
        return (digitalPinToBitMask(pin));
    }
    static uint8_t DIRECT_READ(volatile uint8_t* base, uint8_t mask)
    {
        return (((*(base)) & (mask)) ? true : false);
    }
    static uint8_t DIRECT_MODE_INPUT(volatile uint8_t* base, uint8_t mask)
    {
        return ((*((base)+1)) &= ~(mask));
    }
    static uint8_t DIRECT_MODE_OUTPUT(volatile uint8_t* base, uint8_t mask)
    {
        return ((*((base)+1)) |= (mask));
    }
    static uint8_t DIRECT_WRITE_LOW(volatile uint8_t* base, uint8_t mask)
    {
        return ((*((base)+2)) &= ~(mask));
    }
    static uint8_t DIRECT_WRITE_HIGH(volatile uint8_t* base, uint8_t mask)
    {
        return ((*((base)+2)) |= (mask));
    }

    private:
        data_t _data;

};

#endif // end of DragonIO_H