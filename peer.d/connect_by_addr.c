#include"connect_by_addr.h"
#include<stdio.h>
#include<string.h>
#include<errno.h>

int connect_by_addr(struct sockaddr_in addr)
{
    int s;
    
    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating a socket for connecting to new peer assumed at %s:%d: %s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), strerror(errno));
        return -1;
    }

    if(connect(s, (struct sockaddr *)&addr, sizeof(addr))<0)
    {
        fprintf(stderr, "\n[-]Error in connecting to peer assumed at %s:%d: %s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), strerror(errno));
        return -1;
    }

    return s;
}
