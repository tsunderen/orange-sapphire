#include "display.h"

static inline void setLow(uint32_t pin)
{
	PIOC->PIO_CODR = pin;
}

static inline void setHigh(uint32_t pin)
{
	PIOC->PIO_SODR = pin;
}

static inline uint16_t getBit(uint16_t data, unsigned int bit)
{
	return (data >> bit) & 1;
}

static inline uint16_t getBit(uint16_t data, unsigned int bit, unsigned int len)
{
	return (data >> bit) & ((1 << len) - 1);
}

uint16_t color16(uint8_t red, uint8_t green, uint8_t blue)
{
	return (getBit(red, 3, 5) << 11) | (getBit(green, 2, 6) << 5) | (getBit(blue, 3, 5));
}

uint16_t color16(uint8_t color256)
{
	uint8_t red = getBit(color256, 5, 3);
	uint8_t green = getBit(color256, 2, 3);
	uint8_t blue = getBit(color256, 0, 2);
	return (red << 13) | (getBit(red, 1, 2) << 11) | (green << 8) | (green << 5) | (blue << 3) | (blue << 1) | getBit(blue, 1);
}

uint8_t color8(uint8_t red, uint8_t green, uint8_t blue)
{
	return (getBit(red, 5, 3) << 5) | (getBit(green, 5, 3) << 2) | (getBit(blue, 6, 2));
}

uint8_t color8(uint16_t color)
{
	return (getBit(color, 13, 3) << 5) | (getBit(color, 8, 3) << 2) | (getBit(color, 3, 2));
}

static inline void displayInitGPIO()
{
	//Enable PIO
	REG_PIOA_PER = PIOA_MASK;
	REG_PIOB_PER = PIOB_MASK;
	REG_PIOC_PER = PIOC_MASK | PIOC_MASK_CTRL;
	REG_PIOD_PER = PIOD_MASK;
	//Enable output
	REG_PIOA_OER = PIOA_MASK;
	REG_PIOB_OER = PIOB_MASK;
	REG_PIOC_OER = PIOC_MASK | PIOC_MASK_CTRL;
	REG_PIOD_OER = PIOD_MASK;
	//Disable pull-up
	REG_PIOA_PUDR = PIOA_MASK;
	REG_PIOB_PUDR = PIOB_MASK;
	REG_PIOC_PUDR = PIOC_MASK | PIOC_MASK_CTRL;
	REG_PIOD_PUDR = PIOD_MASK;
	//Set control signal
	setHigh(LCD_WRX);
	setLow(LCD_CSX);
}

void displayReset()
{
	setLow(LCD_RESX);
	setHigh(LCD_RESX);
}

void displayInit()
{
	displayInitGPIO();
	displayReset();

	//Screen initialization
	displayCommand(0x11);
	displayCommand(0xD0, 3, 0x07, 0x42, 0x18);
	displayCommand(0xD1, 3, 0x00, 0x07, 0x10);
	displayCommand(0xD2, 2, 0x01, 0x02);
	displayCommand(0xC0, 5, 0x10, 0x3B, 0x00, 0x02, 0x11);
	displayCommand(0xC5, 1, 0x03);
	displayCommand(0xC8, 12, 0x00, 0x32, 0x36, 0x45, 0x06, 0x16, 0x37, 0x75, 0x77, 0x54, 0x0C, 0x00);
	displayCommand(0x36, 1, 0x2B);                          //Orientation: W=480 H=320
	displayCommand(0x3A, 1, 0x55);                          //Color: RGB565
	displayCommand(0x29);
}

static inline void displayWrite(uint16_t data)
{
	//Reverse bit order of data
	uint32_t datarev;
	asm("rbit %0, %1" : "=r" (datarev) : "r" (data));
	datarev>>=16;
	//Start write cycle
	setLow(LCD_WRX);
	//Clear all bits
	REG_PIOA_CODR = PIOA_MASK;
	REG_PIOB_CODR = PIOB_MASK;
	REG_PIOC_CODR = PIOC_MASK;
	REG_PIOD_CODR = PIOD_MASK;
	//Set all bits
	REG_PIOA_SODR = (getBit(data, 6) << 7) | (getBit(data, 9, 2) << 14);
	REG_PIOB_SODR = (getBit(data, 8) << 26);
	REG_PIOC_SODR = getBit(datarev, 11, 5) << 1;
	REG_PIOD_SODR = (getBit(data, 5) << 10) | (getBit(data, 7) << 9) | (getBit(data, 11, 4)) | (getBit(data, 15) << 6);
	//End write cycle
	setHigh(LCD_WRX);
}

void displayCommand(uint8_t cmd)
{
	setLow(LCD_DCX);
	displayWrite(cmd);
}

void displayCommand(uint8_t cmd, unsigned int count, ...)
{
	displayCommand(cmd);
	va_list param;
	va_start(param, count);
	for (unsigned int i=0;i<count;i++)
	{
		uint8_t arg = va_arg(param, int);
		displayData(arg);
	}
	va_end(param);
}

void displayData(uint16_t data)
{
	setHigh(LCD_DCX);
	displayWrite(data);
}

void displayData(uint16_t data, unsigned int count)
{
	displayData(data);
	asm("nop");
	for (unsigned int i=0;i<count-1;i++)
	{
		setLow(LCD_WRX);
		setHigh(LCD_WRX);
		asm("nop");
	}
}

void setRegion(int x1, int y1, int x2, int y2)
{
	displayCommand(0x2A, 4, x1 >> 8, x1, x2 >> 8, x2);
	displayCommand(0x2B, 4, y1 >> 8, y1, y2 >> 8, y2);
}

void clear(uint16_t color)
{
	fillRegion(0, 0, WIDTH-1, HEIGHT-1, color);
}

void fillRegion(int x1, int y1, int x2, int y2, uint16_t color)
{
	setRegion(x1, y1, x2, y2);
	displayCommand(0x2C);
	displayData(color, (x2-x1+1)*(y2-y1+1));
}

void drawPixel(int x, int y, uint16_t color)
{
	fillRegion(x, y, x, y, color);
}

void drawLine(int x1, int y1, int x2, int y2, uint16_t color)
{
	if (x1==x2 || y1==y2)
		fillRegion(x1, y1, x2, y2, color);
	else
	{
		int dx = x2-x1;
		int dy = y2-y1;
		for (int x=x1;x<=x2;x++)
			drawPixel(x, (x-x1)*dy/dx, color);
	}
}

void drawBitmap(const uint8_t* bitmap, int x, int y, int bits, int width, int height)
{
	int rowSize = (width*bits+7)/8;			//Align to byte
	setRegion(x, y, x+width-1, y+height-1);
	displayCommand(0x2C);
	if (bits==1)							//Monochrome
	{
		for(int i=0;i<height;i++)
			for(int j=0;j<width;j++)
				displayData(-getBit(bitmap[i*rowSize+j/8], 7-(j%8)));
	}
	else if (bits==8)						//RGB332
	{
		for(int i=0;i<height;i++)
			for(int j=0;j<width;j++)
			{
				uint8_t pixel = bitmap[i*rowSize+j];
				displayData(color16(getBit(pixel, 5, 3), getBit(pixel, 2, 3), getBit(pixel, 0, 2)));
			}
	}
	else if (bits==16)						//RGB565
	{
		for(int i=0;i<height;i++)
			for(int j=0;j<width;j++)
				displayData(bitmap[i*rowSize+j*2] | (bitmap[i*rowSize+j*2+1] << 8));
	}
}

void drawBitmap(const uint8_t* bitmap, int x, int y, int width, int height, uint16_t color, uint16_t background)
{
	int diff = color ^ background;
	int rowSize = (width+7)/8;				//Align to byte
	setRegion(x, y, x+width-1, y+height-1);
	displayCommand(0x2C);
	for(int i=0;i<height;i++)
		for(int j=0;j<width;j++)
			displayData(((-getBit(bitmap[i*rowSize+j/8], 7-(j%8))) & diff) ^ background);
}
