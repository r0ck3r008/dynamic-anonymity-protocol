#define NEEDS_JOINEE_GLOBALS

#include"peer_server.h"
#include"global_defs.h"
#include"peer_run.h"
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<errno.h>

void *peer_server(void *a)
{
    pthread_t tid[10];
    struct joinee jpeer[10];
    socklen_t len=sizeof(struct sockaddr_in);
    int stat;
    char *retval;

    for(peer_count=0; peer_count<10; peer_count++)
    {
        if((jpeer[peer_count].sock=accept(server_csock, (struct sockaddr *) &jpeer[peer_count].addr, &len))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting peer %d for peer_server: %s\n", peer_count, strerror(errno));
            continue;
        }

        jpeer[peer_count].peer=1;

        if((stat=pthread_create(&tid[peer_count], NULL, peer_run, &jpeer[peer_count]))!=0)
        {
            fprintf(stderr, "\n[-]Error in starting new thr in p_server for %s:%d: %s\n", inet_ntoa(jpeer[peer_count].addr.sin_addr), ntohs(jpeer[peer_count].addr.sin_port), strerror(stat));
            continue;
        }
    }

    for(int i=0; i<peer_count; i++)
    {
        if((stat=pthread_join(tid[i], (void **)&retval))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to peer %s:%d p_sock: %s\n", inet_ntoa(jpeer[i].addr.sin_addr), ntohs(jpeer[i].addr.sin_port), strerror(stat));
            continue;
        }

        printf("\n[!]Peer %s:%d exited with status: %s\n", inet_ntoa(jpeer[i].addr.sin_addr), ntohs(jpeer[i].addr.sin_port), (char *)retval);
        close(jpeer[i].sock);
    }

    free(retval);
    pthread_exit("SUCCESS");
}
