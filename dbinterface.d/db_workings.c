#include"db_workings.h"
#include"bit_is_one.h"
#include"bit_is_zero.h"
#include<stdlib.h>
#include<string.h>


char *db_workings(struct client cli, char *cmdr)
{
    int bit= (int)strtol(strtok(cmdr, ":"), NULL, 0), peer=(int)strtol(strtok(NULL, ":"), NULL, 10), exp_col=(int)strtol(strtok(NULL, ":"), NULL, 10);
    char *cmds;

    if(bit)
    {
        if((cmds=bit_is_one(cli, peer, exp_col))==NULL)
        {
            return NULL;
        }
    }
    else
    {
        if((cmds=bit_is_zero(cli, peer, exp_col))==NULL)
        {
            return NULL;
        }
    }

    return cmds;
}

