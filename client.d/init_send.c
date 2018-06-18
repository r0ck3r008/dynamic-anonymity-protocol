#include"init_send.h"
#include"allocate.h"
#include"get_rand_sno.h"
#include"encrypt_snd_rcv.h"
#include"get_and_connect_to_new_fake_peer.h"
#include"wait_thr_run.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<sodium.h>
#include<errno.h>

int init_send()
{
    char *retval, *cmds, *cmdr;
    int stat;
    pthread_t wait_thr;
    struct peer_combo *pc=(struct peer_combo *)allocate("struct peer_combo", 1);
    struct wait_thr_combo wtc;

    while(1)
    {
        cmds=(char *)allocate("char", 2048);
        wtc.a=1;
        wtc.rand_time=(int)randombytes_uniform(5);

        if(get_and_connect_to_new_fake_peer(pc))
        {
            continue;
        }

        if((stat=pthread_create(&wait_thr, NULL, wait_thr_run, &wtc))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating new wait thread: %s\n", strerror(stat));
            continue;
        }

        while(wtc.a)
        {
            printf("\n[>] ");
            fgets(cmds, sizeof(char)*2048, stdin);

            if(e_snd(pc->p.sock, cmds, "final send"))
            {
                continue;
            }
            printf("\n[!]Sent %s\n", cmds);

            if((cmdr=d_rcv(pc->p.sock, "final rcv"))==NULL)
            {
                continue;
            }

            printf("\n[!]Received: %s\n", cmdr);
        }

        if((stat=pthread_join(wait_thr, (void **)retval))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to waiting thread: %s\n", strerror(stat));
            continue;
        }

        printf("\n[!]wait_thr returned with %s status\n", (char *)retval);
        explicit_bzero(pc, sizeof(struct peer_combo));
        close(pc->p.sock);
        free(cmdr);
    }

    free(pc);
    return 1;
}
