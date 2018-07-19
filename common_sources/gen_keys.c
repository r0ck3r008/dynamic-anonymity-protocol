#include"common_headers/gen_keys.h"
#include<stdio.h>
#include<string.h>
#include<openssl/pem.h>
#include<openssl/err.h>
#include<errno.h>

RSA *gen_keys(char *fname, int pub)
{
    FILE *f;
    RSA *rsa= RSA_new();

    if((f=fopen(fname, "rb"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening file %s: %s\n", fname, strerror(errno));
        return NULL;
    }

    if(pub)
    {
        rsa=PEM_read_RSA_PUBKEY(f, &rsa, NULL, NULL);
    }
    else
    {
        rsa=PEM_read_RSAPrivateKey(f, &rsa, NULL, NULL);
    }

    fclose(f);
    return rsa;
}
