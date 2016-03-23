#include "input.h"
#include "output.h"
#include "expr.h"

const int VERSION = 19893;
Input in(256);
Output out(WIDTH/10, HEIGHT/20, TERMINAL20, 10, 20);

void setup()
{
	int timeStart = micros();

	out.init();
	out.printf("%f: Screen initialized!\n", (micros()-timeStart)/1000000.0);

	Serial.setTimeout(8);
	Serial.begin(9600);
	out.printf("%f: Serial initialized!\n", (micros()-timeStart)/1000000.0);

	out.printf("%f: Boot success!\n\n", (micros()-timeStart)/1000000.0);

	out.printfc("Orange ", color16(255, 128, 0));
	out.printfc("Sapphire ", color16(0, 0, 255));
	out.printfcb("%d", BLACK, WHITE, VERSION);
	out.printf("\n");

	out.printf("\n>");
	out.setStaticCur(out.getX(), out.getY());
}

void loop()
{
	char command[256];
	while (!in.eof())
	{
		if (in.read())
		{
			out.setCur(out.getStaticX(), out.getStaticY());
			out.printf(in.get());
			out.printf(" ");
		}
	}
	in.scanf("%s", command);
	in.clear();
	out.printf("\n");
	Expr expr(command);
	Result result = expr.eval();
	if (result.error)
		out.printfc("ERROR: %d\n", RED, result.error);
	else
		out.printf("%.10g\n", result.value);
	out.printf("\n>");
	out.setStaticCur(out.getX(), out.getY());
}
