#include <stdio.h>
#include <stdint.h>

#define FONT_SIZE_L                 0
#define FONT_SIZE_H                 1
#define FONT_WIDTH                  2
#define FONT_HEIGHT                 3
#define FONT_FIRSTGLYPH             4
#define FONT_TOTALGLYPHS            5

#define EPD_PIXELS_PER_ROWS         264
#define EPD_TOTAL_ROWS              176

#define EPD_BYTES_PER_ROW           (EPD_PIXELS_PER_ROWS / 8)
#define EPD_CHARS_PER_LINE          (EPD_TOTAL_ROWS / 8)
#define BYTEBUFFER                  (EPD_BYTES_PER_ROW * EPD_TOTAL_ROWS)
#define MAX_EPD_CHARS               (EPD_CHARS_PER_LINE * EPD_BYTES_PER_ROW)

typedef struct epd_font{
    uint16_t    fontSize;
    uint8_t     fontWidth;
    uint8_t     fontHeight;
    uint8_t     fontFirstGlyph;
    uint16_t    fontTotalGlyphs;
    uint16_t    fontWidthOffset;
    uint8_t*    pfontArray;
}tFontStruct;

int32_t epd_fontinit(void);
int32_t epd_putchar(uint32_t x, uint32_t y, uint8_t* pBuffer, uint8_t c);
int32_t epd_putstring(uint32_t x, uint32_t y, uint8_t* pBuffer, char* pString);
