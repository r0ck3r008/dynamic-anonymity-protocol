#include"get_rand_sno.h"
#include"common_headers/allocate.h"
#include"common_headers/snd_rcv.h"
#include"global_defs.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sodium.h>

int get_rand_sno()
{
    char *cmds=(char *)allocate("char", 2048), *cmdr;
    int rand_sno;
    
    sprintf(cmds, "1:select count(*) from peers;");
    if(snd(db_sock, cmds, "send a query to get total numbers of peers"))
    {
        return -1;
    }

    if((cmdr=rcv(db_sock, "receibe total number of peers"))==NULL)
    {
        return -1;
    }

    if(strcmp(strtok(cmdr, ":"), "SUCCESS")!=0)
    {
        fprintf(stderr, "\n[-]Dbinterface returned %s\n", strtok(cmdr, ":"));
        return -1;
    }
    rand_sno=(int)randombytes_uniform((int)strtol(strtok(NULL, ":"), NULL, 10));

    free(cmdr);
    return rand_sno;
}
