#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <err.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/stat.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <time.h>

#include "epd_fonts.h"
#define EPD_PIXELS_PER_ROWS         264
#define EPD_TOTAL_ROWS              176

#define EPD_BYTES_PER_ROW           (EPD_PIXELS_PER_ROWS / 8)
#define EPD_CHARS_PER_LINE          (EPD_TOTAL_ROWS / 8)
#define BYTEBUFFER                  (EPD_BYTES_PER_ROW * EPD_TOTAL_ROWS)
#define MAX_EPD_CHARS               (EPD_CHARS_PER_LINE * EPD_BYTES_PER_ROW)
int app_network(int *poffset, char* pString);
static void epd_putchar(uint32_t x, uint32_t y, uint8_t* pBuffer, uint8_t c);
static void epd_putstring(uint32_t x, uint32_t y, uint8_t* pBuffer, char* pString);
static void epd_transpose(uint8_t *pDest, uint8_t *pSrc);

int main(void)
{
    const char* pName = "\nKODEZINE UG";
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    FILE *pEPD = NULL;
    FILE *pEPC = NULL;
    uint8_t *pBuffer = malloc(BYTEBUFFER);
    char *pScreen = malloc(MAX_EPD_CHARS);
    memset(pBuffer, 0, BYTEBUFFER);
    memset(pScreen, 0, MAX_EPD_CHARS);
    
    if(pBuffer != NULL)
    {
        static int x = 0;
        x += sprintf(pScreen+x, "%s", pName);
        x += sprintf(pScreen+x, "    %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        
        epd_putstring(0, 0, pBuffer, pScreen);
        printf("%s: %d\n",pScreen, x);
        pEPD = fopen("/dev/epd/display","wb+");

        if(pEPD != NULL)
        {
            fwrite(pBuffer, BYTEBUFFER, 1, pEPD);
            pEPC = fopen("/dev/epd/command","wb+");
            if(pEPC != NULL)
            {
                fwrite("U", 1, 1, pEPC);
            }
            fclose(pEPC);
        }else{
            printf("\nWriting in a local 'test.bin' buffer\n");
            FILE *pBIN = fopen("test.bin", "wb+");

            if(pBIN != NULL)
            {
                fwrite(pBuffer, BYTEBUFFER, 1, pBIN);
            }
            fclose(pBIN);
        }
        fclose(pEPD);
    }else
    {
        perror("Error in allocating BYTEBUFFER\n");
    }
    free(pBuffer);
    free(pScreen);
    return 0;
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
    epd_transpose(dest, src);
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