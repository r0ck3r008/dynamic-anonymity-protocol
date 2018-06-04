#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<openssl/pem.h>
#include<openssl/rsa.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<errno.h>

struct client
{
    int sock;
    struct sockaddr_in addr;
    RSA *ku;
};
int server_sock;
int db_sock;
RSA *ku, *kv;
char *bound_ip, *ku_fname;

//prototypes
void *allocate(char *, int);
int init(int);
int server_init(char *);
int dbconnect(char *);
RSA *gen_keys(char *, int);
int server_workings();
void *server_run(void *);
int update_peers_table();

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
        fprintf(stderr, "\n[!] Usage:\n./peer [ip_to_bind:port_to_bind] [pub_key.pem] [priv_key.pem] [dbinterface_ip:port]\n");
        return 1;
    }

    return 0;
}

int server_init(char *argv1)
{
    int s;
    char *ip=(char *)allocate("char", 20);
    ip=strtok(argv1, ":");
    int port=(int)strtol(strtok(NULL, ":"), NULL, 10);
    struct sockaddr_in addr;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr(ip);
    addr.sin_family=AF_INET;

    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating serve_socket: %s\n", strerror(errno));
        return 0;
    }

    if(bind(s, (struct sockaddr *)&addr, sizeof(addr))<0)
    {
        fprintf(stderr, "\n[-]Error in binding the server_socket: %s\n", strerror(errno));
        return 0;
    }

    if(listen(s, 5)<0)
    {
        fprintf(stderr, "\n[-]Error in listning to server_sock: %s\n", strerror(errno));
        return 0;
    }

    free(ip);
    return s;
}

int dbconnect(char *argv4)
{
    char *ip=(char *)allocate("char", 20);
    struct sockaddr_in addr;
    int s;
    ip=strtok(argv4, ":");
    addr.sin_port=htons((int)strtol(strtok(NULL, ":"), NULL, 10));
    addr.sin_addr.s_addr=inet_addr(ip);

    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in craeting db_socket: %s\n", strerror(errno));
        return 0;
    }

    if(connect(s, (struct sockaddr *)&addr, sizeof(addr))<0)
    {
        fprintf(stderr, "\n[-]Error in connecting to db: %s\n", strerror(errno));
        return 0;
    }

    return s;
}

RSA *gen_keys(char *fname, int public)
{
    FILE *f;
    RSA *rsa=RSA_new();

    if((f=fopen(fname, "rb"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening %s file: 5s\n", fname, strerror(errno));
        return NULL;
    }

    if(public)
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

int server_workings()
{
    int stat;
    pthread_t server_thr;
    char *retval;

    if((stat=pthread_create(&server_thr, NULL, server_run, "..."))!=0)
    {
        fprintf(stderr, "\n[-]Error in initialising server thread: %s\n", strerror(stat));
        return 1;
    }

    if((stat=pthread_join(server_thr, (void **)&retval))!=0)
    {
        fprintf(stderr, "\n[-]Error in joining to server thread: %s\n", strerror(stat));
        return 1;
    }

    if(strcmp((char *)retval, "ERROR")==0)
    {
        return 1;
    }
    return 0;
}

void *server_run(void *a)
{
    if(update_peers_table())
    {
        pthread_exit("ERROR");
    }
}

int update_peers_table()
{
    char *cmds=(char *)allocate("char", 2048);
    char *cmdr=(char *)allocate("char", 512);
    char *ku_str=(char *)allocate("char", 1500);
    FILE *f;
    if((f=fopen(ku_fname, "r"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening %s file:%s\n", strerror(errno));
        return 1;
    }

    for(int i=0; !feof(f); i++)
    {
        fscanf(f, "%c", &ku_str[i]);
    }
    fclose(f);

    sprintf(cmds, "1:%s:%s", strtok(bound_ip, ":"), ku_str);

    if(send(db_sock, cmds, 2048*sizeof(char), 0)<0)
    {
        fprintf(stderr, "\n[-]Error in sending update query: %s\n", strerror(errno));
        return 1;
    }

    if(recv(db_sock, cmdr, 512*sizeof(char), 0)<0)
    {
        fprintf(stderr, "\n[-]Error in receving ack from dbinterface: %s\n", strerror(errno));
        return 1;
    }

    if(strcmp(cmdr, "SUCCESS")!=0)
    {
        fprintf(stderr, "\n[-]Error in adding\n");
        return 1;
    }

    free(cmds); free(cmdr); free(ku_str);
    return 0;
}

int main(int argc, char *argv[])
{
    if(init(argc))
    {
        _exit(-1);
    }

    bound_ip=argv[1];
    ku_fname=argv[2];
    if((server_sock=server_init(argv[1]))==0)
    {
        _exit(-1);
    }

    if((db_sock=dbconnect(argv[4]))==0)
    {
        _exit(-1);
    }

    if((ku=gen_keys(argv[2], 1))==NULL || (kv=gen_keys(argv[3], 0))==NULL)
    {
        _exit(-1);
    }

    if(server_workings())
    {
        _exit(-1);
    }
}
