#include <DragonIO.h>

DragonIO pin1(&PINB, PB5);
//DragonIO pin1(13);

//DragonIO pin2(&PIND, PD2);
DragonIO pin2(2);

void setup()
{
	//pin1.Input();
	//pin1.InputPullup();
	pin1.Output();
	/*pin1.Output(true);
	pin1.RegCallback(pin1_int, DragonIO::TYPE_CHANGE);
	pin1.Read();
	pin1.High();
	pin1.High(1500);
	pin1.Low();
	pin1.Low(1250);
	pin1.Toggle();
	pin1.Blink(1000, 250);
	
	pin2.Input();
	pin2.InputPullup();
	pin2.Output();
	pin2.Output(true);
	pin2.RegCallback(pin2_int, DragonIO::TYPE_FALLING);
	pin2.Read();
	pin2.High();
	pin2.High(1234);
	pin2.Low();
	pin2.Low(3210);
	pin2.Toggle();
	pin2.Blink(500, 500);*/
	
	return;
}

void loop()
{
	//uint32_t ctime = millis();
	
	//pin1.Processing(ctime);
	//pin2.Processing(ctime);
	
	pin1.Strobe();
	//pin1.High();
	//pin1.Low();
	
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
