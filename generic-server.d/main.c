#include"server_init.h"
#include"server_workings.h"
#include"global_defs.h"
#include<stdio.h>
#include<unistd.h>

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
    if((server_sock=server_init(argv[1]))==-1)
    {
        _exit(-1);
    }

    if(server_workings(argv[1]))
    {
        return 1;
    }
}
