#include"end_db_connection.h"
#include"allocate.h"
#include"dbconnect.h"
#include"snd_rcv.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

int end_db_connection()
{
    char *cmds=(char *)allocate("char", 512);
    char *cmdr;

    sprintf(cmds, "DONE");
    if(snd(db_sock, cmds, "to end database connection\n"))
    {
        fprintf(stderr, "\n[-]Error in  sending end to db_inetface: %s\n", strerror(errno));
        return 1;
    }

    if((cmdr=rcv(db_sock, "to receive database stop ack\n"))==NULL)
    {
        return 1;
    }

    free(cmdr);
    return 0;
}

