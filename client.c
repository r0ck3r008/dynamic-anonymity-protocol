#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<errno.h>

int server_sock, db_sock;
RSA *ku, *kv;

//prototypes
void *allocate(char *, int);
int init(int);
int server_init(char *);
RSA *gen_keys(char *, int);
int dbconnect(char *);

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
    if(argc!=3)
    {
        fprintf(stderr, "\n[!]Usage:\n./dbinterface [ip_to_bind:port_to_bind] [pub_key.pem] [priv_key.pem] [db_inetface_ip:port]\n");
        return 1;
    }

    return 0;
}

int server_init(char *argv1)
{
    int s;
    char *ip=(char *)allocate("char", 20);
    ip=strtok(argv2, ":");
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

    return s;
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

    return rsa;
}

int dbconnect(char *argv2)
{
    int s;
    char *ip=(char *)allocate("char", 20);
    ip=strtok(argv4, ":");
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
    free(ip);
    return s;
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

    if((ku=gen_keys(argv[2], 1))==NULL || (kv=gen_keys(argv[3], 0))==NULL)
    {
        _exit(-1);
    }

    if((db_sock=dbconnect(argv[4]))==0)
    {
        _exit(-1);
    }
}
