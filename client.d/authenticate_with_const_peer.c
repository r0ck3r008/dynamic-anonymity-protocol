#define NEEDS_ALL
#define _GNU_SOURCE

#include"authenticate_with_const_peer.h"
#include"authenticate_with_tmp_peer.h"
#include"global_defs.h"
#include"common_headers/allocate.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sodium.h>
#include<openssl/err.h>
#include<errno.h>

int authenticate_with_const_peer(char *ku_fname)
{
    pkt_num=0;
    char *key=(char *)allocate("char", 2048);
    FILE *f;
    my_rand_num=(int)randombytes_uniform(1000000);

    if((f=fopen(ku_fname, "r"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening file %s: %s\n", ku_fname, strerror(errno));
        return 1;
    }
    for(int i=0; !feof(f); i++)
    {
        fscanf(f, "%c", &key[i]);
    }
    sprintf(key, "%d:%s:%s", pkt_num++, server_ip, key); 

    if(authenticate_with_tmp_peer(key))
    {
        return 1;
    }

    fclose(f);
    free(key);
    return 0;
}
