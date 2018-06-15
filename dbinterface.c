#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<mysql/mysql.h>
#include<pthread.h>
#include<sodium.h>
#include<termios.h>
#include<errno.h>

struct client
{
    int sock;
    struct sockaddr_in addr;
};
int server_sock, cli_num=0;
MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
int sno=0, rand_array[100];

//prototypes
void *allocate(char *, int);
int init(int);
int server_init(char *);
int dbconnect(char *);
char *get_passwd();
int server_workings();
void *cli_run(void *);
char *db_workings(struct client, char *);
char *bit_is_one(struct client, int);
char *bit_is_zero(struct client, int);

void *allocate(char *type, int size)
{
    void *ret=NULL;

    if(strcmp(type, "char")==0)
    {
        ret=malloc(size*sizeof(char));
        explicit_bzero(ret, size*sizeof(char));
    }
    else if(strcmp(type, "int")==0)
    {
        ret=malloc(size*sizeof(int));
        explicit_bzero(ret, size*sizeof(int));       
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
        fprintf(stderr, "\n[!]Usage:\n./dbinterface [ip_to_bind:port_to_bind] [server:uname:dbname]\n");
        return 1;
    }

    if(mysql_library_init(0, NULL, NULL))
    {
        fprintf(stderr, "\n[-]Error in initiating mysql\n");
        return 1;
    }

    if(sodium_init())
    {
        fprintf(stderr, "\n[-]Error in initiating sodium lib\n");
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

int dbconnect(char *argv2)
{
    char *server=strtok(argv2, ":"), *uname=strtok(NULL, ":"), *dbname=strtok(NULL, ":");
    char *passwd;
    printf("\n[>]Enter passwd: ");
    if((passwd=get_passwd())==NULL)
    {
        return 1;
    }

    if((conn=mysql_init(NULL))==NULL)
    {
        fprintf(stderr, "\n[-]Error in initiating conn data structure\n");
        return 1;
    }

    if(!mysql_real_connect(conn, server, uname, passwd, dbname, 0, NULL, 0))
    {
        fprintf(stderr, "\n[-]Error in connecting to database: %s\n", mysql_error(conn));
        return 1;
    }

    free(passwd);
    return 0;

}

char *get_passwd()
{
    char *passwd=(char *)allocate("char", 50);
    struct termios a, b;
    explicit_bzero(&a, sizeof(a));
    explicit_bzero(&b, sizeof(b));

    if(tcgetattr(fileno(stdin), &a)<0)
    {
        fprintf(stderr, "\n[-]Error in getting attributes: %s\n", strerror(errno));
        return NULL;
    }

    b=a;
    a.c_lflag &= ~ECHO;
    a.c_lflag |= ECHONL;

    if(tcsetattr(fileno(stdin), TCSANOW, &a)<0)
    {
        fprintf(stderr, "\n[-]Error in setting no echo flag :%s\n", strerror(errno));
        return NULL;
    }

    fgets(passwd, sizeof(char)*50, stdin);

    if(tcsetattr(fileno(stdin), TCSANOW, &b)<0)
    {
        fprintf(stderr, "\n[-]Error in makin stdin normal: %s\n", strerror(errno));
        return NULL;
    }

    return passwd;
}

int server_workings()
{
    pthread_t tid[10];
    struct client cli[10];
    int stat;
    socklen_t len=sizeof(struct sockaddr_in);
    char *retval;
    printf("\n[!]Starting dbinterface....\n[!]Waiting for connections...\n");

    for(; cli_num<10; cli_num++)
    {
        if((cli[cli_num].sock=accept(server_sock, (struct sockaddr *)&cli[cli_num].addr, &len))<0)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d: %s\n", cli_num, strerror(errno));
            continue;
        }

        if((stat=pthread_create(&tid[cli_num], NULL, cli_run, &cli[cli_num]))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating new client thread %d: %s\n", cli_num, strerror(stat));
            continue;
        }
    }

    for(int i=0; i<cli_num; i++)
    {
        if((stat=pthread_join(tid[i], (void **)&retval))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to client %d: %s\n", i, strerror(stat));
            continue;
        }
        printf("\n[!]Client %d exited with code: %s\n", (char *)retval);
    }

    return 0;
}

void *cli_run(void *c)
{
    struct client cli=*(struct client *)c;
    char *cmdr=(char *)allocate("char", 2048);
    char *cmds;
    int bit;

    printf("\n[!]Accepted from %s:%d\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port));
    while(1)
    {
        if(recv(cli.sock, cmdr, sizeof(char)*2048, 0)<0)
        {
            fprintf(stderr, "\n[-]Error in receving from client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
            pthread_exit("ERROR IN RECEVING");
        }
        if(strcmp(cmdr, "DONE")==0)
        {
            printf("\n[!]Exiting client %s:%d\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port));
            if(send(cli.sock, "OK", sizeof(char)*strlen("OK"), 0)<0)
            {
                fprintf(stderr, "\n[-]Error in sending back ack to client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
                pthread_exit("ERROR IN SENDING ACK");
            }
            break;
        }

        if((cmds=db_workings(cli, cmdr))==NULL)
        {
            pthread_exit("ERROR IN DB_WORKINGS");
        }

        if(send(cli.sock, cmds, sizeof(char)*2048, 0)<0)
        {
            fprintf(stderr, "\n[-]Error in sending back to %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(errno));
            pthread_exit("ERROR IN SENDING");
        }
    }

    free(cmdr);free(cmds);
    pthread_exit("SUCCESS");
}

char *db_workings(struct client cli, char *cmdr)
{
    int bit= (int)strtol(strtok(cmdr, ":"), NULL, 0), exp_col=(int)strtol(strtok(NULL, ":"), NULL, 10);
    char *cmds;

    if(bit)
    {
        if((cmds=bit_is_one(cli, exp_col))==NULL)
        {
            return NULL;
        }
    }
    else
    {
        if((cmds=bit_is_zero(cli, exp_col))==NULL)
        {
            return NULL;
        }
    }

    return cmds;
}

char *bit_is_one(struct client cli, int exp_col)
{
    char *cmds=(char *)allocate("char", 2048);
    char *query=(char *)allocate("char", 2048);
    int rand, counter, stat;

    for(int flag=0; flag!=1; )
    {
        rand=(int)randombytes_uniform(10000);
        for(counter=0; counter<100; counter++)
        {
            if(rand==rand_array[counter])
            {
                break;
            }
            else if(rand_array[counter]==-1)
            {
                flag=1;
                break;
            }
        }
    }
    if((stat=pthread_mutex_lock(&mutex))!=0)
    {
        fprintf(stderr, "\n[-]Error in locking wrt for client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(stat));
        return NULL;
    }

    rand_array[counter]=rand;
    sprintf(query, "insert into peers values (%d, %d, '%s', '%s');", sno++, rand, inet_ntoa(cli.addr.sin_addr), strtok(NULL, ":"));
    if(mysql_query(conn, query))
    {
        fprintf(stderr, "\n[-]Error in querying for %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), mysql_error(conn));
        return NULL;
    }
    if(mysql_affected_rows(conn)!=1)
    {
        fprintf(stderr, "\n[-]Error in updating db for client %s:%d\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port));
        return NULL;
    }

    if((stat=pthread_mutex_unlock(&mutex))!=0)
    {
        fprintf(stderr, "\n[-]Error in unlocking wrt for client %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(stat));
        return NULL;
    }
    sprintf(cmds, "SUCCESS");

    free(query);
    return cmds;
}

char *bit_is_zero(struct client cli, int exp_col)
{
    char *cmds=(char *)allocate("char", 2048);
    char *query=(char *)allocate("char", 2048);
    int stat;

    if((stat=pthread_mutex_lock(&mutex))!=0)
    {
        fprintf(stderr, "\n[-]Error in locking mutex for %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(stat));
        return NULL;
    }
        
    sprintf(query, "%s", strtok(NULL, ":"));
    if(mysql_query(conn, query))
    {
        fprintf(stderr, "\n[-]Error in querying %s for: %s:%d: %s\n", query, inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), mysql_error(conn));
        return NULL;
    }
    res=mysql_use_result(conn);
    row=mysql_fetch_row(res);
    for(int i=0; i<exp_col; i++)
    {
        strcat(cmds, row[i]);
        if(i==exp_col-1)
        {
            strcat(cmds, "\n");
        }
        else
        {
            strcat(cmds, ":");
        }
    }
    mysql_free_result(res);
    if((stat=pthread_mutex_unlock(&mutex))!=0)
    {
        fprintf(stderr, "\n[-]Error in unlocking mutex for %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), strerror(stat));
        return NULL;
    }

    free(query);
    return cmds;
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

    for(int i=0; i<100; i++)
    {
        rand_array[i]=-1;
    }

    if(server_workings())
    {
        _exit(-1);
    }
}
