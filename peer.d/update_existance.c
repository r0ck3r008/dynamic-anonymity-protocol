#include"update_existance.h"
#include"allocate.h"
#include"snd_rcv.h"
#include"global_defs.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>

int update_existance(char *ku_fname, char *argv1)
{
    FILE *f;
    char *cmds=(char *)allocate("char", 2048), *cmdr, *key=(char *)allocate("char", 1500);

    if(get_sno_and_rand_id())
    {
        return 1;
    }

    if((f=fopen(ku_fname, "r"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening file %s: %s\n", ku_fname, strerror(errno));
        return 1;
    }
    for(int i=0; !feof(f); i++)
    {
        fscanf(f, "%c", &key[i]);
    }
    sprintf(cmds, "0:insert into peers values (%d, %d, '%s', '%s');", my_sno, my_rand_id, strtok(argv1, ":"), key);

    if(snd(db_sock, cmds, "send existance insertion query"))
    {
        return 1;
    }

    if((cmdr=rcv(db_sock, "receive ack on existance insertion query"))==NULL)
    {
        return 1;
    }

    if(strcmp(strtok(cmdr, ":"), "SUCCESS")!=0)
    {
        fprintf(stderr, "\n[-]Existance insertion query failed: %s\n", cmdr);
        return 1;
    }

    printf("\n[!]existance updated with status: %s\n", strtok(NULL, ":"));
    free(cmdr);
    free(key);
    return 0;
}

int get_sno_and_rand_id()
{
    char *cmdr;

    if((cmdr=rcv(db_sock, "receive sno and rand_id from dbinterface"))==NULL)
    {
        return 1;
    }

    if(strcmp(strtok(cmdr, ":"), "NUMS")!=0)
    {
        fprintf(stderr, "\n[-]Cant parse %s\n", cmdr);
        return 1;
    }
    
    my_sno=(int)strtol(strtok(NULL, ":"), NULL, 10);
    my_rand_id=(int)strtol(strtok(NULL, ":"), NULL, 10);

    free(cmdr);
    return 0;
}
