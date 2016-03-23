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

int Input::getCursor()
{
	return cursor;
}

int Input::getLength()
{
	return length;
}

bool Input::read()
{
	int code = Serial.read();
	if (code == 0 || code == 13)            //End-of-line
	{
		end = true;
		return true;
	}
	if (code == -1 || length >= size-1)     //No input or overflow
		return false;
    if (isprint(code))
    {
    	for (int i=length;i>cursor;i--)
			buffer[i] = buffer[i-1];
		buffer[cursor] = code;
		cursor++;
		length++;
		return true;
	}
	if (code == 127)						//Backspace
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
	if (code == 9)							//Tab
	{
		return false;
	}
	if (code == 27)							//Escaped character
	{
		return true;
	}
	return false;
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
