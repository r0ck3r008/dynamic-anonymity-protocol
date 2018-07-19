#include"end_db_connection.h"
#include"global_defs.h"
#include"common_headers/allocate.h"
#include"common_headers/snd_rcv.h"
#include<stdio.h>
#include<string.h>

int end_db_connection()
{
    char *cmds=(char *)allocate("char", 2048), *cmdr;

    sprintf(cmds, "END");
    if(snd(db_sock, cmds, "END DB CONNECTION"))
    {
        return 1;
    }

    if((cmdr=rcv(db_sock, "RECV ACK FOR DB CXN END"))==NULL)
    {
        return 1;
    }

    if(strcmp(cmdr, "ENDING DB CONNECTION")!=0)
    {
        fprintf(stderr, "\n[-]Error in ending db connection\n");
        return 1;
    }

    return 0;
}
