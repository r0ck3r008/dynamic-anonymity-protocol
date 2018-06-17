#ifndef DB_WORKINGS_H
#define DB_WORKINGS_H

#include"server_init.h"
#include<pthread.h>
extern pthread_mutex_t mutex;
char *db_workings(struct client, char *);

#endif
