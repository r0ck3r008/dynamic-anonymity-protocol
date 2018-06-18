#include"server_run.h"
#include"cli_run.h"
#include"server_init.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<errno.h>

int cli_count=0;

void *server_run(void *a)
{
    pthread_t tid[10];
    struct client cli[10];
    socklen_t len=sizeof(struct sockaddr_in);
    int stat;
    char *retval;

    printf("\n[!]Server thread initiated...\n");

    for(cli_count; cli_count<10; cli_count++)
    {
        if((cli[cli_count].sock=accept(server_sock, (struct sockaddr *)&cli[cli_count].addr, &len))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d: %s\n", cli_count, strerror(errno));
            continue;
        }

        if((stat=pthread_create(&tid[cli_count], NULL, cli_run, &cli[cli_count]))!=0)
        {
            fprintf(stderr, "\n[-]Error in initiating client thread at %s:%d: %s\n", inet_ntoa(cli[cli_count].addr.sin_addr), ntohs(cli[cli_count].addr.sin_port), strerror(stat));
            continue;
        }
    }

    for(int i=0; i<cli_count; i++)
    {
        if((stat=pthread_join(tid[i], (void **)&retval))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to client %s:%d: %s\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), strerror(stat));
            continue;
        }
        printf("\n[!]Client %s:%d returned %s\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), (char *)retval);
    }

    pthread_exit("SUCCESS");
}

