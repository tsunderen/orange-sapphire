#include "output.h"

static inline uint16_t getBit(uint32_t data, unsigned int bit)
{
	return (data >> bit) & 1;
}

static inline uint16_t getBit(uint32_t data, unsigned int bit, unsigned int len)
{
	return (data >> bit) & ((1 << len) - 1);
}

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
	x = 0;
	y = 0;
	offset = 0;
	staticX = 0;
	staticY = 0;
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
		uint32_t ch = buffer[((offset+line)%height)*width+x];
		uint16_t background = color16(getBit(ch, 24, 8));
		if (!getBit(ch, 0, 8))
		{
			fillRegion(x*fontX, line*fontY, (x+1)*fontX-1, (line+1)*fontY-1, background);
			continue;
		}
		uint16_t color = getBit(ch, 8, 16);
		const uint8_t* bitmap = font + (getBit(ch, 0, 8)-0x20) * ((fontX+7)/8) * fontY;
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
	this->x = x;
}

void Output::setY(int y)
{
	if (y < 0 || y >= height)
		return;
	this->y = y;
}

int Output::getX()
{
	return x;
}

int Output::getY()
{
	return y;
}

void Output::setStaticCur(int x, int y)
{
	setStaticX(x);
	setStaticY(y);
}

void Output::setStaticX(int x)
{
	if (x < 0 || x >= width)
		return;
	staticX = x;
}

void Output::setStaticY(int y)
{
	if (y < 0 || y >= height)
		return;
	staticY = y;
}

int Output::getStaticX()
{
	return staticX;
}

int Output::getStaticY()
{
	return staticY;
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
		staticY--;
		offset = (offset+1) % height;
		memset(buffer + ((offset+y)%height) * width, 0, width * sizeof(uint32_t));
		redraw();
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
