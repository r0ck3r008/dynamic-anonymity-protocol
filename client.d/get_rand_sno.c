#include"get_rand_sno.h"
#include"allocate.h"
#include"snd_rcv.h"
#include"dbconnect.h"
#include<stdlib.h>
#include<string.h>
#include<sodium.h>

int get_rand_sno()
{
    char *query=(char *)allocate("char", 2048);
    char *cmdr;
    sprintf(query, "0:1:1:select count(*) from peers;");    //0 for non-insert, 1 for using peers table, 1 for 1 exp_col
    
    if(snd(db_sock, query, "query to count number of peers"))
    {
        return -1;
    }

    if((cmdr=rcv(db_sock, "to receive the number of peers from dbinterface"))==NULL)
    {
        return -1;
    }

    int peer_count=(int)strtol(cmdr, NULL, 10);
    int rand_sno=(int)randombytes_uniform(peer_count);

    free(cmdr);
    return rand_sno;
}

