#define NEEDS_STRUCT_GLOBALS

#include"connect_to_new_peer.h"
#include<stdio.h>
#include<string.h>
#include<errno.h>

int connect_to_new_peer(struct peer *p)
{
    int s;
    
    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating a socket for connecting to new peer assumed at %s:%d: %s\n", inet_ntoa(p->addr.sin_addr), ntohs(p->addr.sin_port), strerror(errno));
        return 1;
    }

    if(connect(s, (struct sockaddr *)&p->addr, sizeof(p->addr))<0)
    {
        fprintf(stderr, "\n[-]Error in connecting to peer assumed at %s:%d: %s\n", inet_ntoa(p->addr.sin_addr), ntohs(p->addr.sin_port), strerror(errno));
        return 1;
    }

    p->sock=s;
    return 0;
}
