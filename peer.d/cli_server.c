#define NEEDS_JOINEE_STRUCT

#include"cli_server.h"
#include"allocate.h"
#include"global_defs.h"
#include"cli_run.h"
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<errno.h>

void *cli_server(void *a)
{
    int stat;
    pthread_t tid[10];
    struct joinee cli[10];
    char *retval;
    socklen_t len=sizeof(struct sockaddr_in);

    for(cli_count=0; cli_count<10; cli_count++)
    {
        if((cli[cli_count].sock=accept(server_csock, (struct sockaddr *)&cli[cli_count].addr, &len))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d: %s\n", cli_count, strerror(errno));
            continue;
        }
        cli[cli_count].id=cli_count;

        if((stat=pthread_create(&tid[cli_count], NULL, cli_run, &cli[cli_count]))!=0)
        {
            fprintf(stderr, "\n[-]Error in starting client thr %d at %s:%d: %s\n", cli_count, inet_ntoa(cli[cli_count].addr.sin_addr), ntohs(cli[cli_count].addr.sin_port), strerror(stat));
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
        printf("\n[!]Client %s:%d exited with status: %s\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), (char *)retval);
        close(cli[i].sock);
    }

    pthread_exit("SUCCESS");
}
