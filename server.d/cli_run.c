#include"cli_run.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>
#include"allocate.h"
#include"server_init.h"

void *cli_run(void *c)
{
    struct client cli=*(struct client *)c;
    char *cmdr=(char *)allocate("char", 2048);
    char *cmds=(char *)allocate("char", 2048);

    printf("\n[!]Accepted client %s:%d\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port));

    if(recv(cli.sock, cmdr, sizeof(char)*2048, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in receving from client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
        pthread_exit("ERROR IN RECEVING");
    }

    sprintf(cmds, "FROM SERVER:");
    strcat(cmds, cmdr);

    if(send(cli.sock, cmds, sizeof(char)*2048, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in sending %s back to %s:%d: %s\n", cmds, inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
        pthread_exit("ERROR IN SENDING");
    }

    close(cli.sock);
    free(cmdr);
    free(cmds);
    pthread_exit("SUCCESS");
}


