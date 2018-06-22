#define NEEDS_DB_GLOBALS

#include"dbconnect.h"
#include"global_defs.h"
#include"get_passwd.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int dbconnect(char *argv)
{
    char *server=strtok(argv, ":"), *uname=strtok(NULL, ":"), *passwd, *db_name=strtok(NULL, ":");

    printf("\n[>]Enter db passwd: ");
    passwd=get_passwd();

    if((conn=mysql_init(NULL))==NULL)
    {
        fprintf(stderr, "\n[-]Error in creating connection data type\n");
        return 1;
    }

    if(!mysql_real_connect(conn, server, uname, passwd, db_name, 0, NULL, 0))
    {
        fprintf(stderr, "\n[-]Error in connection %s\n", mysql_error(conn));
        return 1;
    }

    free(passwd);
    return 0;
}
