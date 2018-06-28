#define _GNU_SOURCE
#define NEEDS_JOINEE_GLOBALS

#include"cli_run.h"
#include"global_defs.h"
#include"allocate.h"
#include"snd_rcv.h"
#include"query_db.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>

void *cli_run(void *c)
{
    struct joinee *cli=(struct joinee *)c;
    char *cmdr, *cmds, *query, *cmds_last;
    char *retval=(char *)allocate("char", 64);
    int exp_col;

    while(1)
    {    
        cmds=(char *)allocate("char", 2048);

        //rcv query and check privilages
        if((cmdr=rcv(cli->sock, "receive client query"))==NULL)
        {
            sprintf(retval, "FALIURE IN RECV");
            break;
        }
        if(strcasestr(cmdr, "insert")!=NULL || strcasestr(cmdr, "update")!=NULL)
        {
            sprintf(retval, "USER RIGHTS VIOLATION");
            break;
        }
        else if(strcmp(cmdr, "END")==0)
        {
            sprintf(retval, "ENDING DB CONNECTION"); 
            break;
        }

        //parse and execute query
        exp_col=(int)strtol(strtok(cmdr, ":"), NULL, 10);
        query=strtok(NULL, ":");
        if(query_db(cli, query, cmds, exp_col))
        {
            sprintf(retval, "ERROR IN QUERYING %s", query);
            break;
        }
        free(cmdr);

        //send output
        if(snd(cli->sock, cmds, "send back query output"))
        {
            sprintf(retval, "ERROR IN SENDING BACK OUTPUT");
            break;
        }
    }

    cmds_last=(char *)allocate("char", 2048);
    sprintf(cmds_last, "%s", retval);
    if(snd(cli->sock, cmds_last, "send back feedback to client"))
    {
        explicit_bzero(retval, sizeof(char)*64);
        sprintf(retval, "ERROR IN SENDING EXIT STATUS");
    }

    close(cli->sock);
    pthread_exit(retval);
}
