#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "epd_netid.h"
#ifdef TEST_EPD_NETID
int main(int argc, char *argv[])
{
    uint8_t* pBuffer = malloc(1024);
    memset(pBuffer, 0, 1024);
    int x = 0;
    x = sprintf((char*)pBuffer+x,"%s", "Sohal\n");
    x = epd_getID(x,pBuffer);
    printf("%s\n:%d\n", pBuffer,x);
    exit(EXIT_SUCCESS);
}
#endif
int epd_getID(int x, uint8_t* pBuffer)
{
    struct ifaddrs *ifaddr, *ifa;
    int  s;
    int charCount = x;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) 
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }


    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr == NULL)
            continue;  

        s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        if(((strcmp(ifa->ifa_name,"wlan0")==0)||((strcmp(ifa->ifa_name,"eth0")==0))) &&(ifa->ifa_addr->sa_family==AF_INET))
        {
            if (s != 0)
            {
                charCount += sprintf((char*)pBuffer+charCount,"getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            charCount += sprintf((char*)pBuffer+charCount,"Interface : <%s>\n",ifa->ifa_name );
            charCount += sprintf((char*)pBuffer+charCount,"  Address : <%s>\n", host);
            charCount += sprintf((char*)pBuffer+charCount,"%s", "\n");  
        }
    }

    freeifaddrs(ifaddr);
    return (charCount);
}
