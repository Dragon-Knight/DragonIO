#include <DragonIO.h>

DragonIO pin1(&PIND, PD3);
DragonIO pin2(4);

void setup()
{
	pin1.Begin(DragonIO::MODE_INPUT);
	pin1.RegCallback(pin1_int, DragonIO::TYPE_CHANGE);
	pin1.Read();
	pin1.High();
	pin1.Low();
	pin1.Toggle();
	pin1.Processing();
	
	pin2.Begin(DragonIO::MODE_INPUT);
	pin2.RegCallback(pin2_int, DragonIO::TYPE_FALLING);
	pin2.Read();
	pin2.High();
	pin2.Low();
	pin2.Toggle();
	pin2.Processing();
	
	return;
}

void loop()
{
	return;
}


void pin1_int(uint8_t pin, DragonIO::callback_type_t type)
{
	return;
}


void pin2_int(uint8_t pin, DragonIO::callback_type_t type)
{
	return;
}