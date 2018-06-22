#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

    #ifdef NEEDS_ALL
        #define NEEDS_STRUCT_GLOBALS
        #define NEEDS_KEY_GLOBALS
    #endif

#include<openssl/rsa.h>
int db_sock;

    #ifdef NEEDS_STRUCT_GLOBALS
        #include<sys/socket.h>
        #include<netinet/in.h>
        #include<arpa/inet.h>
        struct peer
        {
            int sock, id;
            struct sockaddr_in addr;
            RSA *ku;
            char *ku_fname;
        };
        struct peer_combo
        {
            struct peer p;
            int rand_sno;
        } const_peer;

    #endif

    #ifdef NEEDS_KEY_GLOBALS
        RSA *ku, *kv;
    #endif

#endif
