#include "input.h"

Input::Input(int size)
	: size(size), cursor(0), length(0), end(false)
{
	buffer = new char[size];
	memset(buffer, 0, size);
}

Input::~Input()
{
	delete[] buffer;
}

void Input::clear()
{
	memset(buffer, 0, size);
	cursor = 0;
	length = 0;
	end = false;
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
	if (length >= size - 1)
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

bool Input::eof()
{
	return end;
}

int Input::scanf(const char* format, ...)
{
	va_list arg;
	va_start(arg, format);
	int count = vsscanf(buffer, format, arg);
	va_end(arg);
	return count;
}
