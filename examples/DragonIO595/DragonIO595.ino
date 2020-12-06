#include <DragonIO595.h>

DragonIO595 r1(5, 6, 7, 8, 9);
//DragonIO pin1(13);

void setup()
{
	
	r1.Reset();
	r1.Enable(true);
	
	byte b = 0xff;
	byte c[] = {0x44, 0x55, 0x66};
	r1.Write(b, true);
	
	r1.Write(0x01, true); delay(250);
	r1.Write(0x02, true); delay(250);
	r1.Write(0x04, true); delay(250);
	r1.Write(0x08, true); delay(250);
	r1.Write(0x10, true); delay(250);
	r1.Write(0x20, true); delay(250);
	r1.Write(0x40, true); delay(250);
	r1.Write(0x80, true); delay(250);
	
	r1.Write(c, sizeof(c), true);
	
	
	return;
}

void loop()
{
	return;
}
