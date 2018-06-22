#include"snd_rcv.h"
#include"allocate.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<errno.h>

int snd(int sock, char *cmds, char *reason)
{
    if(send(sock, cmds, 2048*sizeof(char), 0)<0)
    {
        fprintf(stderr, "\n[-]Error in sending %s for reason %s:%s\n", cmds, reason, strerror(errno));
        return 1;
    }

    free(cmds);
}

char *rcv(int sock, char *reason)
{
    char *cmdr=(char *)allocate("char", 2048);

    if(recv(sock, cmdr, sizeof(char)*2048, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in receving for reason %s:%s\n", reason, strerror(errno));
        return NULL;
    }
}
