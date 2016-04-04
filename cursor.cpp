#include "cursor.h"

Cursor::Cursor()
{
}

Cursor::Cursor(int width, int height)
	: width(width), height(height)
{
	x = 0;
	y = 0;
}

void Cursor::clear()
{
	x = 0;
	y = 0;
}

Cursor& Cursor::operator+=(int offset)
{
	x += offset;
	if (x >= width)
	{
		y += x / width;
		x = x % width;
	}
	return *this;
}
