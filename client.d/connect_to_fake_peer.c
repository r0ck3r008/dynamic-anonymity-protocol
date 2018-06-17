#include"connect_to_fake_peer.h"
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

int connect_to_fake_peer(struct peer *p)
{
    int s;

    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating fake peer soket for peer %s: %s\n", inet_ntoa(p->addr.sin_addr), strerror(errno));
        return 1;
    }
    
    if(connect(s, (struct sockaddr *)&(p->addr), sizeof(p->addr))<0)
    {
        fprintf(stderr, "\n[-]Error in connecting to %s: %s\n", inet_ntoa(p->addr.sin_addr), strerror(errno));
        return 1;
    }

    p->sock=s;

    return 0;
}

