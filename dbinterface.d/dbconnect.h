#ifndef DBCONNECT_H
#define DBCONNECT

#include<mysql/mysql.h>

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
int dbconnect(char *);

#endif
