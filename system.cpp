#include "system.h"

System::System(Input& in, Output& out)
	: cin(in), cout(out)
{
	clearBuf();
}

void System::clearBuf()
{
	memset(eventBuf, 0, 8);
	eventLen = -1;                              //Disable event mode
}

void System::event(char code)
{
	if (eventLen >= 0)                          //Event mode
	{
		eventBuf[eventLen] = code;
		eventLen++;
		if (strcmp(eventBuf, "[D") == 0)				//Left
		{
			if (cin.moveCursor(-1))
				cout.moveCursor(-1);
			clearBuf();
		}
		else if (strcmp(eventBuf, "[C") == 0)			//Right
		{
			if (cin.moveCursor(1))
				cout.moveCursor(1);
			clearBuf();
		}
		else if (strcmp(eventBuf, "[1~") == 0)			//Home
		{
			int cursor = cin.getCursor();
			cin.setCursor(0);
			cout.moveCursor(-cursor);
			clearBuf();
		}
		else if (strcmp(eventBuf, "[4~") == 0)			//End
		{
			int cursor = cin.getCursor();
			int length = cin.getLength();
			cin.setCursor(length);
			cout.moveCursor(length-cursor);
			clearBuf();
		}
		else if (eventLen == 8)
			clearBuf();
		return;
	}
	if (code == 13)                             //Enter
	{
		cout<<"\n";
		exec(cin.get());
		cin.clear();
		prompt();
	}
	if (isprint(code))
	{
		if (cin.read(code))
		{
			cout.useCursor();
			cout<<cin.get()+cin.getCursor()-1;
			cout.moveCursor(1);
		}
	}
	else if (code == 127)						//Backspace
	{
		if (cin.erase())
		{
			cout.moveCursor(-1);
			cout.useCursor();
			cout<<cin.get()+cin.getCursor();
			cout<<" ";
			cout.moveCursor(0);
		}
	}
	else if (code == 27)						//Escape control characters
		eventLen = 0;                           //Enable event mode
}

void System::prompt()
{
	cout<<"\n>";
	cout.setCursor();
}

void System::exec(const char* cmd)
{
	char exp[256];
	strcpy(exp, cmd);
	Expr expr(exp);
	Result result = expr.eval();
	if (result.error)
		cout.printfc("ERROR: %d\n", RED, result.error);
	else
		cout.printf("%.10g\n", result.value);
}

