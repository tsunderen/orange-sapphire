#ifndef OUTPUT_H
#define OUTPUT_H

#include "display.h"
#include "font.h"

static inline uint16_t getBit(uint16_t data, unsigned int bit);
static inline uint16_t getBit(uint16_t data, unsigned int bit, unsigned int len);

class Output
{
	private:
		int width;
		int height;
		const uint8_t* font;
		int fontX;
		int fontY;

		uint32_t* buffer;                   //[31:24]=Background(RGB332) [23:8]=Foreground(RGB565) [7:0]=Character
		int x;
		int y;
		int offset;                         //Buffer address offset of the first line
		
		int staticX;                        //Static cursor will change when screen scrolls
		int staticY;
		
		uint16_t color;
		uint8_t background;

		void print(char ch);
		void print(const char* str);
		void print(char ch, uint16_t color, uint8_t background);
		void print(const char* str, uint16_t color, uint8_t background);
		void print(char ch, int x, int y, uint16_t color, uint8_t background);
		void print(const char* str, int x, int y, uint16_t color, uint8_t background);

	public:
		static const uint16_t PALETTE[];

		Output(int width, int height, const uint8_t* font, int fontX, int fontY);
		~Output();

		void init();						//Hardware initialization
		void reset();						//Hardware reset
		void clear();						//Clear buffer and screen, reset cursor
		void redraw();
		void redraw(int line);

		void setCur(int x, int y);			//Main cursor
		void setX(int x);
		void setY(int y);
		int getX();
		int getY();

		void setStaticCur(int x, int y);	//Static cursor
		void setStaticX(int x);
		void setStaticY(int y);
		int getStaticX();
		int getStaticY();

		void setColor(uint16_t c);
		uint16_t getColor();
		void setBackground(uint16_t c);
		void setBackground(uint8_t c);
		uint16_t getBackground();

		int printf(const char* format, ...);
		int printfc(const char* format, uint16_t color, ...);
		int printfcb(const char* format, uint16_t color, uint16_t background, ...);
		int printfxy(const char* format, int x, int y, ...);
		int printfxyc(const char* format, int x, int y, uint16_t color, ...);
		int printfxycb(const char* format, int x, int y, uint16_t color, uint16_t background, ...);

};

#endif
