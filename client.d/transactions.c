#define NEEDS_ALL

#include"transactions.h"
#include"authenticate_with_const_peer.h"
#include"get_connect_to_new_peer.h"
#include"authenticate_with_tmp_peer.h"
#include"allocate.h"
#include"encrypt_snd_rcv.h"
#include"snd_rcv.h"
#include"global_defs.h"
#include<string.h>
#include<unistd.h>
#include<sodium.h>
#include<pthread.h>

struct wait_struct
{
    int rand_delay;
    int *num;
};

int transactions(char *ku_fname)
{
    server_ip=(char *)allocate("char", 32);
    printf("\n[!]Where to send?: ");
    fgets(server_ip, sizeof(char)*32, stdin);

    if(authenticate_with_const_peer(ku_fname))
    {
        return 1;
    }

    if(init_dialog())
    {
        return 1;
    }
    return 0;
}

int init_dialog()
{
    pthread_t wait_thr_id;
    char *retval, *cmds, *cmdr;
    struct wait_struct ws;
    int stat;

    while(1)
    {
        int flag=0;
        ws.rand_delay=(int)randombytes_uniform(10);
        *(ws.num)=1;

        if((stat=pthread_create(&wait_thr_id, NULL, wait_thr_run, &ws))!=0)
        {
            fprintf(stderr, "\n[-]Error in starting wait thread: %s\n", strerror(stat));
            break;
        }

        while(!(*(ws.num)))
        {
            //get input
            cmds=(char *)allocate("char", 2048);
            printf("[>] ");
            fgets(cmds, sizeof(char)*2048, stdin);
            sprintf(cmds, "%d:%s", pkt_num++, cmds);

            //check exit conditions

            //encrypt and send
            if(e_snd(tmp_peer.p.sock, cmds, "SEND TO TMP PEER"))
            {
                flag=1;
                break;
            }

            //decrypt and recv
            if((cmdr=d_rcv(tmp_peer.p.sock, "RECEIVE FROM PEER"))==NULL)
            {
                flag=1;
            }

            printf("\n[!]Received: %s\n", cmdr);
            free(cmdr);
        }

        if((stat=pthread_join(wait_thr_id, (void **)&retval))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to wait_thread: %s\n", strerror(stat));
            break;
        }

        //send and rcv end to/from tmp peer
        if(snd(tmp_peer.p.sock, "END", "end current tmp peer const cxn"))
        {
            fprintf(stderr, "\n[-]Error in ending const peer cxn\n");
            break;
        }
        if((cmdr=rcv(tmp_peer.p.sock, "rcv final ack from tmp_peer"))==NULL)
        {
            fprintf(stderr, "\n[-]Error in receving final ack from tmp peer\n");
            break;
        }
        printf("\n[!]%s\n", cmdr);
        free(cmdr);

        //find new tmp_peer
        if(find_new_peer())
        {
            fprintf(stderr, "\n[-]Error in finding new tmp peer\n");
            break;
        }

        if(flag)
        {
            fprintf(stderr, "\n[-]Error\n");
            break;
        }
    }
}

void *wait_thr_run(void *r)
{
    struct wait_struct ws=*(struct wait_struct *) r;

    sleep(ws.rand_delay);
    *(ws.num)=0;

    pthread_exit("SUCCESS");
}

int find_new_peer()
{
    int stat=0;
    char *cmds=(char *)allocate("char", 2048);
    sprintf(cmds, "%d:GIBBRESH", pkt_num++);
    
    tmp_peer=get_connect_to_new_peer(&stat, NULL);
    if(stat)
    {
        return 1;
    }
    

    if(authenticate_with_tmp_peer(cmds))
    {
        return 1;
    }

    return 0;
}
