#include"write_key_to_file.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

RSA *write_key_to_file(char *fname, char *key)
{
    FILE *f;
    if((f=fopen(fname, "w"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening %s new file %s\n", fname, strerror(errno));
        return NULL;
    }

    for(int i=0; i<strlen(key); i++)
    {
        fprintf(f, "%c", key[i]);
    }
    fclose(f);

    RSA *ku;
    if((ku=gen_keys(fname, 1))==NULL)
    {
        return NULL;
    }

    return ku;
}
