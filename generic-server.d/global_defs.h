#ifndef GLOBAL_DEFS_H

int server_sock, cli_num;

    #ifdef NEEDS_CLIENT_STRUCT
        #include<sys/socket.h>
        #include<netinet/in.h>
        #include<arpa/inet.h>
        struct client
        {
            int sock, id;
            struct sockaddr_in addr;
        };
        struct sockaddr_in my_addr;
    #endif

#endif
