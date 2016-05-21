#include "system.h"

using namespace std;

System sys;

void setup()
{
	Serial.begin(9600);
	sys.init();
}

void loop()
{
	if (Serial.available() > 0)
		sys.event(Serial.read());
}
