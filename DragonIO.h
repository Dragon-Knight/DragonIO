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

#include "Arduino.h"

#define IO_LOW		(0)
#define IO_HIGH		(1)

class DragonIO
{
	public:
		
		// Конструктор без параметров.
		DragonIO()
		{
			return;
		}
		
		// Конструктор с регистрами, например DragonIO(&PIND, PD3);
		DragonIO(volatile uint8_t *port, uint8_t pin = 255)
		{
			SetPort(port, pin);
			
			return;
		}
		
		// Конструктор с пином, например DragonIO(3);
		DragonIO(uint8_t pin)
		{
			SetPin(pin);
			
			return;
		}
		
		// Установить Порт и Пин, в случае использования пустого конструктора.
		void SetPort(volatile uint8_t *port, uint8_t pin = 255)
		{
			_data.port = port;
			_data.pin = (pin > 7) ? pin : (1 << pin);
			
			return;
		}
		
		// Установить пин Ардуино, в случае использования пустого конструктора.
		void SetPin(uint8_t pin)
		{
			_data.port = PIN_TO_BASEREG(pin);
			_data.pin = PIN_TO_BITMASK(pin);
			
			return;
		}
		
		// Назначить пин на вход.
		void Input()
		{
			DIRECT_MODE_INPUT(_data.port, _data.pin);
			DIRECT_WRITE_LOW(_data.port, _data.pin);
			
			return;
		}
		
		// Назначить пин на вход с подтяжкой к Vcc.
		void InputPullup()
		{
			DIRECT_MODE_INPUT(_data.port, _data.pin);
			DIRECT_WRITE_HIGH(_data.port, _data.pin);
			
			return;
		}
		
		// Назначить пин на выход и установить начальное состояние.
		void Output(bool state = IO_LOW)
		{
			DIRECT_MODE_OUTPUT(_data.port, _data.pin);
			//_data.state_default = state;
			(state == IO_HIGH) ? High() : Low();
			
			return;
		}
		
		// Прочитать состояние пина ( INPUT, INPUT_PULLUP ).
		bool Read()
		{
			_data.state_old = _data.state_new;
			_data.state_new = DIRECT_READ(_data.port, _data.pin);
			
			return _data.state_new;
		}
		
		// Записать в пин указанный уровень.
		void Write(bool state)
		{
			(state == IO_HIGH) ? High() : Low();
			
			return;
		}
		
		// Записать высокий уровень в пин ( OUTPUT ).
		void High()
		{
			DIRECT_WRITE_HIGH(_data.port, _data.pin);
			
			_data.state_old = _data.state_new;
			_data.state_new = IO_HIGH;
			
			return;
		}
		
		// Записать низкий уровень в пин ( OUTPUT ).
		void Low()
		{
			DIRECT_WRITE_LOW(_data.port, _data.pin);
			
			_data.state_old = _data.state_new;
			_data.state_new = IO_LOW;
			
			return;
		}
		
		// Прочитать значение порта целиком ( INPUT, INPUT_PULLUP ).
		byte Raw()
		{
			return *_data.port;
		}
		
		// Записать значение порта целиком ( OUTPUT ).
		void Raw(byte data)
		{
			*_data.port = data;
			
			return;
		}
		
		// Инвертировать состояние пина ( OUTPUT ) и вернуть новое состояние.
		bool Toggle()
		{
			(_data.state_new == IO_HIGH) ? Low() : High();
			
			return _data.state_new;
		}
		
		// Выдать строб-импульс с максимально возможной скоростью.
		void StrobeHigh()
		{
			DIRECT_WRITE_HIGH(_data.port, _data.pin);
			DIRECT_WRITE_LOW(_data.port, _data.pin);
		}
		void StrobeLow()
		{
			DIRECT_WRITE_LOW(_data.port, _data.pin);
			DIRECT_WRITE_HIGH(_data.port, _data.pin);
		}
	
	protected:
		struct data_t
		{
			volatile uint8_t *port;				// Порт пина.
			uint8_t pin;						// Номер пина в порте.
			uint8_t state_new:1;				// Текущее состояние пина.
			uint8_t state_old:1;				// Предыдущие состояние пина.
			//uint8_t state_default:1;			// Состояние пина при инициализации.
			uint8_t __offset:6;					// Смещение до байта.
		} _data;
	
	private:
		static volatile uint8_t *PIN_TO_BASEREG(uint8_t pin){ return portInputRegister(digitalPinToPort(pin)); }
		static uint8_t PIN_TO_BITMASK(uint8_t pin){ return digitalPinToBitMask(pin); }
		static bool DIRECT_READ(volatile uint8_t *base, uint8_t mask){ return (*base & mask) ? IO_HIGH : IO_LOW; }
		static void DIRECT_MODE_INPUT(volatile uint8_t *base, uint8_t mask) { *(base+1) &= ~mask; }
		static void DIRECT_MODE_OUTPUT(volatile uint8_t *base, uint8_t mask){ *(base+1) |= mask; }
		static void DIRECT_WRITE_LOW(volatile uint8_t *base, uint8_t mask){ *(base+2) &= ~mask; }
		static void DIRECT_WRITE_HIGH(volatile uint8_t *base, uint8_t mask){ *(base+2) |= mask; }
};
