#define NEEDS_JOINEE_STRUCT

#include"update_clients_table.h"
#include"snd_rcv.h"
#include"allocate.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int update_clients_table(int cli_rand_num, struct joinee *cli)
{
    char *query=(char *)allocate("char", 2048);
    char *cmdr;

    sprintf(query, "0:update clients set pid=%d where id=%d;", my_rand_id, cli_rand_num);
    if(snd(db_sock, query, "update client's temp association"))
    {
        return 1;
    }

    if((cmdr=rcv(db_sock, "rcv ack of client table updation"))==NULL)
    {
        return 1;
    }

    if(strcmp(strtok(cmdr, ":"), "SUCCESS")!=0)
    {
        fprintf(stderr, "\n[-]Error in sending query for clients updation %s:%d\n", inet_ntoa(cli->addr.sin_addr), ntohs(cli->addr.sin_port));
        return 1;
    }

    printf("\n[!]Updated client %s:%d with status: %s\n", inet_ntoa(cli->addr.sin_addr), ntohs(cli->addr.sin_port), strtok(NULL, ":"));

    free(cmdr);
    return 0;
}
