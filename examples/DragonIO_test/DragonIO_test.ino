#include <DragonIO.h>

DragonIO pin1;
//DragonIO pin1(&PINB, PB5);
//DragonIO pin1(13);

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
	
	return;
}

void loop()
{
	return;
}
