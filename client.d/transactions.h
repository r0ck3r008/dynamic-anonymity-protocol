#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

int transactions(char *);
int init_dialog();
void *wait_thr_run(void *);
int find_new_peer();

#endif
