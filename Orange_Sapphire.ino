#include "input.h"
#include "output.h"
#include "expr.h"

const int VERSION = 19894;
Input cin(256);
Output cout(WIDTH/10, HEIGHT/20, TERMINAL20, 10, 20);

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

	cout<<"\n>";
	cout.setCursor();
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
/*
	benchmark();
	while (1)
		;
*/
	char command[256];
	while (1)
	{
		int code = Serial.read();
		if (code == -1)
			continue;
		if (code == 13)
			break;
		if (isprint(code))
		{
			if (cin.read(code))
			{
				cout.useCursor();
				cout.printf(cin.get()+cin.getCursor()-1);
				cout.moveCursor(1);				
			}
		}
		else if (code == 127)						//Backspace
		{
			if (cin.erase())
			{
				cout.moveCursor(-1);
				cout.useCursor();
				cout.printf(cin.get()+cin.getCursor());
				cout.printf(" ");
				cout.moveCursor(0);				
			}
		}
		else if (code == 27)						//Escape control characters
		{
			char buf[4];
			Serial.readBytes(buf, 4);
			if (buf[0] == 91 && buf[1] == 68)				//Left
			{
				if (cin.moveCursor(-1))
					cout.moveCursor(-1);
			}
			else if (buf[0] == 91 && buf[1] == 67)			//Right
			{
				if (cin.moveCursor(1))
					cout.moveCursor(1);
			}
			else if (buf[0] == 91 && buf[1] == 49 && buf[2] == 126)		//Home
			{
				int cursor = cin.getCursor();
				cin.setCursor(0);
				cout.moveCursor(-cursor);
			}
			else if (buf[0] == 91 && buf[1] == 52 && buf[2] == 126)		//End
			{
				int cursor = cin.getCursor();
				int length = cin.getLength();
				cin.setCursor(length);
				cout.moveCursor(length-cursor);						
			}
		}
	}
	cin.scanf("%s", command);
	cin.clear();
	cout<<"\n";
	Expr expr(command);
	Result result = expr.eval();
	if (result.error)
		cout.printfc("ERROR: %d\n", RED, result.error);
	else
		cout.printf("%.10g\n", result.value);
	cout<<"\n>";
	cout.setCursor();
}
