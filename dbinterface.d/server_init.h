#ifndef SERVER_INIT
#define SERVER_INIT

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

struct client
{
    int sock;
    struct sockaddr_in addr;
};
int server_sock, cli_num;
int server_init(char *);
int server_workings();


#endif
