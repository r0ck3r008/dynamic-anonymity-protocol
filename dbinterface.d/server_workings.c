#include"server_workings.h"
#include"global_defs.h"
#include"server_init.h"
#include"peer_server.h"
#include"cli_server.h"
#include<stdio.h>
#include<string.h>
#include<pthread.h>

int server_workings(char *argv1, char *argv2)
{
    int stat;
    pthread_t psock_thr, csock_thr;
    char *retval;

    if((server_psock=server_init(argv1))==-1)
    {
        return 1;
    }
    if((server_csock=server_init(argv2))==-1)
    {
        return 1;
    }

    if((stat=pthread_create(&psock_thr, NULL, peer_server, NULL))!=0)
    {
        fprintf(stderr, "\n[-]Error in creating peer_sock thread: %s\n", strerror(stat));
        return 1;
    }
    if((stat=pthread_create(&csock_thr, NULL, cli_server, NULL))!=0)
    {
        fprintf(stderr, "\n[-]Error in creating client_sock thread: %s\n", strerror(stat));
        return 1;
    }

    if((stat=pthread_join(psock_thr, (void **)&retval))!=0)
    {
        fprintf(stderr, "\n[-]Error in joining to psock thr: %s\n", strerror(stat));
        return 1;
    }
    printf("\n[!]psock_thr exited with status: %s\n", (char *)retval);

    if((stat=pthread_join(csock_thr, (void **)&retval))!=0)
    {
        fprintf(stderr, "\n[-]Error in joining to csock thr: %s\n", strerror(stat));
        return 1;
    }
    printf("\n[!]csock_thr exited with status: %s\n", (char *)retval);

    return 0;

}
