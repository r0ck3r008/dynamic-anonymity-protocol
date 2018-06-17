#include"snd_rcv.h"
#include"allocate.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<errno.h>

int snd(int sock, char *cmds, char *reason) //this function frees the cmds
{
    if(send(sock, cmds, sizeof(char)*2048, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in sending for reeason: %s: %s\n", reason, strerror(errno));
        return 1;
    }

    free(cmds);
    return 0;
}

char *rcv(int sock, char *reason)   //cmdr is freeed by callee
{
    char *cmdr=(char *)allocate("char", 2048);

    if(recv(sock, cmdr, sizeof(char)*2048, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in receving for reason %s: %s\n", reason, strerror(errno));
        return NULL;
    }

    return cmdr;
}

