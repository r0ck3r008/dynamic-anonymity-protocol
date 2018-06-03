#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<mysql/mysql.h>
#include<termios.h>
#include<pthread.h>
#include<errno.h>

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
int serve_sock, cli_count;
struct client
{
    int sock;
    struct sockaddr_in addr;
};
pthread_mutex_t wrt= PTHREAD_MUTEX_INITIALIZER;
int rc=0;
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;

//prototypes
void *allocate(char *, int);
int init(int);
int dbconnect(char *);
char *get_passwd();
int server_init(char *);
int server_workings();
void *cli_run(void *);
char *db_work(struct client, char *, int);

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
    if(argc<2)
    {
        fprintf(stderr, "\n[!] Usage:\n./dbinterface [ip_to_bind:port_to_bind] [db_server:db_uname:db_name]\n");
        return 1;
    }

    if(mysql_init(NULL))
    {
        fprintf(stderr, "\n[-]Error in initialising mysql library\n");
        return 1;
    }

    return 0;
}

int dbconnect(char *cred)
{
    char *server=strtok(cred, ":");
    char *uname=strtok(NULL, ":");
    char *dbname=strtok(NULL, ":");
    printf("\n[>]Enter db passwd: ");
    char *passwd;
    if((passwd=get_passwd())==NULL)
    {
        return 1;
    }

    if(mysql_init(conn)==NULL)
    {
        fprintf(stderr, "\n[-]Error in creating connection object\n");
        return 1;
    }
    if(!mysql_real_connect(conn, server, uname, passwd, dbname, 0, NULL, 0))
    {
        fprintf(stderr, "\n[-]Error in connecting to the database: %s\n", mysql_error(conn));
        return 1;
    }

    printf("\n[!]Connected to the database\n");

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
        fprintf(stderr, "\n[-]Error in  setting no echo flag in stdin: %s\n", strerror(errno));
        return NULL;
    }

    fgets(passwd, sizeof(char)*50, stdin);

    if(tcsetattr(fileno(stdin), TCSANOW, &b)<0)
    {
        fprintf(stderr, "\n[-]Error in making stdin normal: %s\n", strerror(errno));
        return NULL;
    }

    return passwd;
}

int server_init(char *argv1)
{
    char *ip=strtok(argv1, ":");
    int port=(int)strtol(strtok(NULL, ":"), NULL, 10);
    int s;
    struct sockaddr_in addr;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr(ip);
    addr.sin_family=AF_INET;

    if((s=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "\n[-]Error in creating server_sock: %s\n", strerror(errno));
        return 0;
    }

    if(bind(s, (struct sockaddr *)&addr, sizeof(addr))<0)
    {
        fprintf(stderr, "\n[-]Error in binding to %s:%d: %s\n", ip, port, strerror(errno));
        return 0;
    }

    if(listen(s, 5)<0)
    {
        fprintf(stderr, "\n[-]Error in listning to serve_sock: %s\n", strerror(errno));
        return 0;
    }

    printf("\n[!]serve_socket bound and listning successfully\n");
    return s;
}

int server_workings()
{
    pthread_t tid[10];
    socklen_t len=sizeof(struct sockaddr_in);
    struct client cli[10];
    int stat;
    
    for(cli_count=0; cli_count<10; cli_count++)
    {
        int s;
        if((s=accept(serve_sock, (struct sockaddr *)&cli[cli_count].addr, &len))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d: %s\n", cli_count, strerror(errno));
            continue;
        }

        cli[cli_count].sock=s;
        if((stat=pthread_create(&tid[cli_count], NULL, cli_run, &cli[cli_count]))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating thread for client %s:%d: %s\n", inet_ntoa(cli[cli_count].addr.sin_addr), ntohs(cli[cli_count].addr.sin_port), strerror(stat));
            continue;
        }
    }

    for(int i=0; i<cli_count; i++)
    {
        if((stat=pthread_join(tid[i], NULL))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to client at %s:%d: %s\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), strerror(stat));
            continue;
        }
    }

    return 0;
}

void *cli_run(void *c)
{
    char *cmdr=(char *)allocate("char", 512);
    char *query=(char *)allocate("char", 100), *cmds;
    int q_type;
    struct client cli= *(struct client *)c;

    if(recv(cli.sock, cmdr, sizeof(char)*512, 0)<0)
    {
        fprintf(stderr, "\n[-]Error in receving query from client: %s\n", strerror(errno));
        pthread_exit(NULL);
    }

    q_type=(int)strtol(strtok(cmdr, ":"), NULL, 10);
    sprintf(query, "%s", strtok(NULL, ":"));

    cmds=db_work(cli, query, q_type);
    
    if(strcmp(cmds, "Error")==0)
    {
        pthread_exit(NULL);
    }
    else if(cmds==NULL && q_type==0)
    {
        pthread_exit(NULL);
    }
    else 
    {
        if(send(cli.sock, cmds, 512*sizeof(char), 0)<0)
        {
            fprintf(stderr, "\n[-]Error in sending query to client at %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
            pthread_exit(NULL);
        }
        pthread_exit(NULL);
    }
    free(cmdr);
    free(query);
    if(q_type!=0)
    {
        free(cmds);
    }
}

char *db_work(struct client cli, char *query, int q_type)
{
    int stat;
    char *cmds=(char *)allocate("char", 512);
    cmds=NULL;

    if(q_type==0)   //writer
    {
        if((stat=pthread_mutex_lock(&wrt))!=0)
        {
            fprintf(stderr, "\n[-]Error in locking wrt for client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(stat));
            return "Error";
        }

        if(mysql_query(conn, query))
        {
            fprintf(stderr, "\n[-]Error in sending %s to db for client %s:%d: %s\n", query, inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), mysql_error(conn));
            return "Error";
        }
        res=mysql_use_result(conn);
        if(res==NULL)
        {
            return NULL;
        }

        if((stat=pthread_mutex_unlock(&wrt))!=0)
        {
            fprintf(stderr, "\n[-]Error in unlocking wrt for client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
            return "Error";
        }
    }
    else        //reader
    {
        if((stat=pthread_mutex_lock(&mutex))!=0)
        {
            fprintf(stderr, "\n[-]Error in locing read count for client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(stat));
            return "Error";
        }
        rc++;
        if(rc==1)
        {
            if((stat=pthread_mutex_lock(&wrt))!=0)
            {
                fprintf(stderr, "\n[-]Error in locking wrt for client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(stat));
                return "Error";
            }
        }
        if((stat=pthread_mutex_unlock(&mutex))!=0)
        {
            fprintf(stderr, "\n[-]Error in unlocking read count for client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(stat));
            return "Error";
        }

        //read operation
        if(mysql_query(conn, query))
        {
            fprintf(stderr, "\n[-]Error in quering %s for client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), mysql_error(conn));
            return "Error";
        }
        
        res=mysql_use_result(conn);
        while((row=mysql_fetch_row(res))!=NULL)
        {
            sprintf(cmds, "%s%s:%s\n", cmds, row[0], row[1]);
        }
        if((stat=pthread_mutex_lock(&mutex))!=0)
        {
            fprintf(stderr, "\n[-]Error in locing read count for client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(stat));
            return "Error";
        }
        rc--;
        if(rc==0)
        {
            if((stat=pthread_mutex_unlock(&wrt))!=0)
            {
                fprintf(stderr, "\n[-]Error in unlocking wrt for client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(stat));
                return "Error";
            }
        }
        if((stat=pthread_mutex_unlock(&mutex))!=0)
        {
            fprintf(stderr, "\n[-]Error in unlocking read count for client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(stat));
            return "Error";
        }

        return cmds;
    }
}

int main(int argc, char *argv[])
{
    if(init(argc))
    {
        _exit(-1);
    }

    if(dbconnect(argv[2]))
    {
        _exit(-1);
    }

    if((serve_sock=server_init(argv[1]))==0)
    {
        _exit(-1);
    }

    if(server_workings())
    {
        _exit(-1);
    }
}
