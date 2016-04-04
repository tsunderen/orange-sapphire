#include "output.h"

const uint16_t Output::PALETTE[] = {BLACK, MAROON, GREEN, OLIVE, NAVY, PURPLE, TEAL, SILVER,
									GRAY, RED, LIME, YELLOW, BLUE, FUCHSIA, AQUA, WHITE};

Output::Output(int width, int height, const uint8_t* font, int fontX, int fontY)
	: width(width), height(height), font(font), fontX(fontX), fontY(fontY)
{
	buffer = new uint32_t[width * height];
	curMain = Cursor(width, height);
	curHead = Cursor(width, height);
	curInput = Cursor(width, height);
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
	curMain.clear();
	curHead.clear();
	curInput.clear();
	color = LIME;
	background = color8(BLACK);
	::clear();
}

void Output::redraw()
{
	for (int y=0;y<height;y++)
		redraw(y);
}

void Output::redraw(int line)
{
	for (int x=0;x<width;x++)
	{
		uint32_t* ch = &buffer[((offset+line)%height)*width+x];
		uint16_t background = color16(*((char*)ch+3));
		if (!(*(char*)ch))
		{
			fillRegion(x*fontX, line*fontY, (x+1)*fontX-1, (line+1)*fontY-1, background);
			continue;
		}
		uint16_t color = *(uint16_t*)((char*)ch+1);
		const uint8_t* bitmap = font + (*(char*)ch-0x20) * ((fontX+7)/8) * fontY;
		drawBitmap(bitmap, x*fontX, line*fontY, fontX, fontY, color, background);
	}
}

void Output::setCur(int x, int y)
{
	setX(x);
	setY(y);
}

void Output::setX(int x)
{
	if (x < 0 || x >= width)
		return;
	curMain.x = x;
}

void Output::setY(int y)
{
	if (y < 0 || y >= height)
		return;
	curMain.y = y;
}

int Output::getX()
{
	return curMain.x;
}

int Output::getY()
{
	return curMain.y;
}

void Output::setHeadCursor()
{
	curHead = curMain;
	curInput = curHead;
}

void Output::setInputCursor(int offset)
{
	curInput = curHead;
	curInput += offset;
}

void Output::useHeadCursor()
{
	curMain = curHead;
}

void Output::useInputCursor()
{
	curMain = curInput;
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
	if (curMain.x >= width)
	{
		curMain.x = 0;
		curMain.y++;
	}
	if (curMain.y >= height)                //Scroll down the screen
	{
		curMain.y--;
		curHead.y--;
		curInput.y--;
		offset = (offset+1) % height;
		memset(buffer + ((offset+curMain.y)%height) * width, 0, width * sizeof(uint32_t));
		redraw();
	}
	print(ch, curMain.x, curMain.y, color, background);
	if (ch == '\n')
	{
		curMain.x = 0;
		curMain.y++;
	}
	else
		curMain.x++;
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
	buffer[((offset+y)%height)*width+x] = (background << 24) | (color << 8) | ch;
	const uint8_t* bitmap = font + (ch-0x20) * ((fontX+7)/8) * fontY;
	drawBitmap(bitmap, x*fontX, y*fontY, fontX, fontY, color, color16(background));
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
