#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<openssl/rsa.h>
#include<openssl/pem.h>
#include<openssl/err.h>
#include<pthread.h>
#include<errno.h>

struct client
{
    int sock;
    int fellow;
    struct sockaddr_in addr;
    RSA *ku;
    int id;
};
int server_sock, db_sock, cli_count=0;
RSA *ku, *kv;

//prototypes
void *allocate(char *, int);
int init(int);
int server_init(char *);
int server_workings();
void *server_run(void *);
void *cli_run(void *);
int snd(int, char *, char *);
char *rcv(int, char *);
int connect_to_fellow_peer(struct client *);
RSA *gen_keys(char *, int);
int dbconnect(char *);
int update_existance_in_peers(char *);

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
    if(argc!=5)
    {
        fprintf(stderr, "\n[!]Usage:\n./peer [ip_to_bind:port_to_bind] [pub_key.pem] [priv_key.pem] [db_inetface_ip:port]\n");
        return 1;
    }

    return 0;
}

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

void *server_run(void *a)
{
    pthread_t tid[10];
    struct client cli[10];
    socklen_t len=sizeof(struct sockaddr_in);
    int stat;
    char *retval;

    printf("\n[!]Server thread initiated...\n");

    for(cli_count; cli_count<10; cli_count++)
    {
        if((cli[cli_count].sock=accept(server_sock, (struct sockaddr *)&cli[cli_count].addr, &len))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d: %s\n", cli_count, strerror(errno));
            continue;
        }

        if((stat=pthread_create(&tid[cli_count], NULL, cli_run, &cli[cli_count]))!=0)
        {
            fprintf(stderr, "\n[-]Error in initiating client thread at %s:%d: %s\n", inet_ntoa(cli[cli_count].addr.sin_addr), ntohs(cli[cli_count].addr.sin_port), strerror(stat));
            continue;
        }
    }

    for(int i=0; i<cli_count; i++)
    {
        if((stat=pthread_join(tid[i], (void **)&retval))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to client %s:%d: %s\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), strerror(stat));
            continue;
        }
        printf("\n[!]Client %s:%d returned %s\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), (char *)retval);
    }

    pthread_exit("SUCCESS");
}

void *cli_run(void *c)
{
    struct client cli=*(struct client *)c;
    char *cmds=(char *)allocate("char", 2048), *cmdr;
    int bit;

    printf("\n[!]Now handelling client %s:%d\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port));
    
    if((cmdr=rcv(cli.sock, "to receive first packet\n"))==NULL)
    {
        pthread_exit("ERROR IN RECEVING");
    }
    
    bit=(int)strtol(strtok(cmdr, ":"), NULL, 10);
    if(bit)
    {
        cli.fellow=1;
    }
    else
    {
        struct client fellow;
        char *cmdr_en;
        cli.fellow=0;
        fellow.addr.sin_addr.s_addr=inet_addr(strtok(NULL, ":"));
        fellow.addr.sin_port=htons(6666);
        fellow.addr.sin_family=AF_INET;
        cmdr_en=strtok(NULL, ":");

        if(connect_to_fellow_peer(&fellow))
        {
            pthread_exit("ERROR IN CONNECTING TO FELLOW PEER");
        }

        if(snd(fellow.sock, cmdr_en, "send encrypted msg to fellow peer"))
        {
            pthread_exit("ERROR IN SENDING");
        }
    }

    free(cmdr);
    pthread_exit("SUCCESS");
}

int snd(int sock, char *cmds, char *reason)
{
    if(send(sock, cmds, sizeof(char)*2048, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in sending for %s: %s\n", reason, strerror(errno));
        return 1;
    }

    free(cmds);
    return 0;
}

char *rcv(int sock, char *reason)
{
    char *cmdr=(char *)allocate("char", 2048);

    if(recv(sock, cmdr, sizeof(char)*2048, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in receving %s: %s\n", reason, strerror(errno));
        return NULL;
    }

    return cmdr;
}

int connect_to_fellow_peer(struct client *fell)
{
    int s;

    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating socket for fellow peer: %s\n", strerror(errno));
        return 1;
    }

    if(connect(s, (struct sockaddr *)&(fell->addr), sizeof(fell->addr))<0)
    {
        fprintf(stderr, "\n[-]Error in connecting to fellow peer at %s: %s\n", inet_ntoa(fell->addr.sin_addr), strerror(errno));
        return 1;
    }

    fell->sock=s;
    return 0;
}

RSA *gen_keys(char *fname, int pub)
{
    FILE *f;
    RSA *rsa=RSA_new();

    if((f=fopen(fname, "rb"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening %s: %s\n", fname, strerror(errno));
        return NULL;
    }

    if(pub)
    {
        rsa=PEM_read_RSA_PUBKEY(f, &rsa, NULL, NULL);
    }
    else
    {
        rsa=PEM_read_RSAPrivateKey(f, &rsa, NULL, NULL);
    }

    fclose(f);
    return rsa;
}

int dbconnect(char *argv4)
{
    int s;
    char *ip=strtok(argv4, ":");
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons((int)strtol(strtok(NULL, ":"), NULL, 10));
    addr.sin_addr.s_addr=inet_addr(ip);

    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating db_sock: %s\n", strerror(errno));
        return 0;
    }

    if(connect(s, (struct sockaddr *)&addr, sizeof(addr))<0)
    {
        fprintf(stderr, "\n[-]Error in connecting to %s:%d: %s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), strerror(errno));
        return 0;
    }

    printf("\n[!]Connected to dbinterface\n");
    return s;
}

int update_existance_in_peers(char *ku_fname)
{
    char *cmds=(char *)allocate("char", 2048);
    char *cmdr=(char *)allocate("char", 512);
    FILE *f;
    if((f=fopen(ku_fname, "r"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening %s: %s\n", ku_fname, strerror(errno));
        return 1;
    }
    strcat(cmds, "1:0:");
    for(int i=4; !feof(f); i++)
    {
        fscanf(f, "%c", &cmds[i]);
    }
    //1 for insert, zero for expected return columns

    if(send(db_sock, cmds, sizeof(char)*2048, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in sending to the database interface: %s\n", strerror(errno));
        return 1;
    }

    if(recv(db_sock, cmdr, sizeof(char)*512, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in receving ack from dbinterface: %s\n", strerror(errno));
        return 1;
    }

    printf("\n[!]Received from dbinterface %s\n", cmdr);

    fclose(f);
    free(cmds);
    free(cmdr);
    return 0;
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

    if((ku=gen_keys(argv[2], 1))==NULL || (kv=gen_keys(argv[3], 0))==NULL)
    {
        _exit(-1);
    }

    if((db_sock=dbconnect(argv[4]))==0)
    {
        _exit(-1);
    }

    if(update_existance_in_peers(argv[2]))
    {
        _exit(-1);
    }
}
