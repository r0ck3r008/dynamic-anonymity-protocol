#include"connect_to_fellow_peer.h"
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

int connect_to_fellow_peer(struct client *fell)
{
    int s;

    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating socket for fellow peer: %s\n", strerror(errno));
        return 1;
    }

    if(connect(s, (struct sockaddr *)&(fell->addr), sizeof(fell->addr))<0)
    {
        fprintf(stderr, "\n[-]Error in connecting to fellow peer at %s: %s\n", inet_ntoa(fell->addr.sin_addr), strerror(errno));
        return 1;
    }

    fell->sock=s;
    return 0;
}

