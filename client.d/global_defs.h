#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

    #ifdef NEEDS_ALL
        #define NEEDS_STRUCT_GLOBALS
        #define NEEDS_KEY_GLOBALS
    #endif

int db_sock, my_rand_num, pkt_num;
char *server_ip;

    #ifdef NEEDS_STRUCT_GLOBALS
        #include<openssl/rsa.h>
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
        } const_peer, tmp_peer;

    #endif

    #ifdef NEEDS_KEY_GLOBALS
        #include<openssl/rsa.h>
        RSA *ku, *kv;
    #endif

#endif
