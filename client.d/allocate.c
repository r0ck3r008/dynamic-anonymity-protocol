#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include "allocate.h"
#include"get_rand_sno.h"

void *allocate(char *type, int size)
{
    void *ret=NULL;

    if(strcmp(type, "char")==0)
    {
        ret=malloc(sizeof(char)*size);
        explicit_bzero(ret, size*sizeof(char));
    }
    else if(strcmp(type, "struct peer_combo")==0)
    {
        ret=malloc(size*sizeof(struct peer_combo));
        explicit_bzero(ret, size*sizeof(struct peer_combo));
    }

    if(ret==NULL)
    {
        fprintf(stderr, "\n[-]Error in allocating %d bytes for %s type\n", size, type);
        _exit(-1);
    }

    return ret;
}
