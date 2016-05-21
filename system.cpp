#include "system.h"

System::System()
{
	clearBuf();
}

void System::clearBuf()
{
	memset(eventBuf, 0, 8);
	eventLen = -1;                              //Disable event mode
}

void System::prompt()
{
	cout<<endl<<">";
	cout.setCursor();
}

void System::init()
{
	cout.init();
	cout<<"Booting..."<<endl;

	disk = false;
	/*
	if (SD.begin(53))
	{
		cout<<"Disk found!\n";
		disk = true;
	}
	else
		cout.printfc("Disk not found!\n", RED);
	*/

	cout<<endl;
	cout.printfc("Orange ", color16(255, 128, 0));
	cout.printfc("Sapphire ", color16(0, 0, 255));
	cout.printfcb("%d", BLACK, WHITE, 19895);

	prompt();
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
	}
	else if (isprint(code))
	{
		if (cin.read(code))
		{
			cout.setOutput();
			cout<<cin.get()+cin.getCursor()-1;
			cout.moveCursor(1);
		}
	}
	else if (code == 13)                         //Enter
	{
		cout<<endl;
		exec(cin.get());
		cin.clear();
		prompt();
	}
	else if (code == 127)						//Backspace
	{
		if (cin.erase())
		{
			cout.moveCursor(-1);
			cout.setOutput();
			cout<<cin.get()+cin.getCursor();
			cout<<" ";
			cout.moveCursor(0);
		}
	}
	else if (code == 27)						//Escape control characters
		eventLen = 0;                           //Enable event mode
}

void System::exec(const char* command)
{
	char cmd[256];
	strcpy(cmd, command);
	int argc = 0;
	char* argv[8];
	char* pch = strtok(cmd, " ");
	while (argc < 8 && pch != NULL)
	{
		argv[argc] = strdup(pch);
		pch = strtok(NULL, " ");
		argc++;
	}
	Expr expr(argv[0]);
	Result result = expr.eval();
	if (result.error)
		cout.printfc("ERROR: %d\n", RED, result.error);
	else
		cout.printf("%.10g\n", result.value);
	for (int i=0;i<argc;i++)
		free(argv[i]);
}
