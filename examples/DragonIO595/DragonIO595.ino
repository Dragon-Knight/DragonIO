#include <DragonIO595.h>

DragonIO595 r1(5, 6, 7, 8, 9);
//DragonIO pin1(13);

void setup()
{
	
	r1.Reset();
	
	byte b = 0xff;
	byte c[] = {0x44, 0x55, 0x66};
	r1.WriteByte(b);
	r1.WriteBytes(c, sizeof(c));
	/*pin1.InputPullup();
	pin1.Output();
	pin1.Output(true);
	pin1.Read();
	pin1.High();
	pin1.Low();
	pin1.Toggle();
	pin1.StrobeHigh();
	pin1.StrobeLow();
	*/
	
	return;
}

void loop()
{
	return;
}
