#define NEEDS_MUTEX_GLOBALS
#define NEEDS_JOINEE_GLOBALS
#define _GNU_SOURCE

#include"peer_run.h"
#include"common_headers/allocate.h"
#include"common_headers/snd_rcv.h"
#include"query_db.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sodium.h>
#include<errno.h>

void *peer_run(void *a)
{
    struct joinee *p=(struct joinee *)a;
    char *retval=(char *)allocate("char", 64), *query;
    char *cmds, *cmdr, *cmds_last;
    int exp_col;

    if(snd_sno_rand_id(p, retval))
    {
        if(strcasestr(retval, "faliure")!=NULL)
        {
            goto exit;
        }
    }

    while(1)
    {
        cmds=(char *)allocate("char", 2048);

        //rcv query
        if((cmdr=rcv(p->sock, "rcv command from peer"))==NULL)
        {
            sprintf(retval, "ERROR IN RECEVING");
            break;
        }

        //check if it the end
        if(strcmp(cmdr, "END")==0)
        {
            sprintf(retval, "ENDING DB CONNECTION");
            break;
        }

        //parse cmdr and execute query
        exp_col=(int)strtol(strtok(cmdr, ":"), NULL, 10);
        query=strtok(NULL, ":");
        if(query_db(p, query, cmds, exp_col))
        {
            sprintf(retval, "ERROR IN QUERY EXECUTION");
            break;
        }
        free(cmdr);

        //send query output
        if(snd(p->sock, cmds, "send query output to peer"))
        {
            sprintf(retval, "ERROR IN SENDING");
            break;
        }
    }

exit:
    cmds_last=(char *)allocate("char", 2048);
    sprintf(cmds_last, "%s", retval);
    if(snd(p->sock, cmds_last, "send final feedback to peer"))
    {
        explicit_bzero(retval, sizeof(char)*64);
        sprintf(retval, "ERROR IN SENDING FINAL FEEDBACK");
    }
    close(p->sock);
    pthread_exit(retval);
}

int snd_sno_rand_id(struct joinee *p, char *retval)
{
    int rand_num, stat;
    char *cmds=(char *)allocate("char", 2048);

    if((stat=pthread_mutex_lock(&sno_rand_num))!=0)
    {
        sprintf(retval, "\n[-]faliure in locking sno_rand_num for %s:%d: %s\n", inet_ntoa(p->addr.sin_addr), ntohs(p->addr.sin_port), strerror(stat));
        return 1;
    }

    for(int flag=0; flag!=1; )
    {
        rand_num=(int)randombytes_uniform(1000000);
        for(int i=0; i<100; i++)
        {
            if(rand_num==rand_num_arr[i])
            {
                break;
            }
            else if(rand_num_arr[i]==-1)
            {  
                rand_num_arr[i]=rand_num;
                flag=1;
                break;
            }
        }
    }
    sprintf(cmds, "NUMS:%d:%d", sno++, rand_num);

    if((stat=pthread_mutex_unlock(&sno_rand_num))!=0)
    {
        sprintf(retval, "\nfaliure in unlocking sno_rand_num for %s:%d: %s\n", inet_ntoa(p->addr.sin_addr), ntohs(p->addr.sin_port), strerror(stat));
        return 1;
    }

    if(snd(p->sock, cmds, "send back sno and rand_id"))
    {
        sprintf(retval, "ERROR IN SENDING, faliure");
        return 1;
    }

    return 0;
}
