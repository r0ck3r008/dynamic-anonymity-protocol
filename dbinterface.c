#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<mysql/mysql.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<termios.h>
#include<sodium.h>
#include<pthread.h>
#include<errno.h>

struct client
{
    int sock;
    struct sockaddr_in addr;
};
int server_sock, rc=0, peer_count=0, rand_array[10], all_count=0;
MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
pthread_mutex_t wrt=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex =PTHREAD_MUTEX_INITIALIZER;

//prototypes
void *allocate(char *, int);
int init(int);
int server_init(char *);
int dbconnect(char *);
char *get_passwd();
int server_workings();
void *server_run(void *);
void *cli_run(void *);
char *db_workings(struct client, char *);

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
        fprintf(stderr, "\n[!]Usage:\n./dbinterface [ip_to_bind:port_to_bind] [db_server:db_uname:db_name]\n");
        return 1;
    }

    if(mysql_library_init(0, NULL, 0))
    {
        fprintf(stderr, "\n[-]Error in initialising mysql library\n");
        return 1;
    }

    if(sodium_init())
    {
        fprintf(stderr, "\n[-]Error in starting libsodium\n");
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

int dbconnect(char *argv2)
{
    char *server=strtok(argv2, ":"), *uname=strtok(NULL, ":"), *dbname=strtok(NULL, ":");
    char *passwd;
    if((passwd=get_passwd())==NULL)
    {
        return 1;
    }

    if((conn=mysql_init(NULL))==NULL)
    {
        fprintf(stderr, "\n[-]Error in initiating connection datatype\n");
        return 1;
    }

    if(!mysql_real_connect(conn, server, uname, passwd, dbname, 0, NULL, 0))
    {
        fprintf(stderr, "\n[-]Error in connecting to database: %s\n", mysql_error(conn));
        return 1;
    }

    printf("\n[!]Connected to database: %s\n", dbname);
    free(passwd);
    return 0;
}

char *get_passwd()
{
    char *passwd=(char *)allocate("char", 50);

    struct termios a,b;
    explicit_bzero(&a, sizeof(a));
    explicit_bzero(&b, sizeof(b));

    if(tcgetattr(fileno(stdin), &a)<0)
    {
        fprintf(stderr, "\n[-]Error in getting stdin attributes: %s\n", strerror(errno));
        return NULL;
    }

    b=a;
    a.c_lflag &= ~ECHO;
    a.c_lflag |= ECHONL;

    if(tcsetattr(fileno(stdin), TCSANOW, &a)<0)
    {
        fprintf(stderr, "\n[-]Error in setting no echo flag for stdin: %s\n", strerror(errno));
        return NULL;
    }

    fgets(passwd, 50*sizeof(char), stdin);

    if(tcsetattr(fileno(stdin), TCSANOW, &b)<0)
    {
        fprintf(stderr, "\n[-]Error in making stdin normal: %s\n", strerror(errno));
        return NULL;
    }

    return passwd;
}

int server_workings()
{
    int stat;
    pthread_t server_thr;

    if((stat=pthread_create(&server_thr, NULL, server_run, "..."))!=0)
    {
        fprintf(stderr, "\n[-]Error in initialising server thread: %s\n", strerror(stat));
        return 1;
    }

    if((stat=pthread_join(server_thr, NULL))!=0)
    {
        fprintf(stderr, "\n[-]Error in joining to server thread: %s\n", strerror(stat));
        return 1;
    }

    return 0;
}

void *server_run(void *a)
{
    pthread_t tid[10];
    socklen_t len=sizeof(struct sockaddr_in);
    struct client cli[10];
    int stat;
    char *retval;

    for(all_count; all_count<10; all_count++)
    {
        if((cli[all_count].sock=accept(server_sock, (struct sockaddr *)&cli[all_count].addr, &len))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d: %s\n", all_count, strerror(errno));
            continue;
        }

        if((stat=pthread_create(&tid[all_count], NULL, cli_run, &cli[all_count]))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating thread for client %d at %s:%d: %s\n", all_count, inet_ntoa(cli[all_count].addr.sin_addr), ntohs(cli[all_count].addr.sin_port), strerror(stat));
            continue;
        }
    }

    for(int i=0; i<all_count; i++)
    {
        if((stat=pthread_join(tid[i], (void **)&retval))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to client %s:%d: %s\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), strerror(stat));
            continue;
        }
        if(strcmp((char *)retval, "ERROR")==0)
        {
            fprintf(stderr, "\n[-]Error in execytion of client %s:%d\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port));
            continue;
        }
    }

    pthread_exit(NULL);
}

void *cli_run(void *c)
{
    struct client cli=*(struct client *)c;
    char *cmdr=(char *)allocate("char", 2048);
    char *cmds=(char *)allocate("char", 512);

    if(recv(cli.sock, cmdr, sizeof(char)*2048, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in receving from client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
        pthread_exit("ERROR");
    }

    if((cmds=db_workings(cli, cmdr))==NULL)
    {
        pthread_exit("ERROR");
    }

    if(send(cli.sock, cmds, sizeof(char)*512, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in sending ack to client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
        pthread_exit("ERROR");
    }
    free(cmdr); free(cmds);
    close(cli.sock);
    pthread_exit("SUCCESS");
}

char *db_workings(struct client cli, char *cmdr)
{
    int stat, bit=(int)strtol(strtok(cmdr, ":"), NULL, 10);
    char *query=(char *)allocate("char", 128);
    if(bit)     //writer
    {
        if((stat=pthread_mutex_lock(&wrt))!=0)
        {
            fprintf(stderr, "\n[-]Error in locking wrt for %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(stat));
            return NULL;
        }

        sprintf(query, "insert into peers values ('%d', '%d', '%s', '%s')", peer_count++, (int)randombytes_uniform(10000), inet_ntoa(cli.addr.sin_addr), strtok(NULL, ":"));
        if(mysql_query(conn, query))
        {
            fprintf(stderr, "\n[-]Error in querying %s for client %s:%d: %s\n", query, inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), mysql_error(conn));
            return NULL;
        }
        res=mysql_use_result(conn);
        if(mysql_affected_rows(conn)==1)
        {
            return "SUCCESS";
        }
        else
        {
            return NULL;
        }

        if((stat=pthread_mutex_unlock(&wrt))!=0)
        {
            fprintf(stderr, "\n[-]Error in unlocking wrt for %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
            return NULL;
        }
    }
    else
    {

    }
    free(query);
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

    if(dbconnect(argv[2]))
    {
        _exit(-1);
    }

    if(server_workings())
    {
        _exit(-1);
    }
}
