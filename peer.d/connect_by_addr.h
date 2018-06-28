#ifndef CONNECT_BY_ADDR_H
#define CONNECT_BY_ADDR_H

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
int connect_by_addr(struct sockaddr_in);

#endif
