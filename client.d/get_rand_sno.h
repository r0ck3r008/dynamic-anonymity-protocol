#ifndef GET_RAND_SNO_H
#define GET_RAND_SNO_H

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<openssl/rsa.h>
struct peer
{
    int sock, id;
    struct sockaddr_in addr;
    RSA *ku;
    char *ku_fname;
} dest_peer;
struct peer_combo
{
    struct peer p;
    int rand_sno;
} const_peer;

int get_rand_sno();

#endif
