#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "epd_fonts.h"
#include "utf8font8x8.h"
//#include "font8x8.h"
//#include "System5x7.h"
static tFontStruct F;

static void epd_transpose(uint8_t *pDest, uint8_t *pSrc);

int32_t epd_fontinit(void)
{
    int32_t retVal = -1;
    uint8_t* pFonts = CP437FONT8x8;
    
    F.fontSize = pFonts[FONT_SIZE_L] | 
                    (uint16_t)(pFonts[FONT_SIZE_H] << 8U);

    if(F.fontSize)
    {
        printf("\nThe Fontsize was %d:",F.fontSize);
        if(1U == F.fontSize)
        {
            F.fontWidthOffset = 0U;
        }else{
            /** We look for variable font structure */
            F.fontWidthOffset = 6U;
        }
        F.fontWidth = pFonts[FONT_WIDTH];
        F.fontHeight = pFonts[FONT_HEIGHT];
        F.fontTotalGlyphs = pFonts[FONT_TOTALGLYPHS];
        F.fontFirstGlyph = pFonts[FONT_FIRSTGLYPH];
        F.pfontArray = pFonts;
    }else
    {
        printf("\nThe Fontsize was %d:",F.fontSize);
    } 
    return (retVal);
}

int32_t epd_putchar(uint32_t x, uint32_t y, uint8_t* pBuffer, uint8_t c)
{
    int32_t retVal = -1;
    /** Find the character array from the table */
    int32_t glyphStartIndex = ((c * F.fontWidth) + 6U - (F.fontFirstGlyph * F.fontWidth));
    int32_t bufferIndex;
    uint8_t src[F.fontHeight], dest[F.fontHeight], i = 0;
    // printf("\n%c:%d:%d\n",c,glyphStartIndex,F.pfontArray[14]);
    memset(src,0,sizeof(src));
    memset(dest,0,sizeof(dest));
    bufferIndex = (EPD_BYTES_PER_ROW * y) + x;
    for(i=0; i < F.fontHeight; i++)
    {
        src[i] = F.pfontArray[glyphStartIndex++];
    }
    epd_transpose(dest, src);
    for(i = 0; i < F.fontWidth ;i++)
    {
        pBuffer[bufferIndex] = dest[i];
        bufferIndex += (EPD_BYTES_PER_ROW);
    }
    
    return (retVal);
}

int32_t epd_putstring(uint32_t x, uint32_t y, uint8_t *pBuffer, char* pString)
{
    int32_t retVal = -1;
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
    return (retVal);
}

static void epd_transpose(uint8_t *pDest, uint8_t *pSrc)
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
