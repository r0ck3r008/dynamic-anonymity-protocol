#include"get_rand_peer.h"
#include"allocate.h"
#include"snd_rcv.h"
#include"dbconnect.h"
#include"gen_keys.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sodium.h>
#include<errno.h>

int get_rand_peer(struct peer *p, int rand_sno, char *ku_fname) //ku_fname is not null while selecting const_peer
{
    char *query=(char *)allocate("char", 2048), *cmdr, *ip=(char *)allocate("char", 20);

    if(ku_fname!=NULL)
    {
        sprintf(query, "0:1:3:select id, ip, ku from peers where sno=%d;", rand_sno);
    }
    else
    {
        sprintf(query, "0:1:2:select id, ip from peers where sno=%d;", rand_sno);
    }

    if(snd(db_sock, query, "get ip, id, and key from db_inteface\n"))
    {
        return 1;
    }

    if((cmdr=rcv(db_sock, "receive id, ip, key from peers via rand_sno\n"))==NULL)
    {
        return 1;
    }

    p->id=(int)strtol(strtok(cmdr, ":"), NULL, 10);
    sprintf(ip, "%s", strtok(NULL, ":"));
    p->addr.sin_addr.s_addr=inet_addr(ip);
    p->addr.sin_port=htons(6666);
    p->ku_fname=ku_fname;

    if(ku_fname!=NULL)
    {
        char *key_str=strtok(NULL, ":");
        FILE *f;
        if((f=fopen(ku_fname, "w"))==NULL)
        {
            fprintf(stderr, "\n[-]Error in opening file %s: %s\n", ku_fname, strerror(errno));
            return 1;
        }
        for(int i=0; i<strlen(key_str); i++)
        {
            fprintf(f, "%c", key_str[i]);
        }
        fclose(f);
        if((p->ku=gen_keys(ku_fname, 1))==NULL)
        {
            return 1;
        }
    }
    else
    {
        p->ku=NULL;
    }

    free(cmdr);
    return 0;
}

