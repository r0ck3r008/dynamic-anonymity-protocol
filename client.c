#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<openssl/rsa.h>
#include<openssl/pem.h>
#include<sodium.h>
#include<errno.h>
#include<termios.h>
#include<mysql/mysql.h>
#include<pthread.h>

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
char *uname="root", *dbname="anon", *passwd;
RSA *ku, *kv, *p_ku;
int peer_sock;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALISER;

//prototypes
int init();
void *allocate(char *, int);
char *get_pass();
int dbconnect(char *);
char *get_peer_addr(int *);
int query_db(char *);
int connect_to_peer(char *);
int peer_key_xcg(int, char *);
RSA *gen_keys(char *, int);
int init_server(char *);
void *server_run(void *);
void *peer_run(void *);
int send_to_peer(char *);

int init(int argc)
{
    if(argc!=5)
    {
        fprintf(stderr, "\n[!]Usage:./client [db-server-ip] [client-pub-key.pem] [client-priv-key.pem] [ip-to-bind:port-to-bind]\n");
        return 1;
    }

    if(mysql_library_init(0, NULL, NULL))
    {
        fprintf(stderr, "\n[-]Error in initiating mysql library\n");
        return 1;
    }

    if(sodium_init())
    {
        fprintf(stderr, "\n[-]Error in initiating libsodium library\n");
        return 1;
    }
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

char *get_pass()
{
    char *pass=(char*)allocate("char", 50);
    struct termios obj1, obj2;

    if(tcgetattr(fileno(stdin), &obj1)<0)
    {
        fprintf(stderr, "\n[-]Error in getting stdin attributes: %s\n", strerror(errno));
        _exit(-1);
    }

    obj2=obj1;
    obj2.c_lflag &= ~ECHO;
    obj2.c_lflag |= ECHONL;

    if(tcsetattr(fileno(stdin), TCSANOW,  &obj2)<0)
    {
        fprintf(stderr, "\n[-]Error in setting stding ~ECHO attribute: %s\n", strerror(errno));
        _exit(-1);
    }

    fgets(pass, sizeof(char)*20, stdin);

    if(tcsetattr(fileno(stdin), TCSANOW, &obj1)<0)
    {
        fprintf(stderr, "\n[-]Error in setting stding backto normal: %s\n", strerror(errno));
        _exit(-1);
    }
    return pass;
}

int dbconnect(char *server)
{
    if((conn=mysql_init(NULL))==NULL)
    {
        fprintf(stderr, "\n[-]Error in initiating connection\n");
        return 1;
    }

    if(!mysql_real_connect(conn, server, uname, passwd, dbname, 0, NULL, 0))
    {
        fprintf(stderr, "\n[-]Error in conneting to db: %s\n", mysql_error(conn));
        return 1;
    }
    printf("\n[!]Connected to database\n");
    
    return 0;
}

char *get_peer_addr(int *peer_id)
{
    char *peer_addr=(char*)allocate("char", 20);
    char *query=(char*)allocate("char", 100);

    sprintf(query, "select count(*) from peers;");
    if(query_db(query))
    {
        return NULL;
    }
    int count;
    while((row=mysql_fetch_row(res))!=NULL)
    {
        count=(int)strtol(row[0], NULL, 10);
    }
    explicit_bzero(query, 100*sizeof(char));

    int rand=(int)randombytes_uniform(count);
    sprintf(query, "select id, ip from peers where sno=%d;", rand);
    if(query_db(query))
    {
        return NULL;
    }
    while((row=mysql_fetch_row(res))!=NULL)
    {
        sprintf(ip, "%s", row[1]);
        *peer_id=(int)strtol(row[0], NULL, 10);
    }

    free(query);
    return peer_addr;
}

int query_db(char *query)
{
    if(mysql_query(conn, query))
    {
        fprintf(stderr, "\n[-]Error in sending %s: %s\n", query, mysql_error(conn));
        return 1;
    }
    res=mysql_use_result(conn);
    if(res==NULL)
    {
        fprintf(stderr, "\n[-]Result is null\n");
        return 1;
    }
    return 0;
}

int connect_to_peer(char *peer_addr)
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
    peer_addr.sin_addr.s_addr=inet_addr(peer_addr);

    if(connect(peer_sock, (struct sockaddr*)&addr, sizeof(addr))<0)
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

    p_ku=gen_keys("peer_ku", 1);
    
    free(cmds); free(cmdr);

    return 0;
}

RSA *gen_keys(char *fname, int pub)
{
    FILE *f;
    if((f=fopen(fname, "rb"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening file %s: %s\n", strerror(errno));
        _exit(-1);
    }

    RSA *rsa=RSA_new();

    if(pub)
    {
        rsa=PEM_read_RSA_PUBKEY(f, &rsa, NULL, NULL);
    }
    else
    {
        rsa=PEM_read_RSA_PrivateKey(f, &rsa, NULL, NULL);
    }

    close(f);
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
    free(ip);
    return 0;
}

void *server_run(void *s)
{
    int serve_sock= *(int *)s;

    pthread_t tid[10];
    for(int i=0; ; i++)
    {
        int s;
        if((s=accept(serve_sock, NULL, NULL))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting msg from peer %d: %s\n", i, strerror(errno));
            continue;
        }

        int stat;
        if((stat=pthread_create(&tid[i], NULL, peer_run, &s))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating peer thread %d: %s\n", i, strerror(errno));
            continue;
        }
    }

    pthread_exit(NULL);
}

void *peer_run(void *s)
{
    int sock= *(int *)s;

    char *cmdr=(char *)allocate(512);
    
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

int main(int argc, char *argv[])
{
    if(init(argc))
    {
        _exit(-1);
    }

    printf("\n[>]Enter db password: ");
    passwd=get_pass();

    if(dbconnect(argv[1]))
    {
        _exit(-1);
    }

    int peer_id;
    char *peer_addr;
    if((peer_addr=get_peer_addr(&peer_id))==NULL)
    {
        _exit(-1);
    }

    if((peer_sock=connect_to_peer(peer_addr))==0)
    {
        _exit(-1);    
    }
    
    ku=gen_keys(pubname, 1);
    kv=gen_keys(privname, 0);
    if(ku==NULL || kv==NULL)
    {
        fprintf(stderr, "\n[-]Error in generating keys\n");
        return 1;
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

    free(passwd); 
    mysql_close(conn);
    free(peer_addr);
}
