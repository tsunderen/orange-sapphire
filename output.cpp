#include "output.h"

template<unsigned int W, unsigned int H>
Output<W, H>::Output(const uint8_t* font, int fontX, int fontY)
	: font(font), fontX(fontX), fontY(fontY)
{
}

template<unsigned int W, unsigned int H>
void Output<W, H>::init()
{
	displayInit();
	color = LIME;
	background = color8(BLACK);
	clear();
}

template<unsigned int W, unsigned int H>
void Output<W, H>::clear()
{
	memset(buffer, 0, W * H * sizeof(uint32_t));
	offset = 0;
	x = y = 0;
	cursorX = cursorY = 0;
	::clear(color16(background));
}

template<unsigned int W, unsigned int H>
uint32_t* Output<W, H>::getBufferAddress(int x, int y)
{
	return buffer + ((offset+y) % H) * W + x;
}

template<unsigned int W, unsigned int H>
const uint8_t* Output<W, H>::getFontAddress(char ch)
{
	return font + (ch-0x20) * ((fontX+7)/8) * fontY;
}

template<unsigned int W, unsigned int H>
void Output<W, H>::setChar(uint32_t* buf, char ch)
{
	*(char*)buf = ch;
}

template<unsigned int W, unsigned int H>
char Output<W, H>::getChar(uint32_t* buf)
{
	return *(char*)buf;
}

template<unsigned int W, unsigned int H>
void Output<W, H>::setColor(uint32_t* buf, uint16_t color)
{
	*(uint16_t*)((char*)buf+1) = color;
}

template<unsigned int W, unsigned int H>
uint16_t Output<W, H>::getColor(uint32_t* buf)
{
	return *(uint16_t*)((char*)buf+1);
}

template<unsigned int W, unsigned int H>
void Output<W, H>::setBackground(uint32_t* buf, uint8_t background)
{
	*((char*)buf+3) = background;
}

template<unsigned int W, unsigned int H>
uint8_t Output<W, H>::getBackground(uint32_t* buf)
{
	return *((char*)buf+3);
}

template<unsigned int W, unsigned int H>
void Output<W, H>::draw(int x, int y)
{
	uint32_t* buf = getBufferAddress(x, y);
	char ch = getChar(buf);
	if (ch)
		drawBitmap(getFontAddress(ch), x*fontX, y*fontY, fontX, fontY, getColor(buf), color16(getBackground(buf)));
	else
		fillRegion(x*fontX, y*fontY, (x+1)*fontX-1, (y+1)*fontY-1, color16(getBackground(buf)));
}

template<unsigned int W, unsigned int H>
void Output<W, H>::drawInverse(int x, int y)
{
	uint32_t* buf = getBufferAddress(x, y);
	char ch = getChar(buf);
	if (ch)
		drawBitmap(getFontAddress(ch), x*fontX, y*fontY, fontX, fontY, background, color);
	else
		fillRegion(x*fontX, y*fontY, (x+1)*fontX-1, (y+1)*fontY-1, color);
}

template<unsigned int W, unsigned int H>
void Output<W, H>::draw()
{
	for (int y=0;y<H;y++)
		for (int x=0;x<W;x++)
			draw(x, y);
}

template<unsigned int W, unsigned int H>
void Output<W, H>::setCursor()
{
	draw(cursorX, cursorY);
	cursorX = x;
	cursorY = y;
	drawInverse(cursorX, cursorY);
}

template<unsigned int W, unsigned int H>
void Output<W, H>::setOutput()
{
	x = cursorX;
	y = cursorY;
}

template<unsigned int W, unsigned int H>
void Output<W, H>::moveCursor(int move)
{
	draw(cursorX, cursorY);
	cursorX += move;
	cursorY += cursorX / W;
	cursorX %= W;
	if (cursorX < 0)
	{
		cursorX += W;
		cursorY--;
	}
	drawInverse(cursorX, cursorY);
}

template<unsigned int W, unsigned int H>
void Output<W, H>::setColor(uint16_t c)
{
	color = c;
}

template<unsigned int W, unsigned int H>
uint16_t Output<W, H>::getColor()
{
	return color;
}

template<unsigned int W, unsigned int H>
void Output<W, H>::setBackground(uint16_t c)
{
	background = color8(c);
}

template<unsigned int W, unsigned int H>
void Output<W, H>::setBackground(uint8_t c)
{
	background = c;
}

template<unsigned int W, unsigned int H>
uint16_t Output<W, H>::getBackground()
{
	return color16(background);
}

template<unsigned int W, unsigned int H>
void Output<W, H>::print(char ch, uint16_t color, uint8_t background)
{
	if (x >= W)
	{
		x = 0;
		y++;
	}
	if (y >= H)                //Scroll down the screen
	{
		y--;
		cursorY--;
		offset = (offset+1) % H;
		memset(getBufferAddress(0, y), 0, W * sizeof(uint32_t));
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

template<unsigned int W, unsigned int H>
void Output<W, H>::print(const char* str, uint16_t color, uint8_t background)
{
	int length = strlen(str);
	for(int i=0;i<length;i++)
		print(str[i], color, background);
}

template<unsigned int W, unsigned int H>
void Output<W, H>::print(char ch, int x, int y, uint16_t color, uint8_t background)
{
	if (!isprint(ch))
		return;
	*getBufferAddress(x, y) = (background << 24) | (color << 8) | ch;
	draw(x, y);
}

template<unsigned int W, unsigned int H>
void Output<W, H>::print(const char* str, int x, int y, uint16_t color, uint8_t background)
{
	int length = strlen(str);
	for(int i=0;i<length;i++)
		print(str[i], x+i, y, color, background);
}

template<unsigned int W, unsigned int H>
int Output<W, H>::printf(const char* format, ...)
{
	char buf[W * H];
	va_list arg;
	va_start(arg, format);
	int count = vsnprintf(buf, W * H, format, arg);
	va_end(arg);
	print(buf, color, background);
	return count;
}

template<unsigned int W, unsigned int H>
int Output<W, H>::printfc(const char* format, uint16_t color, ...)
{
	char buf[W * H];
	va_list arg;
	va_start(arg, color);
	int count = vsnprintf(buf, W * H, format, arg);
	va_end(arg);
	print(buf, color, background);
	return count;
}

template<unsigned int W, unsigned int H>
int Output<W, H>::printfcb(const char* format, uint16_t color, uint16_t background, ...)
{
	char buf[W * H];
	va_list arg;
	va_start(arg, background);
	int count = vsnprintf(buf, W * H, format, arg);
	va_end(arg);
	print(buf, color, color8(background));
	return count;
}

template<unsigned int W, unsigned int H>
int Output<W, H>::printfxy(const char* format, int x, int y, ...)
{
	char buf[W * H];
	va_list arg;
	va_start(arg, y);
	int count = vsnprintf(buf, W * H, format, arg);
	va_end(arg);
	print(buf, x, y, color, background);
	return count;
}

template<unsigned int W, unsigned int H>
int Output<W, H>::printfxyc(const char* format, int x, int y, uint16_t color, ...)
{
	char buf[W * H];
	va_list arg;
	va_start(arg, color);
	int count = vsnprintf(buf, W * H, format, arg);
	va_end(arg);
	print(buf, x, y, color, background);
	return count;
}

template<unsigned int W, unsigned int H>
int Output<W, H>::printfxycb(const char* format, int x, int y, uint16_t color, uint16_t background, ...)
{
	char buf[W * H];
	va_list arg;
	va_start(arg, background);
	int count = vsnprintf(buf, W * H, format, arg);
	va_end(arg);
	print(buf, x, y, color, color8(background));
	return count;
}

template<unsigned int W, unsigned int H>
Output<W, H>& Output<W, H>::operator<<(int var)
{
	printf("%d", var);
	return *this;
}

template<unsigned int W, unsigned int H>
Output<W, H>& Output<W, H>::operator<<(unsigned int var)
{
	printf("%u", var);
	return *this;
}

template<unsigned int W, unsigned int H>
Output<W, H>& Output<W, H>::operator<<(double var)
{
	printf("%g", var);
	return *this;
}

template<unsigned int W, unsigned int H>
Output<W, H>& Output<W, H>::operator<<(char var)
{
	printf("%c", var);
	return *this;
}

template<unsigned int W, unsigned int H>
Output<W, H>& Output<W, H>::operator<<(const char* var)
{
	printf("%s", var);
	return *this;
}

template<unsigned int W, unsigned int H>
Output<W, H>& Output<W, H>::operator<<(void* var)
{
	printf("%p", var);
	return *this;
}

template class Output<WIDTH/10, HEIGHT/20>;
namespace std
{
	Output<WIDTH/10, HEIGHT/20> cout(TERMINAL20, 10, 20);
	const char* endl = "\n";
}
