/*
	
*/

#pragma once

#include "DragonIOBasic.h"

class DragonIO595
{
	public:
		DragonIO595(uint8_t pin_data, uint8_t pin_clock, uint8_t pin_latch)
		{
			this->_pins.di = DragonIOBasic(pin_data);
			this->_pins.di.Output();
			
			this->_pins.sh = DragonIOBasic(pin_clock);
			this->_pins.sh.Output();
			
			this->_pins.st = DragonIOBasic(pin_latch);
			this->_pins.st.Output();
			
			this->Reset();
			
			return;
		}
		
		DragonIO595(uint8_t pin_data, uint8_t pin_clock, uint8_t pin_latch, uint8_t pin_enable, uint8_t pin_reset)
		{
			this->_pins.di = DragonIOBasic(pin_data);
			this->_pins.di.Output();
			
			this->_pins.sh = DragonIOBasic(pin_clock);
			this->_pins.sh.Output();
			
			this->_pins.st = DragonIOBasic(pin_latch);
			this->_pins.st.Output();
			
			this->_pins.oe = DragonIOBasic(pin_enable);
			this->_pins.oe.Output(true);
			
			this->_pins.mr = DragonIOBasic(pin_reset);
			this->_pins.mr.Output(true);
			
			this->Reset();
			
			return;
		}
		
		// Включить \ Выключить выход регистра.
		void Enable(bool mode)
		{
			this->_pins.oe.Write(!mode);
			
			return;
		}
		
		// Сброс регистра.
		void Reset()
		{
			this->_pins.mr.StrobeLow();
			
			return;
		}
		
		// Записать байт в регистр.
		void WriteByte(byte data, bool latch = false)
		{
			if(latch == true) this->_pins.st.Low();
			for(uint8_t i = 0; i < 8; ++i)
			{
				this->_pins.di.Write( ((data >> i) & 0x01) == true );
				this->_pins.sh.StrobeHigh();
			}
			if(latch == true) this->_pins.st.High();
		}
		
		// Записать байты в регистр.
		void WriteBytes(byte *data, size_t size, bool latch = false)
		{
			if(latch == true) this->_pins.st.Low();
			for(uint8_t i = 0; i < size; ++i)
			{
				this->WriteByte(data[i], false);
			}
			if(latch == true) this->_pins.st.High();
		}
	private:
		struct HC595_t
		{
			DragonIOBasic di;	// Serial data input.
			DragonIOBasic sh;	// Shift register clock input.
			DragonIOBasic st;	// Storage register clock input.
			DragonIOBasic oe;	// Output enable (active LOW).
			DragonIOBasic mr;	// Master reset (active LOW).
		} _pins;
};
