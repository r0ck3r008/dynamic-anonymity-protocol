#ifndef SERVER_INIT_H
#define SERVER_INIT_H 

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>


//data types
struct client
{
    int sock;
    struct sockaddr_in addr;
    int id;
};
int cli_count, server_sock;

//prototypes
int server_init(char *);
int server_workings();


#endif
