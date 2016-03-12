#include "display.h"
#include "expr.h"

const int VERSION = 19892;

void setup() {
	Serial.setTimeout(30000);
	Serial.begin(9600);
	displayInit();
	clear();
	print("Orange", 0, 0, getColor(255, 128, 0));
	print("Sapphire", 7*FONT_X, 0, getColor(0, 0, 255));
	print(VERSION, 16*FONT_X, 0, BLACK, WHITE);
}

void loop() {
	int line = 1;
	char command[256];
	while (command != "exit")
	{
		memset(command, 0, 256);		
		Serial.print(">");
		print(">", 0, line*FONT_Y, LIME);
		Serial.readBytesUntil('\n', command, 256);
		Serial.println(command);
		print(command, FONT_X, line*FONT_Y, LIME);
		line++;
		Expr expr(command);
		Result result = expr.eval();
		if (result.error)
		{
			Serial.print("ERROR: ");
			Serial.println(result.error);
			print("ERROR: ", 0, line*FONT_Y, RED);
			print(result.error, 7*FONT_X, line*FONT_Y, RED);
			line++;
		}
		else
		{
			Serial.println(result.value);
			print(result.value, 0, line*FONT_Y, LIME);
			line++;
		}
	}
}
