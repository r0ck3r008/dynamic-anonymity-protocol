#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<openssl/rsa.h>
#include<openssl/pem.h>
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
    for(int i=0; !feof(f); i++)
    {
        fscanf(f, "%c", &cmds[i]);
    }
    sprintf(cmds, "1:0:%s", cmds);    //1 for insert, zero for expected return columns

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
