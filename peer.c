#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<mysql/mysql.h>
#include<pthread.h>
#include<termios.h>
#include<errno.h>

struct client
{
    int sock;
    struct sockaddr_in addr;
    RSA *cli_ku;
};
int serve_sock, cli_count
MYSQL *conn;
MYSQL_ROW row;
MYSQL_RES *res;
pthread_mutex mutex= PTHREAD_MUTEX_INITIALIZER;
char *pub_fname;

//prototypes
void *alocate(char *, int);
int init(int);
int server_init(char *);
int dbconnect(char *);
char *get_passwd();
int server_workings();
void *server_run(void *);
void *client_run(void *);
int query_db(char *, MYSQL *);
int exchange_keys(struct client);

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
        fprintf(stderr, "\n[!]Usage: ./peer [ip-to-bind:port-to-bind] [path_to_pub_key.pem] [path_to_priv_key.pem] [addr-of-network-db]\n");
        return 1;
    }

    if(mysql_init(NULL))
    {
        fprintf(stderr, "\n[-]Error in initialising mysql library\n");
        return 1;
    }

    return 0;
}

int server_init(char *ip_port)
{
    char *ip=(char *)allocate("char", 20);
    ip=strtok(ip_port, ":");
    int port =(int)strtol(strtok(NULL, ":"), NULL, 10);

    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr(ip);

    int sock;
    if((sock=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating serve_sock: %s\n", strerror(errno));
        return 0;
    }

    if(bind(sock, (struct sockaddr*)&addr, sizeof(addr))<0)
    {
        fprintf(stderr, "\n[-]Error in binding server socket: %s\n", strerror(errno));
        return 0;
    }

    if(listen(sock, 5)<0)
    {
        fprintf(stderr, "\n[-]Listen error on server_sock: %s\n", strerror(errno));
        return 0;
    }

    free(ip);
    return sock;
}

int dbconnect(char *db_ip)
{
    char *uname="root", *passwd, *dbname="anon";

    printf("\n[>]Enter database passwd: ");
    if((passwd=get_passwd())==NULL)
    {
        return 1;
    }

    if((conn=mysql_init(NULL)))
    {
        fprintf(stderr, "\n[-]Error in connecting to mysql\n");
        return 1;
    }

    if(!mysql_real_connect(conn, db_ip, uname, passwd, dbname, 0, NULL, 0))
    {
        fprintf(stderr, "\n[-]Error in connecting to mysql library: %s\n", mysql_error(conn));
        return 1;
    }

    printf("\n[!]Connected to %s database\n", dbname);

    return 0;
    free(passwd);

}

char *get_passwd()
{
    char *passwd=(char *)allocate("char", 50);
    struct termios a,b;
    explicit_bzero(&a, sizeof(a));
    explicit_bzero(&b, sizeof(b));

    if(tcgetattr(fileno(stdin), &a)<0)
    {
        fprintf(stderr, "\n[-]Error in getting stdin attributes\n");
        return NULL;
    }

    b=a;
    a.c_lflag &= ~ECHO;
    a.c_lflag |= ECHONL;

    if(tcsetattr(fileno(stdin), TCSANOW, &a)<0)
    {
        fprintf(stderr, "\n[-]Error in setting stdin no-echo attr\n");
        return NULL;
    }

    fgets(passwd, sizeof(char)*50, stdin);

    if(tcsetattr(fileno(stdin), TCSANOW, &b)<0)
    {
        fprintf(stderr, "\n[-]Error in setting stdin back to normal\n");
        return NULL;
    }

    return passwd;
}

RSA *gen_keys(char *fname, int public)
{
    RSA *rsa=RSA_new();
    FILE *f;

    if((f=fopen(fname, "rb"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening file %s for generating keys\n", fname);
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

    return rsa;
}

int server_workings()
{
    pthread_t server_thr;
    int stat;

    if((stat=pthread_create(&server_thr, NULL, server_run, "..."))!=0)
    {
        fprintf(stderr, "\n[-]Error in creating server thread: %s\n", strerror(stat));
        return 1;
    }

    if((stat=pthread_join(&server_thr. NULL))<0)
    {
        fprintf(stderr, "\n[-]Error in joining to server thread: %s\n", strerror(stat));
        return 1;
    }

    return 0;
}

void *server_run(void *a)
{
    pthread_t tid[10];
    struct client cli[10];
    socklen_t len=sizeof(struct sockaddr_in);
    int stat;

    for(cli_clount=0; cli_count<10; cli_count++)
    {
        int s;
        if((s=accept(server_sock, (struct sockaddr*)&cli[cli_count].addr, &len))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d: %s\n", cli_count, strerror(stat));
            continue;
        }
        cli[cli_count].sock=s;

        if((stat=pthread_create(&tid[cli_count], NULL, cli_run, &cli[cli_count]))<0)
        {
            fprintf(stderr, "\n[-]Error in creating a thread for new client %d: %s\n", cli_count, strerror(stat));
            continue;
        }
    }

    for(int i=0; i<cli_count; i++)
    {
        if((stat=pthread_join(tid[i], NULL))<0)
        {
            fprintf(stderr, "\n[-]Error in joining to client %d: %s\n", i, strerror(stat));
            continue;
        }
    }

    pthread_exit(NULL);
}

void *cli_run(void *c)
{
    struct client cli= *(struct client *)c;
    printf("\n[!]Client connected at %s:%d\n", inet_ntoa(cli.addr.sin_addr), ntoh(cli.addr.sin_port));

    if(update_client_table(cli))
    {
        pthread_exit(NULL);
    }

    if(exchange_keys(struct client cli))
    {
        pthread_exit(NULL);
    }
}

int update_client_table(struct client cli)
{
    int stat;
    char *query=(char *)allocate("char", 100);

    sprintf(query, "insert into clients values ('%s')\n", inet_ntoa(cli.addr.sin_addr));

    if((stat=pthread_mutex_lock(&mutex))!=0)
    {
        fprintf(stderr, "\n[-]Error in locking db for writing for client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(stat));
        return 1;
    }
    if(query_db(query, conn))
    {
        return 1;
    }
    if((stat=pthread_mutex_unlock(&mutex))!=0)
    {
        fprintf(stderr, "\n[-]Error in unlocking db after writing for %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(stat));
        return 1;
    }

    free(query)
    return 0;
}

int query_db(char *query, MYSQL *cxn)
{
    if(mysql_query(query, cxn))
    {
        fprintf(stderr, "\n[-]Error in sending query %s: %s\n", query, mysql_error(cxn));
        return 1;
    }

    res=mysql_use_result(cxn);
    if(res==NULL)
    {
        return 0;
    }

    return 1;
}

int exchange_keys(struct client cli)
{
    char *cmdr=(char *)allocate("char", 2048);
    char *cmds=(char *)allocate("char", 4096);
    char *c_fname=(char *)allocate("char", 50);
    FILE *f;

    if(recv(cli.sock, cmdr, 2048*sizeof(char), 0)<0)
    {
        fprintf(stderr, "\n[-]Error in receving key from client: %s\n", strerror(errno));
        return 1;
    }
    sprintf(c_fname, "key->%s:%d", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port));
    if((f=fopen(c_fname, "w"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening %s file from opening: %s\n", c_fname, strerror(errno));
        return 1;
    }
    for(int i=0; i<strlen(cmdr), i++)
    {
        fprintf(f, "%c", cmdr[i]);
    }
    fclose(f);
    if((cli.ku=gen_keys(c_fname, 1))==NULL)
    {
        return 1;
    }

    if((f=fopen(pub_fname, "r"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening %s file :%s\n", pub_fname, strerror(errno));
        return 1;
    }
    for(int i=0; !feof(f); i++)
    {
        fscanf(f, "%c", &cmds[i]);
    }
    if(send(cli.sock, cmds, 4096*sizeof(char), 0)<0)
    {
        fprintf(stderr, "\n[-]Error in sending the key to client at %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
        return 1;
    }
    fclose(f);

    free(cmdr);
    free(cmds);
    free(c_fname);
    return 0;
}

int main(int argc, char *argv[])
{
    if(init(argc))
    {
        _exit(-1);
    }
    pub_fname=argv[2];

    if((serve_sock=server_init(argv[1]))==0)
    {
        _exit(-1);
    }

    if(dbconnect(argv[4]))
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
