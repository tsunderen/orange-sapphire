#ifndef DISPLAY_H
#define DISPLAY_H

#include "Arduino.h"

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

inline void setLow(int pin);
inline void setHigh(int pin);
inline unsigned int getBit(unsigned int data, int bit);
inline unsigned int getBit(unsigned int data, int bit, int len);

uint16_t color16(uint8_t red, uint8_t green, uint8_t blue);
uint16_t color16(uint8_t color256);
uint8_t color8(uint8_t red, uint8_t green, uint8_t blue);
uint8_t color8(uint16_t color);

inline void displayInitGPIO();
void displayReset();
void displayInit();

inline void displayWrite(uint16_t data);
inline void displayCommand(uint8_t cmd);
void displayCommand(uint8_t cmd, unsigned int count, ...);
inline void displayData(uint16_t data);
void displayData(uint16_t data, unsigned int count);

void setRegion(int x1, int y1, int x2, int y2);
void clear(uint16_t color = BLACK);
void fillRegion(int x1, int y1, int x2, int y2, uint16_t color);
void drawPixel(int x, int y, uint16_t color);
void drawLine(int x1, int y1, int x2, int y2, uint16_t color);

void drawBitmap(const uint8_t* bitmap, int x, int y, int bits, int width, int height);
void drawBitmap(const uint8_t* bitmap, int x, int y, int width, int height, uint16_t color, uint16_t background);

#endif
