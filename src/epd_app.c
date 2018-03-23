#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "epd_fonts.h"
#define EPD_PIXELS_PER_ROWS         264
#define EPD_TOTAL_ROWS              176

#define EPD_BYTES_PER_ROW           (EPD_PIXELS_PER_ROWS / 8)

#define BYTEBUFFER                  (EPD_BYTES_PER_ROW*EPD_TOTAL_ROWS)


void epd_putchar(uint32_t x, uint32_t y, uint8_t* pBuffer, uint8_t c);
void epd_putstring(uint32_t x, uint32_t y, uint8_t* pBuffer, char* pString);

int main(void)
{
    uint8_t *pBuffer = malloc(BYTEBUFFER);
    if(pBuffer != NULL)
    {
        memset(pBuffer, 0, BYTEBUFFER);
        epd_putstring(0,8,pBuffer,"Hello Komal \nHello Dev\0");
        
        FILE *pEPD = fopen("test.bin", "wb+");

        if(pEPD != NULL)
        {
            fwrite(pBuffer, BYTEBUFFER, 1, pEPD);
        }
        fclose(pEPD);
    }
    return 0;
}

void transpose(uint8_t *pDest, uint8_t *pSrc)
{
    uint8_t i, j;
    for(i = 0; i < 8; i++)
    {
        for(j = 0; j < 8; j++)
        {
            if(pSrc[i] & (0x01U << j))
            {
                pDest[j] |= (0x01U << i);
            }else
            {
                pDest[j] &= (~(0x01U << i));
            }
        }
    }
}

void epd_putchar(uint32_t x, uint32_t y, uint8_t* pBuffer, uint8_t c)
{
    /** Find the character array from the table */
    int index = ((c*8)), bufferIndex;
    uint8_t src[8], dest[8], i = 0;
    memset(src,0,8);
    memset(dest,0,8);
    bufferIndex = (EPD_BYTES_PER_ROW * y) + x;
    for(; index < ((c*8)+8) ;index++)
    {
        src[i] = CP437FONT8x8[index];
        i++;
    }
    transpose(dest, src);
    for(index = 0; index < 8 ;index++)
    {
        pBuffer[bufferIndex] = dest[index];
        bufferIndex += (EPD_BYTES_PER_ROW);
    }
}

void epd_putstring(uint32_t x, uint32_t y, uint8_t *pBuffer, char* pString)
{
    do{
        if('\0' == *pString)
        {
            break;
        }else{
            if(*pString == '\n')
            {
                y += 8U;
                x = 0;
            }else{
                epd_putchar(x++, y, pBuffer, *pString);
            }
        }
        pString++;
    }while(1);
}