#ifndef PEER_RUN_H
#define PEER_RUN_H

#include"global_defs.h"
void *peer_run(void *);
char *decrypt(char *);
char *encrypt(RSA *, char *);
int connect_to_final_server(char *, int);
int write_and_gen_ku(int, char *);
int inc_max_clients(char *);
int find_in_clients_arr(int);

#endif
