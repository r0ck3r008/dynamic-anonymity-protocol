#ifndef GLOBAL_DEFS_H   //main
#define GLOBAL_DEFS_H

    #ifdef NEEDS_ALL
        #define NEEDS_DB_GLOBALS
        #define NEEDS_MUTEX_GLOBALS
        #define NEEDS_JOINEE_GLOBALS
    #endif

    #ifdef NEEDS_DB_GLOBALS     //for db
        #include<mysql/mysql.h>
        MYSQL *conn;
        MYSQL_RES *res;
        MYSQL_ROW row;
    #endif      //for db end

int server_psock, server_csock;
int cli_count, peer_count;
int sno, *rand_num_arr;

    #ifdef NEEDS_MUTEX_GLOBALS  //for mutex
        #include<pthread.h>
        extern pthread_mutex_t mutex;
        extern pthread_mutex_t sno_rand_num;
    #endif      //for mutex end

    #ifdef NEEDS_JOINEE_GLOBALS     //for joinee related
        #include<sys/socket.h>
        #include<netinet/in.h>
        #include<arpa/inet.h>
        struct joinee
        {
            int sock, peer;
            struct sockaddr_in addr;
        };
    #endif      //for joinee end

#endif      //main end
