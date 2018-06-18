#include"cli_run.h"
#include"allocate.h"
#include"snd_rcv.h"
#include"server_run.h"
#include"dbconnect.h"
#include"connect_to_fellow_peer.h"
#include"gen_keys.h"
#include"update_existance_in_peers.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<openssl/rsa.h>
#include<openssl/err.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<errno.h>

void *cli_run(void *c)
{
    struct client cli=*(struct client *)c;
    char *cmds=(char *)allocate("char", 2048), *cmdr, *cmdr_en;
    int bit;

    printf("\n[!]Now handelling client %s:%d\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port));
    
    if((cmdr=rcv(cli.sock, "to receive first packet\n"))==NULL)
    {
        pthread_exit("ERROR IN RECEVING");
    }
    
    bit=(int)strtol(strtok(cmdr, ":"), NULL, 10);
    cli.fellow=bit;
    if(bit) //no need to make a new fellow as this is the fellow
    {
        FILE *f;
        cmdr_en=strtok(NULL, ":");
        char *cmds_db=(char *)allocate("char", 2048), *cmdr_db;

        if(RSA_private_decrypt(RSA_size(kv), cmdr_en, cmds, kv, RSA_PKCS1_PADDING)<0)
        {
            fprintf(stderr, "\n[-]Error in decrypting for %s:%d: %s\n", inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port), ERR_get_error());
            pthread_exit("ERROR IN DECRYPTING");
        }

        sprintf(cmds_db, "1:0:0:%d", my_rand);
        if(snd(db_sock, cmds_db, "to send client join info to dbinterface"))
        {
            pthread_exit("ERROR IN SENDING");
        }

        if((cmdr_db=rcv(db_sock, "receive client's newly generated random number"))==NULL)
        {
            pthread_exit("ERROR IN RECEVING");
        }

        printf("\n[!]Db_interface updated clients with status %s for peer %s:%s\n", strtok(cmdr_db, ":"), inet_ntoa(cli.addr.sin_addr), ntohs(cli.addr.sin_port));
        if((f=fopen(strtok(NULL, ":"), "w"))==NULL)
        {
            fprintf(stderr, "\n[-]Error in opening file %s: %s\n", cmdr_db, strerror(errno));
            pthread_exit("ERROR IN OPENING FILE");
        }

        for(int i=0; i<strlen(cmds); i++)
        {
            fprintf(f, "%c", cmds[i]);
        }
        fclose(f);
        free(cmdr_db);
    }
    else    //make a new fellow
    {
        struct client fellow;
        fellow.addr.sin_addr.s_addr=inet_addr(strtok(NULL, ":"));
        fellow.addr.sin_port=htons(6666);
        fellow.addr.sin_family=AF_INET;
        cmdr_en=strtok(NULL, ":");

        if(connect_to_fellow_peer(&fellow))
        {
            pthread_exit("ERROR IN CONNECTING TO FELLOW PEER");
        }

        sprintf(cmds, "1:%s", cmdr_en); //1 for peer-peer send/recv
        if(snd(fellow.sock, cmds, "send encrypted msg to fellow peer"))
        {
            pthread_exit("ERROR IN SENDING");
        }
    }

    free(cmdr);
    pthread_exit("SUCCESS");
}

