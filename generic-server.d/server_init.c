#include"server_init.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<errno.h>

int server_init(char *argv)
{
    char *ip=strtok(argv, ":");
    int port=(int)strtol(strtok(NULL, ":"), NULL, 10);
    struct sockaddr_in addr;
    int s;

    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr(ip);

    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating socket for server at %s:%d: %s\n", ip, port, strerror(errno));
        return -1;
    }

    if(bind(s, (struct sockaddr *)&addr, sizeof(addr))<0)
    {
        fprintf(stderr, "\n[-]Error in binding %s:%d: %s\n", ip, port, strerror(errno));
        return -1;
    }

    if(listen(s, 5)<0)
    {
        fprintf(stderr, "\n[-]Error in listning to %s:%d: %s\n", ip, port, strerror(errno));
        return -1;
    }

    return s;
}
