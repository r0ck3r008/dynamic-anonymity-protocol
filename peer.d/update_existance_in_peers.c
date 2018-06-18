#include"update_existance_in_peers.h"
#include"allocate.h"
#include"snd_rcv.h"
#include"dbconnect.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

int my_rand;

int update_existance_in_peers(char *ku_fname)
{
    char *cmds=(char *)allocate("char", 2048);
    char *cmdr;
    FILE *f;
    if((f=fopen(ku_fname, "r"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening %s: %s\n", ku_fname, strerror(errno));
        return 1;
    }
    strcat(cmds, "1:1:0:");
    for(int i=6; !feof(f); i++)
    {
        fscanf(f, "%c", &cmds[i]);
    }
    //1 for insert, 1 for inserting in peers, 0 for expected return columns

    if(snd(db_sock, cmds, "send for updation in peers"))
    {
        fprintf(stderr, "\n[-]Error in sending to the database interface: %s\n", strerror(errno));
        return 1;
    }

    if((cmdr=rcv(db_sock, "receive for updation of existance in peers"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in receving ack from dbinterface: %s\n", strerror(errno));
        return 1;
    }

    printf("\n[!]Received from dbinterface %s\n", strtok(cmdr, ":"));
    int my_rand=(int)strtol(strtok(NULL, ":"), NULL, 10);

    fclose(f);
    free(cmdr);
    return 0;
}

