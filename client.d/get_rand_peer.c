#define NEEDS_STRUCT_GLOBALS

#include"get_rand_peer.h"
#include"allocate.h"
#include"snd_rcv.h"
#include"write_key_to_file.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int get_rand_peer(struct peer *p, int rand_sno, char *ku_fname)
{
    char *cmds=(char *)allocate("char", 2048), *cmdr;
    char *ip=(char *)allocate("char", 20);

    //segregate if its const_peer or fake_peers
    if(ku_fname!=NULL)
    {
         sprintf(cmds, "3:select id, ip, ku from peers where sno=%d;", rand_sno);
    }
    else
    {
         sprintf(cmds, "2:select id, ip from peers where sno=%d", rand_sno);
    }

    //send query
    if(snd(db_sock, cmds, "send query to get random peer"))
    {
        return 1;
    }

    if((cmdr=rcv(db_sock, "to receive random peer query resp"))==NULL)
    {
        return 1;
    }

    //check if success
    if(strcmp(strtok(cmdr, ":"), "SUCCESS")!=0)
    {
        fprintf(stderr, "\n[-]dbinterface returned %s for rand_peer query for rand_sno %d\n", strtok(cmdr, ":"), rand_sno);
        return 1;
    }
    //fill struct peer *p
    p->id=(int)strtol(strtok(NULL, ":"), NULL, 10);
    sprintf(ip, "%s", strtok(NULL, ":"));
    p->addr.sin_addr.s_addr=inet_addr(ip);
    p->addr.sin_port=htons(6666);
    p->addr.sin_family=AF_INET;

    if(ku_fname!=NULL)
    {
        p->ku_fname=ku_fname;
        if((p->ku=write_key_to_file(ku_fname, strtok(NULL, ":")))==NULL)
        {
            return 1;
        }
    }

    free(cmdr);
    return 0;
}
