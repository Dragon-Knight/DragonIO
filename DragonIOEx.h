/*
 * DragonIOEx.h
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

#include "DragonIO.h"

class DragonIOEx : public DragonIO
{
	public:
		enum mode_t { MODE_NORMAL, MODE_DELAY, MODE_BLINK };
		enum callback_type_t { TYPE_NONE, TYPE_LOW, TYPE_HIGH, TYPE_CHANGE, TYPE_RISING, TYPE_FALLING };
		using callback_t = void (*)(uint8_t pin, callback_type_t type);
		
		// Конструктор без параметров.
		DragonIOEx() : DragonIO()
		{
			return;
		}
		
		// Конструктор с регистрами, например DragonIOEx(&PIND, PD3);
		DragonIOEx(volatile uint8_t *port, uint8_t pin) : DragonIO(port, pin)
		{
			return;
		}
		
		// Конструктор с пином, например DragonIOEx(3);
		DragonIOEx(uint8_t pin) : DragonIO(pin)
		{
			return;
		}
		
		// Регистрация псевдопрерывания для пина ( INPUT, INPUT_PULLUP ).
		void RegCallback(callback_t callback, callback_type_t type)
		{
			_data_ex.callback = callback;
			_data_ex.callback_type = type;
		}
		
		// Назначить пин на выход и установить начальное состояние.
		void Output(bool state = IO_LOW)
		{
			DragonIO::Output(state);
			_data_ex.state_default = state;
			
			return;
		}
		
		// Прочитать состояние пина ( INPUT, INPUT_PULLUP ).
		bool Read()
		{
			_data_ex.state_old = _data_ex.state_new;
			_data_ex.state_new = DragonIO::Read();
			
			return _data_ex.state_new;
		}
		
		// Записать высокий уровень в пин ( OUTPUT ).
		void High()
		{
			DragonIO::High();
			
			_data_ex.state_old = _data_ex.state_new;
			_data_ex.state_new = IO_HIGH;
			
			return;
		}
		
		// Записать низкий уровень в пин ( OUTPUT ).
		void Low()
		{
			DragonIO::Low();
			
			_data_ex.state_old = _data_ex.state_new;
			_data_ex.state_new = IO_LOW;
			
			return;
		}
		
		// Инвертировать состояние пина ( OUTPUT ) и вернуть новое состояние.
		bool Toggle()
		{
			(_data_ex.state_new == IO_HIGH) ? Low() : High();
			
			return _data_ex.state_new;
		}
		
		// Записать высокий уровень в пин ( OUTPUT ) на указанное время.
		void HighDelay(uint16_t time)
		{
			DragonIO::High();
			
			_data_ex.delay_time = millis() + time;
			_data_ex.mode = MODE_DELAY;
			
			return;
		}
		
		// Записать низкий уровень в пин ( OUTPUT ) на указанное время.
		void LowDelay(uint16_t time)
		{
			DragonIO::Low();
			
			_data_ex.delay_time = millis() + time;
			_data_ex.mode = MODE_DELAY;
			
			return;
		}
		
		// Запустить циклическое ВКЛ-ВЫКЛ пина.
		void Blink(uint16_t time_on, uint16_t time_off)
		{
			_data_ex.blinkon_time = time_on;
			_data_ex.blinkoff_time = time_off;
			_data_ex.mode = MODE_BLINK;
			
			return;
		}
		
		// Остановить мигание и сбросить задержку уровня.
		void Stop()
		{
			DragonIO::Write(_data_ex.state_default);
			_data_ex.mode = MODE_NORMAL;
			
			return;
		}
		
		// Псевдопрерывание для работы колбеков ( INPUT, INPUT_PULLUP ).
		void Processing(uint32_t time = millis())
		{
			if(_data_ex.mode != MODE_NORMAL)
			{
				if(_data_ex.delay_time < time)
				{
					bool state = Toggle();
					
					switch(_data_ex.mode)
					{
						case MODE_DELAY:
						{
							_data_ex.mode = MODE_NORMAL;
							
							break;
						}
						case MODE_BLINK:
						{
							_data_ex.delay_time = ((state == IO_HIGH) ? _data_ex.blinkon_time : _data_ex.blinkoff_time) + time;
							
							break;
						}
						default:
						{
							break;
						}
					}
				}
			}
			
			if(_data_ex.callback_type != TYPE_NONE)
			{
				DragonIO::Read();
				
				bool is_run = false;
				switch(_data_ex.callback_type)
				{
					case TYPE_LOW:
					{
						if(_data_ex.state_new == IO_LOW){ is_run = true; break; }
					}
					case TYPE_HIGH:
					{
						if(_data_ex.state_new == IO_HIGH){ is_run = true; break; }
					}
					case TYPE_CHANGE:
					{
						if(_data_ex.state_new != _data_ex.state_old){ is_run = true; break; }
					}
					case TYPE_RISING:
					{
						if(_data_ex.state_old == IO_LOW && _data_ex.state_new == IO_HIGH){ is_run = true; break; }
					}
					case TYPE_FALLING:
					{
						if(_data_ex.state_old == IO_HIGH && _data_ex.state_new == IO_LOW){ is_run = true; break; }
					}
					default:
					{
						break;
					}
				}
				if(is_run == true)
				{
					_data_ex.callback(_data.pin, _data_ex.callback_type);
				}
			}
			
			return;
		}
	
	protected:
	
	private:
		struct data_t
		{
			mode_t mode:2;						// Текущий режим работы: Обычный пин, С задержкой, Мигалка.
			callback_type_t callback_type:5;	// Тип псевдопрерывания.
			uint8_t state_default:1;			// Состояние пина при инициализации.
			uint8_t state_new:1;				// Текущее состояние пина.
			uint8_t state_old:1;				// Предыдущие состояние пина.
			uint8_t __offset:6;					// 
			callback_t callback;				// Колбек псевдопрерывания.
			uint32_t delay_time;				// Время задержки при MODE_DELAY или MODE_BLINK.
			uint16_t blinkon_time;				// Время первой стадии 'мигания'.
			uint16_t blinkoff_time;				// Время второй стадии 'мигания'.
		} _data_ex;
};
