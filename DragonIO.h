/*
	https://github.com/SpenceKonde/ATTinyCore/blob/master/avr/cores/tiny/wiring_digital.c
*/

#ifndef DragonIO_H
#define DragonIO_H

#define PIN_TO_BASEREG(pin)				(portInputRegister(digitalPinToPort(pin)))
#define PIN_TO_BITMASK(pin)				(digitalPinToBitMask(pin))
#define DIRECT_READ(base, mask)			(((*(base)) & (mask)) ? true : false)
#define DIRECT_MODE_INPUT(base, mask)	((*((base)+1)) &= ~(mask))
#define DIRECT_MODE_OUTPUT(base, mask)	((*((base)+1)) |= (mask))
#define DIRECT_WRITE_LOW(base, mask)	((*((base)+2)) &= ~(mask))
#define DIRECT_WRITE_HIGH(base, mask)	((*((base)+2)) |= (mask))

class DragonIO
{
	public:
	
	enum mode_t { MODE_NONE, MODE_INPUT, MODE_INPUT_PULLUP, MODE_OUTPUT };
	enum callback_type_t { TYPE_NONE, TYPE_LOW, TYPE_HIGH, TYPE_CHANGE, TYPE_RISING, TYPE_FALLING };
	using callback_t = void (*)(uint8_t pin, callback_type_t type);
	
	struct data_t
	{
		volatile uint8_t *port;				// Порт пина.
		uint8_t pin;						// Номер пина в порте.
		mode_t mode:2;						// Режим работы пина. А нужен?
		uint8_t state_new:1;				// Текущее состояние пина.
		uint8_t state_old:1;				// Текущее состояние пина.
		uint8_t __offset:1;					// 
		callback_type_t callback_type:3;	// Тип псевдопрерывания.
		callback_t callback;				// Колбек псевдопрерывания.
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
		
		void Begin(mode_t mode)
		{
			this->_data.mode = mode;
			
			switch(mode)
			{
				case INPUT:
				{
					DIRECT_MODE_INPUT(this->_data.port, this->_data.pin);
					DIRECT_WRITE_LOW(this->_data.port, this->_data.pin);
					
					break;
				}
				case INPUT_PULLUP:
				{
					DIRECT_MODE_INPUT(this->_data.port, this->_data.pin);
					DIRECT_WRITE_HIGH(this->_data.port, this->_data.pin);
					
					break;
				}
				case OUTPUT:
				{
					DIRECT_MODE_OUTPUT(this->_data.port, this->_data.pin);
					
					break;
				}
			}
			
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
			this->_data.state_new = DIRECT_READ(this->_port, this->_pin);
			
			return this->_data.state_new;
		}
		
		// Записать высокий уровень в пин ( OUTPUT ).
		void High()
		{
			DIRECT_WRITE_HIGH(this->_port, this->_pin);
			
			this->_data.state_old = this->_data.state_new;
			this->_data.state_new = true;
			
			return;
		}
		
		// Записать низкий уровень в пин ( OUTPUT ).
		void Low()
		{
			DIRECT_WRITE_LOW(this->_port, this->_pin);
			
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
		data_t _data;
		volatile uint8_t *_port;
		uint8_t _pin;
		mode_t _mode;
};

#endif
