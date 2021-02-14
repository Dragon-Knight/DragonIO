#include <DragonIOEx.h>

//DragonIOEx pin1;
//DragonIOEx pin1(&PINB, PB5);
DragonIOEx pin1(13);

void setup()
{
	pin1.SetPort(&PINB, PB5);
	pin1.SetPin(13);
	pin1.Input();
	pin1.InputPullup();
	pin1.Output();
	pin1.Output(true);
	pin1.Read();
	pin1.Write(true);
	pin1.Write(false);
	pin1.High();
	pin1.Low();
	pin1.Raw();
	pin1.Raw(0b10101010);
	pin1.Toggle();
	pin1.StrobeHigh();
	pin1.StrobeLow();
	
	pin1.RegCallback(onPin, DragonIOEx::TYPE_CHANGE);
	pin1.HighDelay(500);
	pin1.LowDelay(1000);
	pin1.Blink(10, 350);
	pin1.Stop();
	
	return;
}

void loop()
{
	uint32_t time = millis();
	
	pin1.Processing(time);
	
	return;
}


void onPin(uint8_t pin, DragonIOEx::callback_type_t type)
{
	
}
