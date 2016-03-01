#ifndef DISPLAY_H
#define DISPLAY_H

#include "Arduino.h"
#include "font.h"

#define LCD_DCX PIO_PC6
#define LCD_WRX PIO_PC7
#define LCD_CSX PIO_PC8
#define LCD_RESX PIO_PC9

#define PIOA_MASK 0x0000C080
#define PIOB_MASK 0x04000000
#define PIOC_MASK 0x0000003E
#define PIOD_MASK 0x0000064F
#define PIOC_MASK_CTRL 0x000003C0

#define WIDTH 480
#define HEIGHT 320

#define BLACK 0x0000
#define MAROON 0x8000
#define GREEN 0x0400
#define OLIVE 0x8400
#define NAVY 0x0010
#define PURPLE 0x8010
#define TEAL 0x0410
#define SILVER 0xC618
#define GRAY 0x8410
#define RED 0xF800
#define LIME 0x07E0
#define YELLOW 0xFFE0
#define BLUE 0x001F
#define FUCHSIA 0xF81F
#define AQUA 0x07FF
#define WHITE 0xFFFF

#define FONT CONSOLAS16
#define FONT_X 8
#define FONT_Y 16
#define FONT_OFFSET 0x20

const uint16_t PALETTE[] = {BLACK, MAROON, GREEN, OLIVE, NAVY, PURPLE, TEAL, SILVER,
							GRAY, RED, LIME, YELLOW, BLUE, FUCHSIA, AQUA, WHITE};

inline void setLow(uint32_t pin);
inline void setHigh(uint32_t pin);
inline uint16_t getBit(uint16_t data, unsigned int bit);
inline uint16_t getBit(uint16_t data, unsigned int bit, unsigned int len);
uint16_t getColor(uint8_t red, uint8_t green, uint8_t blue);

void displayInitGPIO();
void displayReset();
void displayInit();

void displayWrite(uint16_t data);
void displayCommand(uint8_t cmd);
void displayCommand(uint8_t cmd, unsigned int count, ...);
void displayData(uint16_t data);
void displayData(uint16_t data, unsigned int count);

void setRegion(int x1, int y1, int x2, int y2);
void clear(uint16_t color = BLACK);
void fillRegion(int x1, int y1, int x2, int y2, uint16_t color);
void drawPixel(int x, int y, uint16_t color);
void drawLine(int x1, int y1, int x2, int y2, uint16_t color);

void print(char ch, int x, int y, uint16_t color = WHITE, uint16_t background = BLACK);
void print(const char* str, int x, int y, uint16_t color = WHITE, uint16_t background = BLACK);
void print(String str, int x, int y, uint16_t color = WHITE, uint16_t background = BLACK);
void drawBitmap(uint8_t* bitmap, int x, int y, int width, int height);

#endif
