#define NEEDS_JOINEE_STRUCT

#include"cli_run.h"
#include"allocate.h"
#include"snd_rcv.h"
#include"global_defs.h"
#include"regex_check.h"
#include"handle_new_client.h"
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>

void *cli_run(void *c)
{
    struct joinee *cli=(struct joinee *)c;
    char *cmds, *cmdr, *retval=(char *)allocate("char", 64);
    struct joinee j;
    int cli_rand_num;

    for(int flag=0; ; flag++)
    {
        //rcv from client and do checks
        if((cmdr=rcv(cli->sock, "receive from client"))==NULL)
        {
            sprintf(retval, "ERROR IN RECEVING FROM CLIENT");
        }
        if(strcmp(cmdr, "END")==0)
        {
            sprintf(retval, "ENDING PEER CONNECTION");
            if(snd(j.sock, cmdr, "END CONNECTION TO PEER"))
            {
                explicit_bzero(retval, sizeof(char)*64);
                sprintf(retval, "ERROR IN SENDING END PEER CONNECTION");
            }
            break;
        }
        else if(strcmp(cmdr, "SERVER_END")==0)
        {
            sprintf(retval, "ENDING FINAL SERVER CONNECTION");
            if(snd(j.sock, cmdr, "ending final server connection"))
            {
                explicit_bzero(retval, sizeof(char)*64);
                sprintf(retval, "ERROR IN SENDING ACK TO END FINAL CONNECTION");
            }
            break;
        }
        if(regex_check(cmdr, "^[0-9]\\{1,3\\}[.]*")) //first time joinee
        {
            char *ip=strtok(cmdr, ":");
            cli_rand_num=(int)strtol(strtok(NULL, ":"), NULL, 10);
            //make new cmdr
            char *cmdr_tmp=(char *)allocate("char", 2048);
            sprintf(cmdr_tmp, "%s", strtok(NULL, ":"));
            if(handle_new_client(&j, cli, ip, cli_rand_num))
            {
                sprintf(retval, "ERROR IN UPDATING NEW CLIENT");
                break;
            }
            explicit_bzero(cmdr, sizeof(char)*2048);
            sprintf(cmdr, "%s", cmdr_tmp);
            free(cmdr_tmp);
            flag=1;
        }

        //send and rcv from  its const_peer
        cmds=(char *)allocate("char", 2048);
        sprintf(cmds, "%d:%s", cli_rand_num, cmdr);
        free(cmdr);
        if(snd(j.sock, cmds, "send client's cmd to peer"))
        {
            sprintf(retval, "ERROR IN SENDING TO PEER");
            break;
        }
        if(flag!=0)
        {
            if((cmdr=rcv(j.sock, "receive from clients const_peer"))==NULL)
            {
                sprintf(retval, "ERROR IN RECEVING FROM CONST_PEER");
                break;
            }

            //send to client
            cmds=(char *)allocate("char", 2048);
            sprintf(cmds, "%s", cmdr);
            free(cmdr);
            if(snd(cli->sock, cmds, "send back to client from const_peer"));
            {
                sprintf(retval, "ERROR IN SENDING BACK TO CLIENT");
                break;
            }
        }
    }

    //rcv ack from const_peer
    if((cmdr=rcv(j.sock, "receive final ack from const_peer"))==NULL)
    {
        explicit_bzero(retval, sizeof(char)*64);
        sprintf(retval, "ERROR IN RECEVING ACK FROM CONST_PEER");
    }
    //send final ack and exit
    sprintf(cmds, "%s", retval);
    if(snd(cli->sock, cmds, "send back final ack"))
    {
        explicit_bzero(retval, sizeof(char)*64);
        sprintf(retval, "ERROR IN SENDING LAST ACK");
    }
    close(j.sock);
    close(cli->sock);
    pthread_exit(retval);
}
