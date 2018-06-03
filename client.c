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
#include<sodium.h>
#include<errno.h>
#include<pthread.h>

RSA *ku, *kv, *p_ku;
int peer_sock;
int peer_id;
int db_sock;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

//prototypes
int init(int);
void *allocate(char *, int);
int dbconnect(char *);
char *get_peer_addr(int *);
char *query_db(char *, int);
int connect_to_peer(char *);
int peer_key_xcg(int, char *);
RSA *gen_keys(char *, int);
int init_server(char *);
void *server_run(void *);
void *peer_run(void *);
int send_to_peer(char *);
int client_workings();
char *encrypt_to_peer(char *);
char *decrypt_from_peer(char *);

int init(int argc)
{
    if(argc!=5)
    {
        fprintf(stderr, "\n[!]Usage:./client [db-server-ip:db_server:port] [client-pub-key.pem] [client-priv-key.pem] [ip-to-bind:port-to-bind]\n");
        return 1;
    }

    if(sodium_init())
    {
        fprintf(stderr, "\n[-]Error in initiating libsodium library\n");
        return 1;
    }

    return 0;
}

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

int dbconnect(char *argv1)
{
    char *ip=(char *)allocate("char", 20);
    ip=strtok(argv1, ":");
    int port=(int)strtol(strtok(NULL, ":"), NULL, 10);
    int s;
    struct sockaddr_in db_addr;
    db_addr.sin_family=AF_INET;
    db_addr.sin_port=htons(port);
    db_addr.sin_addr.s_addr=inet_addr(ip);

    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating db_sock: %s\n", strerror(errno));
        return 0;
    }

    if(connect(s, (struct sockaddr *)&db_addr, sizeof(db_addr))<0)
    {
        fprintf(stderr, "\n[-]Error in connecting to db server: %s\n", strerror(errno));
        return 0;
    }

    free(ip);
    return s;
}

char *get_peer_addr(int *peer_id)
{
    char *peer_addr=(char*)allocate("char", 20);
    char *query=(char*)allocate("char", 512);
    char *cmdr;

    sprintf(query, "1:select count(*) from peers;");
    if((cmdr=query_db(query, 1))==NULL)
    {
        return NULL;
    }
    int count=(int)strtol(cmdr, NULL, 10);
    explicit_bzero(query, 512*sizeof(char));

    int rand=(int)randombytes_uniform(count);
    sprintf(query, "1:select id, ip from peers where sno=%d;", rand);
    if((cmdr=query_db(query,1))==NULL)
    {
        return NULL;
    }
    *peer_id=(int)strtol(strtok(cmdr, ":"), NULL, 10);
    sprintf(peer_addr, "%s", strtok(NULL, ":"));

    free(query);
    free(cmdr);
    return peer_addr;
}

char *query_db(char *query, int rcv)
{
    char *cmdr=(char *)allocate("char", 512);

    if(send(db_sock, query, sizeof(char)*512, 0)<0)
    {
        fprintf(stderr, "\n[-]error in sending query %s db server: %s\n", query, strerror(errno));
        return NULL;
    }

    if(rcv)
    {
        if(recv(db_sock, cmdr, sizeof(char)*512, 0)<0)
        {
            fprintf(stderr, "\n[-]Error in receving from dbserver for query %s: %s\n", query, strerror(errno));
            return NULL;
        }
        return cmdr;
    }
    return NULL;
}

int connect_to_peer(char *peer_ip)
{
    int peer_sock;
    if((peer_sock=socket(AF_INET, SOCK_STREAM, 0))==-1)
    {
        fprintf(stderr, "\n[-]Error in creating socket for peer: %s\n", strerror(errno));;
        return 0;
    }

    struct sockaddr_in peer_addr;
    peer_addr.sin_port=htons(12345);
    peer_addr.sin_family=AF_INET;
    peer_addr.sin_addr.s_addr=inet_addr(peer_ip);

    if(connect(peer_sock, (struct sockaddr*)&peer_addr, sizeof(peer_addr))<0)
    {
        fprintf(stderr, "\n[-]Error in connecting to peer: %s\n", strerror(errno));
        return 0;
    }

    return peer_sock;
}

int peer_key_xcg(int peer_sock, char *pubname)
{
    char *cmds=(char *)allocate("char", 2048);
    char *cmdr=(char *)allocate("char", 4096);

    FILE *f;
    if((f=fopen(pubname, "r"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening %s file: %s\n", pubname, strerror(errno));
        return 1;
    }
    for(int i=0; !feof(f); i++)
    {
        fscanf(f, "%c", &cmds[i]);
    }
    if(send(peer_sock, cmds, 2048*sizeof(char), 0)<0)
    {
        fprintf(stderr, "\n[-]Error in sending pubkey to peer: %s\n", strerror(errno));
        return 1;
    }
    explicit_bzero(cmds, 2048*sizeof(char));
    fclose(f);

    if(recv(peer_sock, cmdr, 4096*sizeof(char), 0)<0)
    {
        fprintf(stderr, "\n[-]Error in recving pubkey from peer: %s\n", strerror(errno));
        return 1;
    }
    if((f=fopen("peer_ku", "w"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening peer_ku file for writing: %s\n", strerror(errno));
        return 1;
    }
    for(int i=0; i<strlen(cmdr); i++)
    {
        fprintf(f, "%c", cmdr[i]);
    }
    fclose(f);

    if((p_ku=gen_keys("peer_ku", 1))==NULL)
    {
        fprintf(stderr, "\n[-]Error in generating peer key\n");
        return 1;
    }
    
    free(cmds); free(cmdr);
    return 0;
}

RSA *gen_keys(char *fname, int pub)
{
    FILE *f;
    if((f=fopen(fname, "rb"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening file %s: %s\n", strerror(errno));
        return NULL;
    }

    RSA *rsa=RSA_new();

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

int init_server(char *ip_port)
{
    int serve_sock;
    if((serve_sock=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating server socket: %s\n", strerror(errno));
        return 1;
    }

    char *ip=(char*)allocate("char", 20);
    ip=strtok(ip_port, ":");
    int port=(int)strtol(strtok(NULL, ":"), NULL, 10);

    struct sockaddr_in addr;
    addr.sin_port=htons(port);
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(ip);

    if(bind(serve_sock, (struct sockaddr*)&addr, sizeof(addr))<0)
    {
        fprintf(stderr, "\n[-]Error in binding to %s:%d for server: %s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), strerror(errno));
        return 1;
    }

    if(listen(serve_sock, 5)<0)
    {
        fprintf(stderr, "\n[-]Error in listning to %s:%d: %s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), strerror(errno));
        return 1;
    }

    pthread_t serve_thr;
    int stat;
    if((stat=pthread_create(&serve_thr, NULL, server_run, &serve_sock))!=0)
    {
        fprintf(stderr, "\n[-]Error in initialising server thread: %s\n", strerror(stat));
        return 1;
    }
    if((stat=pthread_join(serve_thr, NULL))!=0)
    {
        fprintf(stderr, "\n[-]Error in joining to server thread: %s\n", strerror(stat));
        return 1;
    }

    free(ip);
    return 0;
}

void *server_run(void *s)
{
    int serve_sock= *(int *)s, count_cxns=0;

    pthread_t tid[10];
    for(; ; count_cxns++)
    {
        int s;
        if((s=accept(serve_sock, NULL, NULL))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting msg from peer %d: %s\n", count_cxns, strerror(errno));
            continue;
        }

        int stat;
        if((stat=pthread_create(&tid[count_cxns], NULL, peer_run, &s))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating peer thread %d: %s\n", count_cxns, strerror(errno));
            continue;
        }
    }

    for(int i=0; i<count_cxns; i++)
    {
        int stat;
        if((stat=pthread_join(tid[i], NULL))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining %d thread: %s\n", i, strerror(stat));
            pthread_exit(NULL);
        }
    }

    pthread_exit(NULL);
}

void *peer_run(void *s)
{
    int sock= *(int *)s;
    char *cmdr=(char *)allocate("char", 512);
    
    if(recv(sock, cmdr, 512*sizeof(char), 0)<0)
    {
        fprintf(stderr, "\n[-]Error in recving: %s\n", strerror(errno));
        pthread_exit(NULL);
    }

    if(send_to_peer(cmdr))
    {
        pthread_exit(NULL);
    }

    free(cmdr);
    pthread_exit(NULL);
}

int send_to_peer(char *cmd)
{
    //critical section
    pthread_mutex_lock(&mutex);

    if(send(peer_sock, cmd, 512*sizeof(char), 0)<0)
    {
        fprintf(stderr, "\n[-]Error in sending to peer: %s\n", strerror(errno));
        return 1;
    }

    pthread_mutex_unlock(&mutex);

    return 0;
}

int client_workings()
{
    char *cmds_en;
    char *cmds=(char *)allocate("char", 100);
    char *cmdr;
    char *cmdr_en=(char *)allocate("char", 512);
    printf("\n[>]Enter what to send(max 50 char): ");
    fgets(cmds, 100*sizeof(char), stdin);

    //append the anon-network prefixes
    sprintf(cmds, "%d:%d:%s\n", peer_id, 0, cmds);
    if((cmds_en=encrypt_to_peer(cmds))==NULL)
    {
        return 1;
    }

    if(send_to_peer(cmds_en))
    {
        return 1;
    }

    //recv from peer own packet reply
    if(recv(peer_sock, cmdr_en, 512*sizeof(char), 0)<0)
    {
        fprintf(stderr, "\n[-]Error in receving response from peer: %s\n", strerror(errno));
        return 1;
    }

    if((cmdr=decrypt_from_peer(cmdr_en))==NULL)
    {
        return 1;
    }

    printf("\n[!]Received: %s\n", cmdr);

    free(cmds); free(cmds_en);
    free(cmdr); free(cmdr_en);
    return 0;
}

char *encrypt_to_peer(char *in)
{
    char *cmds_en=(char*)allocate("char", 512);
    if(RSA_public_encrypt(RSA_size(p_ku)-11, in, cmds_en, p_ku, RSA_PKCS1_PADDING)<0)
    {
        fprintf(stderr, "\n[-]Error in encrypting: %s\n", strerror(ERR_get_error()));
        return NULL;
    }

    return cmds_en;
}

char *decrypt_from_peer(char *cmdr_en)
{
    char *cmdr=(char *)allocate("char", 512);

    if(RSA_private_decrypt(RSA_size(ku), cmdr_en, cmdr, kv, RSA_PKCS1_PADDING)<0)
    {
        fprintf(stderr, "\n[-]Error in decrypting peer's reply: %s\n", strerror(ERR_get_error()));
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

    if((db_sock=dbconnect(argv[1]))==0)
    {
        _exit(-1);
    }

    char *peer_addr;
    if((peer_addr=get_peer_addr(&peer_id))==NULL)
    {
        _exit(-1);
    }

    if((peer_sock=connect_to_peer(peer_addr))==0)
    {
        _exit(-1);    
    }
    
    if((ku=gen_keys(argv[2], 1))==NULL || (kv=gen_keys(argv[3], 0))==NULL)
    {
        _exit(-1);
    }

    if(peer_key_xcg(peer_sock, argv[2]))
    {
        _exit(-1);
    }
    printf("\n[!]Connected to peer and exchanged keys\n");

    if(init_server(argv[4]))
    {
        _exit(-1);
    }

    if(client_workings())
    {
        _exit(-1);
    }

    free(peer_addr);
    return 0;
}
