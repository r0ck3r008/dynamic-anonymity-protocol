#define NEEDS_CLIENT_STRUCT

#include"server_workings.h"
#include"global_defs.h"
#include"common_headers/allocate.h"
#include"common_headers/snd_rcv.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>

int server_workings(char *argv1)
{
    int stat;
    char *retval;
    cli_num=0;
    socklen_t len=sizeof(struct sockaddr_in);
    pthread_t tid[10];
    struct client cli[10];
    my_addr.sin_addr.s_addr=inet_addr(strtok(argv1, ":"));
    my_addr.sin_port=ntohs((int)strtol(strtok(NULL, ":"), NULL, 10));
    my_addr.sin_family=AF_INET;

    for(cli_num; cli_num<10; cli_num++)
    {
        if((cli[cli_num].sock=accept(server_sock, (struct sockaddr *)&cli[cli_num].addr, &len))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d: %s\n", cli_num, strerror(errno));
            continue;
        }

        cli[cli_num].id=cli_num;

        if((stat=pthread_create(&tid[cli_num], NULL, cli_run, &cli[cli_num]))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating thread %d: %s\n", cli_num, strerror(stat));
            continue;
        }
    }

    for(int i=0; i<cli_num; i++)
    {
        if((stat=pthread_join(tid[i], (void **)&retval))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to client %d at %s:%d: %s\n", i, inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), strerror(stat));
            continue;
        }

        printf("\n[!]Client %s:%d returned with status: %s\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), (char *)retval);
        free(retval);
    }
}

void *cli_run(void *c)
{
    struct client *cli=(struct client *)c;
    char *retval=(char *)allocate("char", 64), *cmdr, *cmds;

    while(1)
    {
        if((cmdr=rcv(cli->sock, "recv from client"))==NULL)
        {
            sprintf(retval, "ERROR IN RECEVING FROM CLIENT");
            break;
        }
        printf("\n[!]Received %s from %s:%d\n", cmdr, inet_ntoa(cli->addr.sin_addr), ntohs(cli->addr.sin_port));

        cmds=(char *)allocate("char", 2048);
        sprintf(cmds, "[!]echoed from server at %s:%d-> '%s'\n", inet_ntoa(my_addr.sin_addr), ntohs(my_addr.sin_port), cmdr);
        free(cmdr);

        if(snd(cli->sock, cmds, "send back echo"))
        {
            sprintf(retval, "ERROR IN SENDING BACK ECHOED OUTPUT");
        }
    }

    close(cli->sock);
    pthread_exit(retval);
}
