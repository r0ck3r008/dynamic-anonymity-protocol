#define NEEDS_JOINEE_STRUCT

#include"allocate.h"
#include"global_defs.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

void *allocate(char *type, int size)
{
    void *ret=NULL;
    if(strcmp(type, "char")==0)
    {
        ret=malloc(size*sizeof(char));
        explicit_bzero(ret, size*sizeof(char));
    }
    else if(strcmp(type, "struct client")==0)
    {
        ret=malloc(size*sizeof(struct client));
        explicit_bzero(ret, size*sizeof(struct client));
    }

    if(ret==NULL)
    {
        fprintf(stderr, "\n[-]Error in allocating %d bytes for %s type\n", size, type);
        _exit(-1);
    }

    return ret;
}
