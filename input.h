#ifndef INPUT_H
#define INPUT_H

#include "Arduino.h"

class Input
{
	private:
		int size;
		char* buffer;
		int cursor;
		int length;
		bool end;

	public:
		Input(int size);
		~Input();

		void clear();
		const char* get();
		int getLength();
		int getCursor();
		bool setCursor(int newCursor);
		bool moveCursor(int offset);

		bool read(char code);
		bool erase();
		bool eof();
		int scanf(const char* format, ...);
};

#endif
