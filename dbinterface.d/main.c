#include<stdio.h>
#include<unistd.h>
#include<mysql/mysql.h>
#include<sodium.h>
#include"global_defs.h"
#include"dbconnect.h"

int init(int argc)
{
    if(argc!=4)
    {
        fprintf(stderr, "\n[!]Usage:\n./dbinterface [ip_to_bind_for_peers:port] [ip_to_bind_for_clients:port] [db_server:db_uname:db_name]\n");
        return 1;
    }

    if(mysql_library_init(0, NULL, NULL))
    {
        fprintf(stderr, "\n[-]Error in initiating mysql library\n");
        return 1;
    }

    if(sodium_init())
    {
        fprintf(stderr, "\n[-]Error in starting libsodium\n");
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

    if(dbconnect(argv[3]))
    {
        _exit(-1);
    }
}
