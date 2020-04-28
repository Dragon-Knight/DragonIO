/*
 * DragonIO.h
 * Very Fast GPIO library for Arduino and other Arduino-based boards.
 *
 *	@author		Nikolai Tikhonov aka Dragon_Knight <dubki4132@mail.ru>, https://vk.com/globalzone_edev
 *	@licenses	MIT https://opensource.org/licenses/MIT
 *	@repo		https://github.com/Dragon-Knight/DragonIO
 *
 * Special thanks to valmat https://github.com/valmat
 *
 */

#pragma once

class DragonIO
{
	public:
		enum mode_t { MODE_NORMAL, MODE_DELAY, MODE_BLINK };
		enum callback_type_t { TYPE_NONE, TYPE_LOW, TYPE_HIGH, TYPE_CHANGE, TYPE_RISING, TYPE_FALLING };
		using callback_t = void (*)(uint8_t pin, callback_type_t type);
		
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
		
		// Регистрация псевдопрерывания для пина ( INPUT, INPUT_PULLUP ).
		void RegCallback(callback_t callback, callback_type_t type)
		{
			this->_data.callback = callback;
			this->_data.callback_type = type;
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
			//if(force == true) this->_data.mode = MODE_NORMAL;
			
			return;
		}
		
		// Записать высокий уровень в пин ( OUTPUT ) на указанное время.
		void High(uint16_t time)
		{
			this->High();
			
			this->_data.delay_time = millis() + time;
			this->_data.mode = MODE_DELAY;
			
			return;
		}
		
		// Записать низкий уровень в пин ( OUTPUT ).
		void Low()
		{
			DIRECT_WRITE_LOW(this->_data.port, this->_data.pin);
			
			this->_data.state_old = this->_data.state_new;
			this->_data.state_new = false;
			//if(force == true) this->_data.mode = MODE_NORMAL;
			
			return;
		}
		
		// Записать низкий уровень в пин ( OUTPUT ) на указанное время.
		void Low(uint16_t time)
		{
			this->Low();
			
			this->_data.delay_time = millis() + time;
			this->_data.mode = MODE_DELAY;
			
			return;
		}
		
		// Запустить циклическое ВКЛ-ВЫКЛ пина.
		void Blink(uint16_t time_on, uint16_t time_off)
		{
			this->_data.blinkon_time = time_on;
			this->_data.blinkoff_time = time_off;
			this->_data.mode = MODE_BLINK;
			
			return;
		}
		
		// Инвертировать состояние пина ( OUTPUT ) и вернуть новое состояние.
		bool Toggle()
		{
			(this->_data.state_new == true) ? this->Low() : this->High();
			
			return this->_data.state_new;
		}
		
		// Выдать строб-импульс с максимально возможной скоростью.
		bool StrobeHigh()
		{
			DIRECT_WRITE_HIGH(this->_data.port, this->_data.pin);
			DIRECT_WRITE_LOW(this->_data.port, this->_data.pin);
		}
		bool StrobeLow()
		{
			DIRECT_WRITE_LOW(this->_data.port, this->_data.pin);
			DIRECT_WRITE_HIGH(this->_data.port, this->_data.pin);
		}
		
		// Псевдопрерывание для работы колбеков ( INPUT, INPUT_PULLUP ).
		void Processing(uint32_t time = millis())
		{
			if(this->_data.mode != MODE_NORMAL)
			{
				if(this->_data.delay_time < time)
				{
					bool state = this->Toggle();
					
					switch(this->_data.mode)
					{
						case MODE_DELAY:
						{
							this->_data.mode = MODE_NORMAL;
							
							break;
						}
						case MODE_BLINK:
						{
							this->_data.delay_time = ((state == true) ? this->_data.blinkon_time : this->_data.blinkoff_time) + time;
							
							break;
						}
					}
				}
			}
			
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
	
	protected:
	
	private:
		static volatile uint8_t *PIN_TO_BASEREG(uint8_t pin){ return portInputRegister(digitalPinToPort(pin)); }
		static uint8_t PIN_TO_BITMASK(uint8_t pin){ return digitalPinToBitMask(pin); }
		static bool DIRECT_READ(volatile uint8_t *base, uint8_t mask){ return (*base & mask) ? true : false; }
		static void DIRECT_MODE_INPUT(volatile uint8_t *base, uint8_t mask) { *(base+1) &= ~mask; }
		static void DIRECT_MODE_OUTPUT(volatile uint8_t *base, uint8_t mask){ *(base+1) |= mask; }
		static void DIRECT_WRITE_LOW(volatile uint8_t *base, uint8_t mask){ *(base+2) &= ~mask; }
		static void DIRECT_WRITE_HIGH(volatile uint8_t *base, uint8_t mask){ *(base+2) |= mask; }
		
		struct data_t
		{
			volatile uint8_t *port;				// Порт пина.
			uint8_t pin;						// Номер пина в порте.
			uint8_t state_new:1;				// Текущее состояние пина.
			uint8_t state_old:1;				// Предыдущие состояние пина.
			mode_t mode:2;						// Текущий режим работы: Обычный пин, С задержкой, Мигалка.
			uint8_t __offset:1;					// 
			callback_type_t callback_type:3;	// Тип псевдопрерывания.
			callback_t callback;				// Колбек псевдопрерывания.
			uint32_t delay_time;				// Время задержки при MODE_DELAY или MODE_BLINK.
			uint16_t blinkon_time;				// Время первой стадии 'мигания'.
			uint16_t blinkoff_time;				// Время второй стадии 'мигания'.
		} _data;
};
