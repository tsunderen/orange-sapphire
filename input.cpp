#include "input.h"

Input::Input()
	: cursor(0), length(0)
{
	memset(buffer, 0, SIZE);
}

void Input::clear()
{
	memset(buffer, 0, SIZE);
	cursor = 0;
	length = 0;
}

const char* Input::get()
{
	return buffer;
}

int Input::getLength()
{
	return length;
}

int Input::getCursor()
{
	return cursor;
}

bool Input::setCursor(int newCursor)
{
	if (newCursor < 0 || newCursor > length)
		return false;
	cursor = newCursor;
	return true;
}

bool Input::moveCursor(int offset)
{
	return setCursor(cursor + offset);
}

bool Input::read(char code)
{
	if (length >= SIZE - 1)
		return false;
	for (int i=length;i>cursor;i--)
		buffer[i] = buffer[i-1];
	buffer[cursor] = code;
	cursor++;
	length++;
	return true;
}

bool Input::erase()
{
	if (cursor == 0)
		return false;
	for (int i=cursor;i<length;i++)
		buffer[i-1] = buffer[i];
	buffer[length-1] = '\0';
	cursor--;
	length--;
	return true;
}

int Input::scanf(const char* format, ...)
{
	va_list arg;
	va_start(arg, format);
	int count = vsscanf(buffer, format, arg);
	va_end(arg);
	return count;
}

namespace std
{
	Input cin;
}
