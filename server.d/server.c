#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include"server_init.h"
#include"cli_run.h"

int init(int argc)
{
    if(argc!=2)
    {
        fprintf(stderr, "\n[!]Usage:\n./server [ip_to_bind:port_to_bind]\n");
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

    if(server_workings())
    {
        _exit(-1);
    }
}
