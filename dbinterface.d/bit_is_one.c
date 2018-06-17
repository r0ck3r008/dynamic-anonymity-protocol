#include"bit_is_one.h"
#include"dbconnect.h"
#include"db_workings.h"
#include"allocate.h"
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sodium.h>
#include<errno.h>

#include<mysql/mysql.h>

char *bit_is_one(struct client cli, int peer, int exp_col)
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

    if(peer)
    {
        sprintf(query, "insert into peers values (%d, %d, '%s', '%s');", sno++, rand, inet_ntoa(cli.addr.sin_addr), strtok(NULL, ":"));
    }
    else
    {
        sprintf(query, "insert into clients values ('%d', '%d');", (int)strtol(strtok(NULL, ":"), NULL, 10), rand);
    }
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
    sprintf(cmds, "SUCCESS:%d", rand);

    free(query);
    return cmds;
}

