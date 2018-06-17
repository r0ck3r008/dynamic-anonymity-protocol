#include"cli_run.h"
#include"allocate.h"
#include"db_workings.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<string.h>
#include<errno.h>
#include<pthread.h>

void *cli_run(void *c)
{
    struct client cli=*(struct client *)c;
    char *cmdr=(char *)allocate("char", 2048);
    char *cmds;
    int bit;

    printf("\n[!]Accepted from %s:%d\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port));
    while(1)
    {
        if(recv(cli.sock, cmdr, sizeof(char)*2048, 0)<0)
        {
            fprintf(stderr, "\n[-]Error in receving from client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
            pthread_exit("ERROR IN RECEVING");
        }
        if(strcmp(cmdr, "DONE")==0)
        {
            printf("\n[!]Exiting client %s:%d\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port));
            if(send(cli.sock, "OK", sizeof(char)*strlen("OK"), 0)<0)
            {
                fprintf(stderr, "\n[-]Error in sending back ack to client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
                pthread_exit("ERROR IN SENDING ACK");
            }
            break;
        }

        if((cmds=db_workings(cli, cmdr))==NULL)
        {
            pthread_exit("ERROR IN DB_WORKINGS");
        }

        if(send(cli.sock, cmds, sizeof(char)*2048, 0)<0)
        {
            fprintf(stderr, "\n[-]Error in sending back to %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
            pthread_exit("ERROR IN SENDING");
        }
    }

    free(cmdr);free(cmds);
    pthread_exit("SUCCESS");
}

