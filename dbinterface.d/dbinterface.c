#include<stdio.h>
#include<unistd.h>
#include<sodium.h>
#include"allocate.h"
#include"get_passwd.h"
#include"dbconnect.h"
#include"server_init.h"
#include"cli_run.h"
#include"db_workings.h"
#include"bit_is_one.h"
#include"bit_is_zero.h"

int init(int argc)
{
    if(argc!=3)
    {
        fprintf(stderr, "\n[!]Usage:\n./dbinterface [ip_to_bind:port_to_bind] [server:uname:dbname]\n");
        return 1;
    }

    if(mysql_library_init(0, NULL, NULL))
    {
        fprintf(stderr, "\n[-]Error in initiating mysql\n");
        return 1;
    }
    if(sodium_init())
    {
        fprintf(stderr, "\n[-]Error in initiating sodium lib\n");
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

    if(dbconnect(argv[2]))
    {
        _exit(-1);
    }

    sno=0;
    for(int i=0; i<100; i++)
    {
        rand_array[i]=-1;
    }

    if(server_workings())
    {
        _exit(-1);
    }
}
