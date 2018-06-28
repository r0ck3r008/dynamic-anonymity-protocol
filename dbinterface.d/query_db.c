#define NEEDS_ALL

#include"query_db.h"
#include"allocate.h"
#include<stdio.h>
#include<string.h>
#include<errno.h>

int query_db(struct joinee *j, char *query, char *cmds, int exp_col)
{
    int stat;
    explicit_bzero(cmds, sizeof(char)*2048);
    int flag=0;

    if((stat=pthread_mutex_lock(&mutex))!=0)
    {
        fprintf(stderr, "\n[-]Error in locking mutex for %s:%d: %s\n", inet_ntoa(j->addr.sin_addr), ntohs(j->addr.sin_port), strerror(stat));
        return 1;
    }

    if(mysql_query(conn, query))
    {
        fprintf(stderr, "\n[-]Error in querying %s for %s:%d: %s\n", query, inet_ntoa(j->addr.sin_addr), ntohs(j->addr.sin_port), mysql_error(conn));
        return 1;
    }

    res=mysql_use_result(conn);
    row=mysql_fetch_row(res);
    for(int i=0; i<exp_col; i++)
    {
        strcat(cmds, row[i]);
    }
    
    if(mysql_affected_rows(conn)==1)
    {
        sprintf(cmds, "INSERTED/UPDATED");
    }

    mysql_free_result(res);

    if((stat=pthread_mutex_unlock(&mutex))!=0)
    {
        fprintf(stderr, "\n[-]Error in unlocking mutex for %s query for %s:%d: %s\n", query, inet_ntoa(j->addr.sin_addr), ntohs(j->addr.sin_port), strerror(stat));
        return 1;
    }

    sprintf(cmds, "SUCCESS:%s", cmds);
    return 0;
}
