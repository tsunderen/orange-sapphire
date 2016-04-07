#ifndef OUTPUT_H
#define OUTPUT_H

#include "display.h"
#include "font.h"

class Output
{
	private:
		int width;
		int height;
		const uint8_t* font;
		int fontX;
		int fontY;

		uint32_t* buffer;                   //[31:24]=Background(RGB332) [23:8]=Foreground(RGB565) [7:0]=Character
		int offset;                         //Buffer address offset of the first line

		int x;                              //Output location
		int y;
		int cursorX;                        //Cursor location
		int cursorY;
		
		uint16_t color;
		uint8_t background;

		//Helper function for screen buffer
		uint32_t* getBufferAddress(int x, int y);
		const uint8_t* getFontAddress(char ch);
		
		void setChar(uint32_t* buf, char ch);
		char getChar(uint32_t* buf);
		void setColor(uint32_t* buf, uint16_t color);
		uint16_t getColor(uint32_t* buf);
		void setBackground(uint32_t* buf, uint8_t background);
		uint8_t getBackground(uint32_t* buf);

		void draw(int x, int y);
		void drawCursor(int x, int y);

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
		void draw();

		void setCursor();
		void moveCursor(int move);
		void useCursor();

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

		Output& operator<<(int var);
		Output& operator<<(unsigned int var);
		Output& operator<<(double var);
		Output& operator<<(char var);
		Output& operator<<(const char* var);
		Output& operator<<(void* var);
};

#endif
