#include"dbconnect.h"
#include"get_passwd.h"
#include"db_workings.h"
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

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
