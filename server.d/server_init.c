#include"server_init.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<pthread.h>
#include"cli_run.h"

int server_init(char *argv1)
{
    char *ip=strtok(argv1, ":");
    int port=(int)strtol(strtok(NULL, ":"), NULL, 10), s;
    struct sockaddr_in addr;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr(ip);
    addr.sin_family=AF_INET;

    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating the server_socket: %s\n", strerror(errno));
        return 0;
    }

    if(bind(s, (struct sockaddr *)&addr, sizeof(addr))<0)
    {
        fprintf(stderr, "\n[-]bind: %s\n", strerror(errno));
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
    socklen_t len=sizeof(struct sockaddr_in);
    int stat;
    cli_count=0;
    char *retval;

    for(; cli_count<10; cli_count++)
    {
        if((cli[cli_count].sock=accept(server_sock, (struct sockaddr *)&cli[cli_count].addr, &len))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d: %s\n", cli_count, strerror(errno));
            continue;
        }

        cli[cli_count].id=cli_count;

        if((stat=pthread_create(&tid[cli_count], NULL, cli_run, &cli[cli_count]))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating thread for client at %s:%d: %s\n", inet_ntoa(cli[cli_count].addr.sin_addr), ntohs(cli[cli_count].addr.sin_port), strerror(stat));
            continue;
        }
    }

    for(int i=0; i<cli_count; i++)
    {
        if((stat=pthread_join(tid[i], (void **)&retval))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to client at %s:%d: %s\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), strerror(stat));
            continue;
        }

        printf("\n[!]The client %s:%d exited with status %s\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), (char *)retval);
    }

    return 0;
}
