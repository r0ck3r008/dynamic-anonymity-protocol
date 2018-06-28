#define NEEDS_JOINEE_GLOBALS

#include"cli_server.h"
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
    pthread_t tid[10];
    struct joinee jcli[10];
    socklen_t len=sizeof(struct sockaddr_in);
    int stat;
    char *retval;

    for(cli_count=0; cli_count<10; cli_count++)
    {
        if((jcli[cli_count].sock=accept(server_csock, (struct sockaddr *) &jcli[cli_count].addr, &len))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d for cli_server: %s\n", cli_count, strerror(errno));
            continue;
        }

        jcli[cli_count].peer=0;

        if((stat=pthread_create(&tid[cli_count], NULL, cli_run, &jcli[cli_count]))!=0)
        {
            fprintf(stderr, "\n[-]Error in starting new thr in c_server for %s:%d: %s\n", inet_ntoa(jcli[cli_count].addr.sin_addr), ntohs(jcli[cli_count].addr.sin_port), strerror(stat));
            continue;
        }
    }

    for(int i=0; i<cli_count; i++)
    {
        if((stat=pthread_join(tid[i], (void **)&retval))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to client %s:%d c_sock: %s\n", inet_ntoa(jcli[i].addr.sin_addr), ntohs(jcli[i].addr.sin_port), strerror(stat));
            continue;
        }

        printf("\n[!]Client %s:%d exited with status: %s\n", inet_ntoa(jcli[i].addr.sin_addr), ntohs(jcli[i].addr.sin_port), (char *)retval);
        close(jcli[i].sock);
        free(retval);
    }

    pthread_exit("SUCCESS");
}
