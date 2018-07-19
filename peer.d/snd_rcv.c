#define NEEDS_MUTEX

#include"common_headers/snd_rcv.h"
#include"common_headers/allocate.h"
#include"global_defs.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<errno.h>

int snd(int sock, char *cmds, char *reason)
{
    int stat;
    if(sock=db_sock)
    {
        if((stat=pthread_mutex_lock(&db_sock_mutex))!=0)
        {
            fprintf(stderr, "\n[-]Error in locking db_sock for cmds %s reason %s: %s\n", cmds, reason,  strerror(stat));
            return 1;
        }
    }
    if(send(sock, cmds, 2048*sizeof(char), 0)<0)
    {
        fprintf(stderr, "\n[-]Error in sending %s for reason %s:%s\n", cmds, reason, strerror(errno));
        return 1;
    }

    if(sock=db_sock)
    {
        if((stat=pthread_mutex_unlock(&db_sock_mutex))!=0)
        {
            fprintf(stderr, "\n[-]Error in unlocking db_sock for %s: %s\n", reason, strerror(stat));
            return 1;
        }
    }
    free(cmds);
}

char *rcv(int sock, char *reason)
{
    char *cmdr=(char *)allocate("char", 2048);
    int stat;

    if(sock=db_sock)
    {
        if((stat=pthread_mutex_lock(&db_sock_mutex))!=0)
        {
            fprintf(stderr, "\n[-]Error in locking db_sock for sending for reason %s: %s\n", reason, strerror(stat));
            return NULL;
        }
    }

    if(recv(sock, cmdr, sizeof(char)*2048, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in receving for reason %s:%s\n", reason, strerror(errno));
        return NULL;
    }

    if(sock=db_sock)
    {
        if((stat=pthread_mutex_unlock(&db_sock_mutex))!=0)
        {
            fprintf(stderr, "\n[-]Error in unlocking db_sock for receving for reason %s: %s\n", reason, strerror(stat));
            return NULL;
        }
    }

    return cmdr;
}
