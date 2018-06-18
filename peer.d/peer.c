#include<stdio.h>
#include<unistd.h>
#include"server_init.h"
#include"dbconnect.h"
#include"gen_keys.h"
#include"update_existance_in_peers.h"
#include"end_db_connection.h"

int init(int argc)
{
    if(argc!=5)
    {
        fprintf(stderr, "\n[!]Usage:\n./peer [ip_to_bind:port_to_bind] [pub_key.pem] [priv_key.pem] [db_inetface_ip:port]\n");
        return 1;
    }

    return 0;
}

int server_sock;

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

    if(server_workings())
    {
        _exit(-1);
    }

    if((ku=gen_keys(argv[2], 1))==NULL || (kv=gen_keys(argv[3], 0))==NULL)
    {
        _exit(-1);
    }

    if((db_sock=dbconnect(argv[4]))==0)
    {
        _exit(-1);
    }

    if(update_existance_in_peers(argv[2]))
    {
        _exit(-1);
    }

    if(end_db_connection())
    {
        _exit(-1);
    }
}
