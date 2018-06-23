#define NEEDS_ALL

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"global_defs.h"
#include"server_workings.h"
#include"gen_keys.h"
#include"dbconnect.h"
#include"update_existance.h"

int init(int argc)
{
    if(argc!=6)
    {
        fprintf(stderr, "\n[!]Usage:\n./peer [ip_to_bind_peers:port] [ip_to_bind_clients:port] [pub.pem] [priv.pem] [dbinterface_ip:port]\n");
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if(init(argc))
    {
        _exit(-1);
    }

    if((ku=gen_keys(argv[3], 1))==NULL || (kv=gen_keys(argv[4], 0))==NULL)
    {
        _exit(-1);
    }

    if(dbconnect(argv[5]))
    {
        _exit(-1);
    }

    if(server_workings(argv[1], argv[2]))
    {
        _exit(-1);
    }

    if(update_existance(argv[3], argv[1]))
    {
        _exit(-1);
    }
}
