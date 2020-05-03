/*
 * DragonIOBasic.h
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

class DragonIOBasic
{
	public:
		// Конструктор с регистрами, например DragonIOBasic(&PIND, PD3);
		DragonIOBasic(volatile uint8_t *port, uint8_t pin)
		{
			this->_data.port = port;
			this->_data.pin = (1 << pin);
			
			return;
		}
		
		// Конструктор с пином, например DragonIOBasic(3);
		DragonIOBasic(uint8_t pin)
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
			uint8_t __offset:6;					// Смещение до байта.
		} _data;
};
