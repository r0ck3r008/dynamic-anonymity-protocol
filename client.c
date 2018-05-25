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

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
char *uname="root", *dbname="anon", *passwd;

//prototypes
int init();
void *allocate(char *, int);
char *get_pass();
int dbconnect(char *);
char *get_peer_addr(int *);
int query_db(char *);

int init()
{
    if(argc!=2)
    {
        fprintf(stderr, "\n[!]Usage:./client [db-server-ip]\n");
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

int main(int argc, char *argv[])
{
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

    free(passwd); 
    mysql_close(conn);
    free(peer_addr);
}
