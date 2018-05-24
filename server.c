#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<pthread.h>

struct client
{
    struct sockaddr_in addr;
    int id,sock;
};

void *allocate(char *type, int size)
{
    void *ret=NULL;
    if(strcmp(type, "char")==0)
    {
        ret=malloc(size*sizeof(char));
        explicit_bzero(ret, size*sizeof(char));
    }

    if(ret==NULL)
    {
        fprintf(stderr, "\n[-]Error in allocating %d bytes for %s type\n", size, type);
        _exit(-1);
    }

    return ret;
}

void *cli_run(void *c)
{
    struct client cli=*(struct client *)c;

    printf("\n[!]Accepted client at %s:%d\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port));

    char *cmdr=(char*)allocate("char", 512);
    char *cmds=(char*)allocate("char", 512);

    if(recv(cli.sock, cmdr, sizeof(char)*512, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in receving from client at %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
        pthread_exit(NULL);
    }
    
    sprintf(cmds, "Hello from server\n");
    if(send(cli.sock, cmds, sizeof(char)*512, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in sending to client at %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
        pthread_exit(NULL);
    }
    free(cmdr);free(cmds);

    printf("\n[!]Tansaction with client at %s:%d over\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port));

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        fprintf(stderr, "\n[!]Usage:\n./server [ip-to-bind:port-to-bind]\n");
        _exit(-1);
    }

    int sock;
    if((sock=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating socket: %s\n", strerror(errno));
        _exit(-1);
    }

    char *ip=(char*)allocate("char", 50);
    ip=strtok(argv[1], ":");
    int port=(int)strtol(strtok(NULL, ":"), NULL, 10);

    struct sockaddr_in addr;
    addr.sin_port=htons(port);
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(ip);

    if(bind(sock, (struct sockaddr*)&addr, sizeof(addr))<0)
    {
        fprintf(stderr, "\n[-]Error in binding: %s\n", strerror(errno));
        _exit(-1);
    }

    if(listen(sock, 5)<0)
    {
        fprintf(stderr, "\n[-]Error in linsning: %s\n", strerror(errno));
        _exit(-1);
    }

    struct client cli[10];
    pthread_t tid[10];
    socklen_t len=sizeof(addr);

    for(int i=0; ; i++)
    {
        int s;
        if((s=accept(sock, (struct sockaddr*)&cli[i].addr, &len))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d: %s\n", i, strerror(errno));
            continue;
        }
        cli[i].id=i;
        cli[i].sock=s;
        
        int stat;
        if((stat=pthread_create(&tid[i], NULL, cli_run, &cli[i]))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating thread %d: %s\n", i, strerror(stat));
            continue;
        }
    }

    free(ip);
}
