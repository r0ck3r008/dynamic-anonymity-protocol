#define NEEDS_ALL

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sodium.h>
#include"dbconnect.h"
#include"gen_keys.h"
#include"get_connect_to_new_peer.h"
#include"transactions.h"
#include"end_db_connection.h"
#include"global_defs.h"

int init(int argc)
{
    if(argc!=4)
    {
        fprintf(stderr, "\n[!]Usage:\n./client [db_ip:db_port] [pub_key.pem] [priv_key.pem]\n");
        return 1;
    }

    if(sodium_init())
    {
        fprintf(stderr, "\n[-]Error in initialising libsodium\n");
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

    if((db_sock=dbconnect(argv[1]))==-1)
    {
        _exit(-1);
    }

    RSA *ku=NULL, *kv=NULL;
    if((ku=gen_keys(argv[2], 1))==NULL || (kv=gen_keys(argv[3], 0))==NULL)
    {
        _exit(-1);
    }
    
    //get const_peer
    int stat=0;
    const_peer=get_connect_to_new_peer(&stat, "const_peer.pem");
    if(stat)
    {
        _exit(-1);
    }

    if(transactions(argv[2]))
    {
        _exit(-1);
    }

    if(end_db_connection())
    {
        _exit(-1);
    }

    free(server_ip);
}
