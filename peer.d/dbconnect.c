#include"dbconnect.h"
#include"allocate.h"
#include"snd_rcv.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

int dbconnect(char *argv)
{
    char *cmdr=(char *)allocate("char", 2048), *cmdr;
    char *ip=strtok(ip, ":");
    int port=(int)strtol(strtok(NULL, ":"), NULL, 10);
    struct sockaddr_in addr;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr(ip);
    addr.sin_family=AF_INET;
    int s;

    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating dbsock: %s\n", strerror(errno));
        return -1;
    }

    if(connect(s, (struct sockaddr *)&addr, sizeof(addr))<0)
    {
        fprintf(stderr, "\n[-]Error in connecting to %s:%d: %s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), strerror(errno));
        return -1;
    }

    return s;
}
