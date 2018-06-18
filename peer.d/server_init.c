#include"server_init.h"
#include"server_run.h"
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<errno.h>

int server_init(char *argv1)
{
    int s;
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

    printf("\n[!]listning on %s:%d...\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    return s;
}

int server_workings()
{
    pthread_t server_thr;
    int stat;
    char *retval;

    if((stat=pthread_create(&server_thr, NULL, server_run, "..."))!=0)
    {
        fprintf(stderr, "\n[-]Error in starting server thread: %s\n", strerror(stat));
        return 1;
    }

    if((stat=pthread_join(server_thr, (void **)retval))!=0)
    {
        fprintf(stderr, "\n[-]Error in joining to server thread: %s\n", strerror(stat));
        return 1;
    }
    printf("\n[!]Server thread returned with status %s\n", (char *)retval);
}

