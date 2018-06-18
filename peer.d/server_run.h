#ifndef SERVER_RUN_H
#define SERVER_RUN_H

#include<openssl/rsa.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
struct client
{
    int sock;
    int fellow;
    struct sockaddr_in addr;
    RSA *ku;
    int id;
};
extern int cli_count;

void *server_run(void *);

#endif
