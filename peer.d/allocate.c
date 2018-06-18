#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include "allocate.h"

void *allocate(char *type, int size)
{
    void *ret=NULL;

    if(strcmp(type, "char")==0)
    {
        ret=malloc(sizeof(char)*size);
        explicit_bzero(ret, size*sizeof(char));
    }

    if(ret=NULL)
    {
        fprintf(stderr, "\n[-]Error in allocating %d bytes for %s type\n", size, type);
        _exit(-1);
    }

    return ret;
}
