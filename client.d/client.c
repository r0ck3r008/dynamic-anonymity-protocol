#include<stdio.h>
#include<unistd.h>
#include<sodium.h>
#include"server_init.h"
#include"gen_keys.h"
#include"dbconnect.h"
#include"get_rand_sno.h"
#include"get_rand_peer.h"
#include"authenticate_with_const_peer.h"
#include"end_db_connection.h"

int init(int argc)
{
    if(argc!=5)
    {
        fprintf(stderr, "\n[!]Usage:\n./client [ip_to_bind:port_to_bind] [pub_key.pem] [priv_key.pem] [db_inetface_ip:port]\n");
        return 1;
    }

    if(sodium_init())
    {
        fprintf(stderr, "\n[-]Error in initialising sodium library\n");
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

    if((server_sock=server_init(argv[1]))==0)
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

    //for main peer;
    if((const_peer.rand_sno=get_rand_sno())==-1)
    {
        _exit(-1);
    }
    printf("\n[!]rand_sno is: %d\n", const_peer.rand_sno);
    if(get_rand_peer(&const_peer.p, const_peer.rand_sno, "const_peer_ku.pem"))
    {
        _exit(-1);
    }
    printf("\n[!]Got constant peer at: %s\n", inet_ntoa(const_peer.p.addr.sin_addr));

    if(authenticate_with_const_peer(argv[2]))
    {
        _exit(-1);
    }

    if(end_db_connection())
    {
        _exit(-1);
    }
}
