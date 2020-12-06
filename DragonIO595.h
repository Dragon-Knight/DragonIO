/*
 * DragonIO595.h
 * Fast library for 74HC595 chip.
 *
 *	@author		Nikolai Tikhonov aka Dragon_Knight <dubki4132@mail.ru>, https://vk.com/globalzone_edev
 *	@licenses	MIT https://opensource.org/licenses/MIT
 *	@repo		https://github.com/Dragon-Knight/DragonIO
 *
 * Special thanks to valmat https://github.com/valmat
 *
 */

#pragma once

#include "DragonIO.h"

class DragonIO595
{
	public:
		DragonIO595(uint8_t pin_data, uint8_t pin_clock, uint8_t pin_latch, uint8_t pin_enable, uint8_t pin_reset)
		{
			_pins.di.SetPin(pin_data);
			_pins.di.Output(IO_LOW);
			
			_pins.sh.SetPin(pin_clock);
			_pins.sh.Output(IO_LOW);
			
			_pins.st.SetPin(pin_latch);
			_pins.st.Output(IO_LOW);
			
			_pins.oe.SetPin(pin_enable);
			_pins.oe.Output(IO_HIGH);
			
			_pins.mr.SetPin(pin_reset);
			_pins.mr.Output(IO_LOW);
			
			_full_pin = true;
			
			return;
		}
		
		DragonIO595(uint8_t pin_data, uint8_t pin_clock, uint8_t pin_latch)
		{
			_pins.di.SetPin(pin_data);
			_pins.di.Output(IO_LOW);
			
			_pins.sh.SetPin(pin_clock);
			_pins.sh.Output(IO_LOW);
			
			_pins.st.SetPin(pin_latch);
			_pins.st.Output(IO_LOW);
			
			_full_pin = false;
			
			return;
		}
		
		// Включить \ Выключить выход регистра.
		void Enable(bool mode)
		{
			if(_full_pin == true)
			{
				_pins.oe.Write(!mode);
			}
			
			return;
		}
		
		// Сброс регистра.
		void Reset()
		{
			if(_full_pin == true)
			{
				_pins.mr.StrobeLow();
			}
			
			return;
		}
		
		// Записать байт в регистр.
		void Write(byte data, bool latch = false)
		{
			if(latch == true) _pins.st.Low();
			for(uint8_t i = 0; i < 8; ++i)
			{
				_pins.di.Write( data & 0b10000000 );
				data <<= 1;
				
				_pins.sh.StrobeHigh();
			}
			if(latch == true) _pins.st.High();
			
			return;
		}
		
		// Записать байты в регистр.
		void Write(byte *data, uint8_t size, bool latch = false)
		{
			if(latch == true) _pins.st.Low();
			while(--size != 255)
			{
				Write(data[size], false);
			}
			if(latch == true) _pins.st.High();
			
			return;
		}
	
	private:
		struct HC595_t
		{
			DragonIO di;	// Serial data input.
			DragonIO sh;	// Shift register clock input.
			DragonIO st;	// Storage register clock input.
			DragonIO oe;	// Output enable (active LOW).
			DragonIO mr;	// Master reset (active LOW).
		} _pins;
		bool _full_pin;
};
