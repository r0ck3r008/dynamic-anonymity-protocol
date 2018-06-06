#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
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
};
int server_sock, cli_num=0;
MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

//prototypes
void *allocate(char *, int);
int init(int);
int server_init(char *);
int dbconnect(char *);
char *get_passwd();
int server_workings();
void *cli_run(void *);

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
        fprintf(stderr, "\n[-]Error in initiating conn data structure\n");
        return 1;
    }

    if(!mysql_real_connect(conn, uname, passwd, dbname, 0, NULL, 0))
    {
        fprintf(stderr, "\n[-]Error in connecting to database: %s\n", strerror(errno));
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

void *cli_run(void *c);
{
    struct client cli=*(struct client *)c;

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
