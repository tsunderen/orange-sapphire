#include "output.h"

const uint16_t Output::PALETTE[] = {BLACK, MAROON, GREEN, OLIVE, NAVY, PURPLE, TEAL, SILVER,
									GRAY, RED, LIME, YELLOW, BLUE, FUCHSIA, AQUA, WHITE};

Output::Output(int width, int height, const uint8_t* font, int fontX, int fontY)
	: width(width), height(height), font(font), fontX(fontX), fontY(fontY)
{
	buffer = new uint32_t[width * height];
}

Output::~Output()
{
	delete[] buffer;
}

void Output::init()
{
	displayInit();
	clear();
}

void Output::reset()
{
	displayReset();
	clear();
}

void Output::clear()
{
	memset(buffer, 0, width * height * sizeof(uint32_t));
	offset = 0;
	x = y = 0;
	cursorX = cursorY = 0;
	color = LIME;
	background = color8(BLACK);
	::clear();
}

uint32_t* Output::getBufferAddress(int x, int y)
{
	return buffer + ((offset+y) % height) * width + x;
}

const uint8_t* Output::getFontAddress(char ch)
{
	return font + (ch-0x20) * ((fontX+7)/8) * fontY;
}

void Output::setChar(uint32_t* buf, char ch)
{
	*(char*)buf = ch;
}

char Output::getChar(uint32_t* buf)
{
	return *(char*)buf;
}

void Output::setColor(uint32_t* buf, uint16_t color)
{
	*(uint16_t*)((char*)buf+1) = color;
}

uint16_t Output::getColor(uint32_t* buf)
{
	return *(uint16_t*)((char*)buf+1);
}

void Output::setBackground(uint32_t* buf, uint8_t background)
{
	*((char*)buf+3) = background;
}

uint8_t Output::getBackground(uint32_t* buf)
{
	return *((char*)buf+3);
}

void Output::draw(int x, int y)
{
	uint32_t* buf = getBufferAddress(x, y);
	char ch = getChar(buf);
	if (ch)
		drawBitmap(getFontAddress(ch), x*fontX, y*fontY, fontX, fontY, getColor(buf), color16(getBackground(buf)));
	else
		fillRegion(x*fontX, y*fontY, (x+1)*fontX-1, (y+1)*fontY-1, color16(getBackground(buf)));
}

void Output::draw()
{
	for (int y=0;y<height;y++)
		for (int x=0;x<width;x++)
			draw(x, y);
}

void Output::drawCursor(int x, int y)
{
	uint32_t* buf = getBufferAddress(x, y);
	char ch = getChar(buf);
	if (ch)
		drawBitmap(getFontAddress(ch), x*fontX, y*fontY, fontX, fontY, background, color);
	else
		fillRegion(x*fontX, y*fontY, (x+1)*fontX-1, (y+1)*fontY-1, color);
}

void Output::setCursor()
{
	draw(cursorX, cursorY);
	cursorX = x;
	cursorY = y;
	drawCursor(cursorX, cursorY);
}

void Output::moveCursor(int move)
{
	draw(cursorX, cursorY);
	cursorX += move;
	cursorY += cursorX / width;
	cursorX %= width;
	if (cursorX < 0)
	{
		cursorX += width;
		cursorY--;
	}
	drawCursor(cursorX, cursorY);
}

void Output::useCursor()
{
	x = cursorX;
	y = cursorY;
}

void Output::setColor(uint16_t c)
{
	color = c;
}

uint16_t Output::getColor()
{
	return color;
}

void Output::setBackground(uint16_t c)
{
	background = color8(c);
}

void Output::setBackground(uint8_t c)
{
	background = c;
}

uint16_t Output::getBackground()
{
	return color16(background);
}

void Output::print(char ch)
{
	print(ch, color, background);
}

void Output::print(const char* str)
{
	print(str, color, background);
}

void Output::print(char ch, uint16_t color, uint8_t background)
{
	if (x >= width)
	{
		x = 0;
		y++;
	}
	if (y >= height)                //Scroll down the screen
	{
		y--;
		cursorY--;
		offset = (offset+1) % height;
		memset(getBufferAddress(0, y), 0, width * sizeof(uint32_t));
		draw();
	}
	print(ch, x, y, color, background);
	if (ch == '\n')
	{
		x = 0;
		y++;
	}
	else
		x++;
}

void Output::print(const char* str, uint16_t color, uint8_t background)
{
	int length = strlen(str);
	for(int i=0;i<length;i++)
		print(str[i], color, background);
}

void Output::print(char ch, int x, int y, uint16_t color, uint8_t background)
{
	if (!isprint(ch))
		return;
	*getBufferAddress(x, y) = (background << 24) | (color << 8) | ch;
	draw(x, y);
}

void Output::print(const char* str, int x, int y, uint16_t color, uint8_t background)
{
	int length = strlen(str);
	for(int i=0;i<length;i++)
		print(str[i], x+i, y, color, background);
}

int Output::printf(const char* format, ...)
{
	char buf[width * height];
	va_list arg;
	va_start(arg, format);
	int count = vsnprintf(buf, width * height, format, arg);
	va_end(arg);
	print(buf);
	return count;
}

int Output::printfc(const char* format, uint16_t color, ...)
{
	char buf[width * height];
	va_list arg;
	va_start(arg, color);
	int count = vsnprintf(buf, width * height, format, arg);
	va_end(arg);
	print(buf, color, background);
	return count;
}

int Output::printfcb(const char* format, uint16_t color, uint16_t background, ...)
{
	char buf[width * height];
	va_list arg;
	va_start(arg, background);
	int count = vsnprintf(buf, width * height, format, arg);
	va_end(arg);
	print(buf, color, color8(background));
	return count;
}

int Output::printfxy(const char* format, int x, int y, ...)
{
	char buf[width * height];
	va_list arg;
	va_start(arg, y);
	int count = vsnprintf(buf, width * height, format, arg);
	va_end(arg);
	print(buf, x, y, color, background);
	return count;
}

int Output::printfxyc(const char* format, int x, int y, uint16_t color, ...)
{
	char buf[width * height];
	va_list arg;
	va_start(arg, color);
	int count = vsnprintf(buf, width * height, format, arg);
	va_end(arg);
	print(buf, x, y, color, background);
	return count;
}

int Output::printfxycb(const char* format, int x, int y, uint16_t color, uint16_t background, ...)
{
	char buf[width * height];
	va_list arg;
	va_start(arg, background);
	int count = vsnprintf(buf, width * height, format, arg);
	va_end(arg);
	print(buf, x, y, color, color8(background));
	return count;
}

Output& Output::operator<<(int var)
{
	printf("%d", var);
	return *this;
}

Output& Output::operator<<(unsigned int var)
{
	printf("%u", var);
	return *this;
}

Output& Output::operator<<(double var)
{
	printf("%g", var);
	return *this;
}

Output& Output::operator<<(char var)
{
	printf("%c", var);
	return *this;
}

Output& Output::operator<<(const char* var)
{
	printf("%s", var);
	return *this;
}

Output& Output::operator<<(void* var)
{
	printf("%p", var);
	return *this;
}
