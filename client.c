#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<openssl/pem.h>
#include<openssl/rsa.h>
#include<sodium.h>
#include<pthread.h>
#include<errno.h>

struct peer
{
    int sock, id;
    struct sockaddr_in addr;
    RSA *ku;
    char *ku_fname;
};
struct peer_combo
{
    struct peer p;
    int rand_sno;
} pc[2];
int server_sock, db_sock;
RSA *ku, *kv;
struct peer dest_peer;

//prototypes
void *allocate(char *, int);
int init(int);
int server_init(char *);
RSA *gen_keys(char *, int);
int dbconnect(char *);
int get_rand_sno();
int gen_rand_peer(struct peer *, int, char *);
int snd(int, char *, char *);
char *rcv(int, char *);

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
    ip=strtok(argv1, ":");
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

int dbconnect(char *argv4)
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

int get_rand_sno()
{
    char *query=(char *)allocate("char", 2048);
    char *cmdr;
    sprintf(query, "0:1:select counnt(*) from peers;");
    
    if(snd(db_sock, query, "query to count number of peers"))
    {
        return -1;
    }

    if((cmdr=rcv(db_sock, "to receive the number of peers from dbinterface"))==NULL)
    {
        return -1;
    }

    int peer_count=(int)strtol(cmdr, NULL, 10);
    int rand_sno=(int)randombytes_uniform(peer_count);

    free(cmdr);
    return rand_sno;
}

int get_rand_peer(struct peer *p, int rand_sno, char *ku_fname)
{
    char *query=(char *)allocate("char", 2048), *key_str, *cmdr, *ip=(char *)allocate("char", 20);
    FILE *f;

    sprintf(query, "0:2:select id, ip, ku from peers where sno=%d;", rand_sno);

    if(snd(db_sock, query, "get ip, id, and key from db_inteface\n"))
    {
        return 1;
    }

    if((cmdr=rcv(db_sock, "receive id, ip, key from peers via rand_sno\n"))==NULL)
    {
        return 1;
    }

    if((f=fopen(ku_fname, "w"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening file %s: %s\n", ku_fname, strerror(errno));
        return 1;
    }
    p->ku_fname=ku_fname;
    p->id=(int)strtol(strtok(cmdr, ":"), NULL, 10);
    sprintf(ip, "%s", strtok(NULL, ":"));
    p->addr.sin_addr.s_addr=inet_addr(ip);
    key_str=strtok(NULL, ":");
    for(int i=0; i<strlen(key_str); i++)
    {
        fprintf(f, "%c", key_str[i]);
    }
    fclose(f);
    if((p->ku=gen_keys(ku_fname, 1))==NULL)
    {
        return 1;
    }

    free(cmdr);
    return 0;
}

int snd(int sock, char *cmds, char *reason) //this function frees the cmds
{
    if(send(sock, cmds, sizeof(char)*2048, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in sending for reeason: %s: %s\n", reason, strerror(errno));
        return 1;
    }

    free(cmds);
    return 0;
}

char *rcv(int sock, char *reason)   //cmdr is freeed by callee
{
    char *cmdr=(char *)allocate("char", 2048);

    if(recv(sock, cmdr, sizeof(char)*2048, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in receving for reason %s: %s\n", reason, strerror(errno));
        return NULL;
    }

    return cmdr;
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

    //for main peer;
    if((pc[0].rand_sno=get_rand_sno())==-1)
    {
        _exit(-1);
    }
    if(get_rand_peer(&pc[0].p, pc[0].rand_sno, "const_peer_ku.pem"))
    {
        _exit(-1);
    }
}
