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

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
char *user="root", *dbname="anon", *pass;

//prototypes
void *allocate(char *, int);
char *get_pass();
int dbconnect();

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

    struct termios a,b;
    if(tcgetattr(fileno(stdin), &a)==-1)
    {
        fprintf(stderr, "\n[-]Error in getting stdin attributes for password: %s\n", strerror(errno));
        _exit(-1);
    }

    b=a;
    a.c_lflag &= ~ECHO;
    a.c_lflag |= ECHONL;

    if(tcsetattr(fileno(stdin), TCSANOW, &a)==-1);
    {
        fprintf(stderr, "\n[-]Error in setting no-echo flag in stdin: %s\n", strerror(errno));
        _exit(-1);
    }

    fgets(pass, sizeof(char)*50, stdin);

    if(tcsetattr(fileno(stdin), TCSANOW, &b)==-1)
    {
        fprintf(stderr, "\n[-]Error in setting stdin backto normal: %s\n", strerror(errno));
        _exit(-1);
    }

    return pass;
}

int dbconnect()
{
    if(mysql_library_init(0, NULL, NULL))
    {
        fprintf(stderr, "\n[-]Error in initiating mysql library\n");
        return 1;
    }

    if((conn=mysql_init(NULL))==NULL)
    {
        fprintf(stderr, "\n[-]Error in initiating connection\n");
        return 1;
    }

    if(!mysql_real_connect(conn, argv[1], uname, passwd, dbname, 0, NULL, 0))
    {
        fprintf(stderr, "\n[-]Error in conneting to db: %s\n", mysql_error(conn));
        return 1;
    }
    printf("\n[!]Connected to database\n");
    
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        fprintf(stderr, "\n[!]Usage:./client [db-server-location]\n");
        _exit(-1);
    }

    printf("\n[>]Enter db password: ");
    passwd=get_pass();

    if(dbconnect())
    {
        _exit(-1);
    }

    free(passwd);
}
