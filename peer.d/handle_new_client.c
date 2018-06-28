#define NEEDS_JOINEE_STRUCT

#include"handle_new_client.h"
#include"connect_by_addr.h"
#include"update_clients_table.h"

int handle_new_client(struct joinee *j, struct joinee *cli, char *ip, int rand_cli_num)
{
    j->addr.sin_addr.s_addr=inet_addr(ip);
    j->addr.sin_port=htons(12345);
    j->addr.sin_family=AF_INET;

    if((j->sock=connect_by_addr(j->addr))==-1)
    {
        return 1;
    }

    if(update_clients_table(rand_cli_num, cli))
    {
        return 1;
    }
}
