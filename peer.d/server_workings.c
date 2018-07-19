#define NEEDS_JOINEE_STRUCT

#include"server_workings.h"
#include"server_init.h"
#include"cli_server.h"
#include"peer_server.h"
#include"global_defs.h"
#include"common_headers/allocate.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<errno.h>

int server_workings(char *argv1, char *argv2)
{
    int stat;
    pthread_t cli_thr, peer_thr;
    char *retval;
    clients=(struct client *)allocate("struct client", 100);

    if((server_psock=server_init(argv1)))
    {
        return 1;
    }
    if((server_csock=server_init(argv2)))
    {
        return 1;
    }

    if((stat=pthread_create(&cli_thr, NULL, cli_server, NULL))!=0)
    {
        fprintf(stderr, "\n[-]Error in starting client server thread: %s\n", strerror(stat));
        return 1;
    }
    if((stat=pthread_create(&peer_thr, NULL, peer_server, NULL))!=0)
    {
        fprintf(stderr, "\n[-]Error in starting server_thread for peer: %s\n", strerror(stat));
        return 1;
    }

    if((stat=pthread_join(cli_thr, (void **)&retval))!=0)
    {
        fprintf(stderr, "\n[-]Error in joining to cli_server thread: %s\n", strerror(stat));
        return 1;
    }
    printf("\n[!]Client server thread exited with status: %s\n", (char *)retval);

    if((stat=pthread_join(peer_thr, (void **)&retval))!=0)
    {
        fprintf(stderr, "\n[-]Error in joining to peer server thread: %s\n", strerror(stat));
        return 1;
    }
    printf("\n[!]peer server returned with status: %s\n", (char *)retval);

    return 0;
}
