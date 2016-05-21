#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>

class Input
{
	private:
		static const int SIZE = 256;
		char buffer[SIZE];
		int cursor;
		int length;

	public:
		Input();

		void clear();
		const char* get();
		int getLength();
		int getCursor();
		bool setCursor(int newCursor);
		bool moveCursor(int offset);

		bool read(char code);
		bool erase();
		int scanf(const char* format, ...);
};

namespace std
{
	extern Input cin;
}

#endif
