#include"server_init.h"
#include"cli_run.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

int server_init(char *argv1)
{
    int s;
    cli_num=0;
    char *ip=strtok(argv1, ":");
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(ip);
    addr.sin_port=htons((int)strtol(strtok(NULL, ":"), NULL, 10));

    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]socket: %s\n", strerror(errno));
        return 0;
    }
    
    if(bind(s, (struct sockaddr *)&addr, sizeof(addr))<0)
    {
        fprintf(stderr, "\n[-]Bind: %s\n", strerror(errno));
        return 0;
    }

    if(listen(s, 5)<0)
    {
        fprintf(stderr, "\n[-]Listen: %s\n", strerror(errno));
        return 0;
    }

    return s;
}

int server_workings()
{
    pthread_t tid[10];
    struct client cli[10];
    int stat;
    socklen_t len=sizeof(struct sockaddr_in);
    char *retval;
    printf("\n[!]Starting dbinterface....\n[!]Waiting for connections...\n");

    for(; cli_num<10; cli_num++)
    {
        if((cli[cli_num].sock=accept(server_sock, (struct sockaddr *)&cli[cli_num].addr, &len))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d: %s\n", cli_num, strerror(errno));
            continue;
        }

        if((stat=pthread_create(&tid[cli_num], NULL, cli_run, &cli[cli_num]))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating new client thread %d: %s\n", cli_num, strerror(stat));
            continue;
        }
    }

    for(int i=0; i<cli_num; i++)
    {
        if((stat=pthread_join(tid[i], (void **)&retval))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to client %d: %s\n", i, strerror(stat));
            continue;
        }
        printf("\n[!]Client %d exited with code: %s\n", (char *)retval);
    }

    return 0;
}


