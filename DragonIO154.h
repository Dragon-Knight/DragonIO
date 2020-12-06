/*
 * DragonIO154.h
 * Fast library for 74HC154 chip.
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

class DragonIO154
{
	public:
		DragonIO154(volatile uint8_t *port)
		{
			_pins.port.SetPort(port, 8);
			_pins.port.Output();
			
			_use_port = true;
			
			return;
		}
		
		DragonIO154(uint8_t pin_a, uint8_t pin_b, uint8_t pin_c, uint8_t pin_d, uint8_t pin_e1, uint8_t pin_e2, uint8_t pin_e3, uint8_t pin_e4)
		{
			_pins.id.SetPin(pin_d);
			_pins.id.Output(IO_LOW);
			
			_pins.ic.SetPin(pin_c);
			_pins.ic.Output(IO_LOW);
			
			_pins.ib.SetPin(pin_b);
			_pins.ib.Output(IO_LOW);
			
			_pins.ia.SetPin(pin_a);
			_pins.ia.Output(IO_LOW);
			
			_pins.e4.SetPin(pin_e4);
			_pins.e4.Output(IO_HIGH);
			
			_pins.e3.SetPin(pin_e3);
			_pins.e3.Output(IO_HIGH);
			
			_pins.e2.SetPin(pin_e2);
			_pins.e2.Output(IO_HIGH);
			
			_pins.e1.SetPin(pin_e1);
			_pins.e1.Output(IO_HIGH);
			
			_use_port = false;
			
			return;
		}
		
		// Сброс демультиплексоров.
		void Reset()
		{
			return;
		}
		
		// Активирует выход демультиплексора. Прозрачно 4 шт : 35 = 3 микросхема, 5 порт.
		void Select(uint8_t out)
		{
			if(_use_port == true)
			{
				uint8_t raw = (out < 64) ? (((out % 16) << 4) | 1 << (out / 16)) : 0x00;
				_pins.port.Raw(raw);
			}
			else
			{
				uint8_t ic_pins = (out % 16);
				uint8_t ic_addr = (out / 16);
				
				_pins.id.Write( (ic_pins >> 3) & 0x01 );
				_pins.ic.Write( (ic_pins >> 2) & 0x01 );
				_pins.ib.Write( (ic_pins >> 1) & 0x01 );
				_pins.ia.Write( (ic_pins >> 0) & 0x01 );
				_pins.e4.Write( (ic_addr == 3) ? 1 : 0 );
				_pins.e3.Write( (ic_addr == 2) ? 1 : 0 );
				_pins.e2.Write( (ic_addr == 1) ? 1 : 0 );
				_pins.e1.Write( (ic_addr == 0) ? 1 : 0 );
			}
			
			return;
		}
	
	private:
		struct HC154_t
		{
			DragonIO port;	// Port to 4 74165 ( bit map: A, B, C, D, E1, E2, E3, E4 ).
			
			DragonIO id;	// A input.
			DragonIO ic;	// B input.
			DragonIO ib;	// C input.
			DragonIO ia;	// D input.
			DragonIO e4;	// Enable chip 1 input.
			DragonIO e3;	// Enable chip 2 input.
			DragonIO e2;	// Enable chip 3 input.
			DragonIO e1;	// Enable chip 4 input.
		} _pins;
		bool _use_port;
};
