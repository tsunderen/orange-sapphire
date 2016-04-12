#include "system.h"

const int VERSION = 19894;
Input cin(256);
Output cout(WIDTH/10, HEIGHT/20, TERMINAL20, 10, 20);
System sys(cin, cout);

void setup()
{
	int timeStart = micros();

	cout.init();
	cout<<(micros()-timeStart)/1000000.0<<": Screen initialized!\n";

	Serial.setTimeout(8);
	Serial.begin(9600);
	cout<<(micros()-timeStart)/1000000.0<<": Serial initialized!\n";

	cout<<(micros()-timeStart)/1000000.0<<": Boot success!\n\n";

	cout.printfc("Orange ", color16(255, 128, 0));
	cout.printfc("Sapphire ", color16(0, 0, 255));
	cout.printfcb("%d", BLACK, WHITE, VERSION);
	cout<<"\n";

	sys.prompt();
}

void benchmark()
{
	int timeStart = micros();
	for(int i=0;i<15;i++)
		cout<<"ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()~\n";
	cout<<(micros()-timeStart)/1000.0<<"ms\n";
}

void loop()
{
	int code = Serial.read();
	if (code != -1)
		sys.event(code);
}
