#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

int db_sock, server_psock, server_csock;
int my_sno, my_rand_id;

    #ifdef NEEDS_ALL
        #define NEEDS_KEYS
    #endif

    #ifdef NEEDS_KEYS
        #include<openssl/rsa.h>
        RSA *ku, *kv;
    #endif

#endif
