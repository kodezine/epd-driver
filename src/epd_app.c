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
#include "epd_netid.h"

int main(int argc, char* argv[])
{
    char* pUser = NULL;
    if(argc > 2)
    {
        printf("\nProvide one arguement only\n");
        return 1;
    }else{
        if(argc < 2)
        {;
        }else
        {
            pUser = argv[1];
        }
    }
    
    const char* pName = "KODEZINE UG";
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
        x += sprintf(pScreen+x, "%s\n", pName);
        x += sprintf(pScreen+x, "%4d.%02d.%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        if(pUser != NULL)
        {
            x += sprintf(pScreen+x, "%s\n", pUser);
        }
        x += sprintf(pScreen+x, "%s\n", "_________________________________");

        x += sprintf(pScreen+x, "%s","\n");
        
        x = epd_getID(x, (uint8_t*)pScreen);

        epd_fontinit();
        
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

