#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<errno.h>

struct client
{
    int sock;
    struct sockaddr_in addr;
    int id;
};
int server_sock, cli_count=0;

//prototypes
void *allocate(char *, int);
int server_init(char *);
int server_workings();
void *cli_run(void *);

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

int init(int argc)
{
    if(argc!=2)
    {
        fprintf(stderr, "\n[!]Usage:\n./server [ip_to_bind:port_to_bind]\n");
        return 1;
    }

    return 0;
}

int server_init(char *argv1)
{
    char *ip=strtok(argv1, ":");
    int port=(int)strtol(strtok(NULL, ":"), NULL, 10), s;
    struct sockaddr_in addr;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr(ip);
    addr.sin_family=AF_INET;

    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating the server_socket: %s\n", strerror(errno));
        return 0;
    }

    if(bind(s, (struct sockaddr *)&addr, sizeof(addr))<0)
    {
        fprintf(stderr, "\n[-]bind: %s\n", strerror(errno));
        return 0;
    }

    if(listen(s, 5)<0)
    {
        fprintf(stderr, "\n[-]Listen: %s\n", strerror(errno));
        return 0;
    }

    return s;
}

int server_workings()
{
    pthread_t tid[10];
    struct client cli[10];
    socklen_t len=sizeof(struct sockaddr_in);
    int stat;
    char *retval;

    for(cli_count; cli_count<10; cli_count++)
    {
        if((cli[cli_count].sock=accept(server_sock, (struct sockaddr *)&cli[cli_count].addr, &len))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d: %s\n", cli_count, strerror(errno));
            continue;
        }

        cli[cli_count].id=cli_count;

        if((stat=pthread_create(&tid[cli_count], NULL, cli_run, &cli[cli_count]))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating thread for client at %s:%d: %s\n", inet_ntoa(cli[cli_count].addr.sin_addr), ntohs(cli[cli_count].addr.sin_port), strerror(stat));
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

        printf("\n[!]The client %s:%d exited with status %s\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), (char *)retval);
    }

    return 0;
}

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

int main(int argc, char *argv[])
{
    if(init(argc))
    {
        _exit(-1);
    }

    if((server_sock=server_init(argv[1]))==0)
    {
        _exit(-1);
    }

    if(server_workings())
    {
        _exit(-1);
    }
}
