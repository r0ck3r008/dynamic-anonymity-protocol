#include"bit_is_zero.h"
#include"allocate.h"
#include"dbconnect.h"
#include"db_workings.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<mysql/mysql.h>
#include<errno.h>

char *bit_is_zero(struct client cli, int peer, int exp_col)
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

