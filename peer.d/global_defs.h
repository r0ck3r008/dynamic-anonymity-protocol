#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

int db_sock, server_psock, server_csock;
int my_sno, my_rand_id;
int peer_count, cli_count;
int max_clients;

    #ifdef NEEDS_ALL
        #define NEEDS_KEYS
        #define NEEDS_JOINEE_STRUCT
        #define NEEDS_MUTEX
    #endif

    #ifdef NEEDS_KEYS
        #include<openssl/rsa.h>
        RSA *ku, *kv;
    #endif

    #ifdef NEEDS_JOINEE_STRUCT
        #include<sys/socket.h>
        #include<netinet/in.h>
        #include<arpa/inet.h>
        #include<openssl/rsa.h>
        struct client
        {
            int final_server_sock;
            int cli_rand_num;
            struct sockaddr_in final_server_addr;
            char *ku_fname;
            RSA *ku;
        } *clients;
        struct joinee
        {
            int sock, id;
            struct sockaddr_in addr;
            struct client *my_curr_client;
        };
    #endif

    #ifdef NEEDS_MUTEX
        #include<pthread.h>
        extern pthread_mutex_t db_sock_mutex;
        extern pthread_mutex_t max_clients_mutex;
    #endif
#endif
