#define NEEDS_ALL

#include"peer_run.h"
#include"common_headers/snd_rcv.h"
#include"common_headers/allocate.h"
#include"common_headers/gen_keys.h"
#include"connect_by_addr.h"
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<openssl/err.h>

void *peer_run(void *p)
{
    struct joinee *peer=(struct joinee *)p;
    int pkt_num, cli_rand_num, flag=0, flag1=0, final_server_sock;
    char *cmdr_en, *cmds, *cmdr, *retval=(char *)allocate("char", 64), *cmds_final;
    char *final_ip=(char *)allocate("char", 32);

    while(1)
    {
        //rcv clients command from tmp_peer and decrypt it
        if((cmdr_en=rcv(peer->sock, "recv from peer"))==NULL)
        {
            sprintf(retval, "ERROR IN RECEVING DECRYPTED FROM PEER");
            break;
        }
        if(strcmp(cmdr_en, "END")==0)
        {
            explicit_bzero(retval, sizeof(char)*64);
            sprintf(retval, "ENDING CONNECTION FOR TMP PEER");
            break;
        }
        else if(strcmp(cmdr_en, "SERVER_END")==0)
        {
            explicit_bzero(&clients[peer->id], sizeof(struct client));
            sprintf(retval, "ENDING FINAL SERVER CONNECTION...");
            flag=1; 
            break;
        }
        cli_rand_num=(int)strtol(strtok(cmdr_en, ":"), NULL, 10);
        if((cmdr=decrypt(strtok(NULL, ":")))==NULL)
        {
            sprintf(retval, "ERROR IN DECRYPTION OF MSG");
            break;
        }
        free(cmdr_en);

        //parse the decrypted output
        pkt_num=(int)strtol(strtok(cmdr, ":"), NULL, 10);
        if(!pkt_num)    //first time
        {
            int stat;
            sprintf(final_ip, "%s", strtok(NULL, ":"));
            //connect to final server
            if(connect_to_final_server(final_ip, peer->id))
            {
                sprintf(retval, "ERROR IN CONNECTING TO FINAL SERVER");
                break;
            }
            clients[peer->id].cli_rand_num=cli_rand_num;
            //gen public key
            if(write_and_gen_ku(peer->id, strtok(NULL, ":")))
            {
                sprintf(retval, "ERROR IN GENERATING KU");
                break;
            }
            final_server_sock=clients[peer->id].final_server_sock;

            if(inc_max_clients(retval))
            {
                sprintf(retval, "ERROR IN INCREMENTING CURRENT CLIENTS EXISTANCE ARRAY");
                break;
            }
            flag1=1;
            continue;
        }
        else if(!flag1)    //means a new tmp_peer has connected in current connection
        {
            //find final server socket
            if((peer->id=find_in_clients_arr(cli_rand_num))==-1)
            {
                sprintf(retval, "ERROR IN FINDING FINAL SERVER SOCKET");
                break;
            }
            continue;
        }

        //send to final server and rcv ans
        cmds=(char *)allocate("char", 2048);
        sprintf(cmds, "%s", strtok(NULL, ":"));
        if(snd(clients[peer->id].final_server_sock, cmds, "send clients msg to final server"))
        {
            sprintf(retval, "ERROR IN SENDING CLIENT'S MSG TO FINAL SERVER");
            break;
        }
        free(cmdr);
        if((cmdr=rcv(clients[peer->id].final_server_sock, "rcv output from final serve"))==NULL)
        {
            sprintf(retval, "ERROR IN RECEVING FROM FINAL SERVER");
            break;
        }

        //encrypt final server output and send back
        if((cmdr_en=encrypt(clients[peer->id].ku, cmdr))==NULL)
        {
            sprintf(retval, "ERROR IN ENCRYPTING SERVER OUTPUT");
            break;
        }
        free(cmdr);
        cmds=(char *)allocate("char", 2048);
        sprintf(cmds, "%d:%s", cli_rand_num, cmdr_en);
        if(snd(peer->sock, cmds, "send back to tmp_peer"))
        {
            sprintf(retval, "ERROR IN SENDING BACK TO TMP_PEER");
            break;
        }
        free(cmdr_en);
    }

    cmds_final=(char *)allocate("char", 2048);
    sprintf(cmds_final, "%s", retval);
    if(snd(peer->sock, cmds_final, "SEND FINAL ACK"))
    {
        explicit_bzero(retval, 64*sizeof(char));
        sprintf(retval, "ERROR IN SENDING FINAL ACK");
    }
    close(peer->sock);
    if(flag)
    {
        close(clients[peer->id].final_server_sock);
    }
    pthread_exit(retval);
}

char *decrypt(char *cmdr_en)
{
    char *cmdr=(char *)allocate("char", 2048);

    if(RSA_private_decrypt(RSA_size(kv), cmdr_en, cmdr, kv, RSA_PKCS1_PADDING)<0)
    {
        fprintf(stderr, "\n[-]Error in decrypting: %s\n", strerror(ERR_get_error()));
        return NULL;
    }

    return cmdr;
}

char *encrypt(RSA *c_ku, char *cmds)
{
    char *cmds_en=(char *)allocate("char", 2048);

    if(RSA_public_encrypt(RSA_size(c_ku)-11, cmds, cmds_en, c_ku, RSA_PKCS1_PADDING)<0)
    {
        fprintf(stderr, "\n[-]Error in encrypting %s: %s\n", cmds, strerror(ERR_get_error()));
        return NULL;
    }

    return cmds_en;
}

int connect_to_final_server(char *ip, int id)
{
    clients[id].final_server_addr.sin_addr.s_addr=inet_addr(ip);
    clients[id].final_server_addr.sin_port=htons(12346);
    clients[id].final_server_addr.sin_family=AF_INET;

    if((clients[id].final_server_sock=connect_by_addr(clients[id].final_server_addr))==-1)
    {
        return 1;
    }

    free(ip);
}

int write_and_gen_ku(int id, char *key)
{
    FILE *f;
    clients[id].ku_fname=(char *)allocate("char", 32);

    sprintf(clients[id].ku_fname, "%d_ku.pem", clients[id].cli_rand_num);

    if((f=fopen(clients[id].ku_fname, "w"))==NULL)
    {
        fprintf(stderr, "\n[-]Error in opening file %s: %s\n", clients[id].ku_fname, strerror(errno));
        return 1;
    }
    for(int i=0; i<strlen(key); i++)
    {
        fprintf(f, "%c", key[i]);
    }
    fclose(f);

    if((clients[id].ku=gen_keys(clients[id].ku_fname, 1))==NULL)
    {
        return 1;
    }

    return 0;
}

int inc_max_clients(char *retval)
{
    int stat;

    if((stat=pthread_mutex_lock(&max_clients_mutex))!=0)
    {
        explicit_bzero(retval, sizeof(char)*64);
        sprintf(retval, "ERROR IN LOCKING MAX_CLIENTS MUTEX FOR INC");
        return 1;
    }
    max_clients++;
    if((stat=pthread_mutex_unlock(&max_clients_mutex))!=0)
    {
        explicit_bzero(retval, sizeof(char)*64);
        sprintf(retval, "ERROR IN UNLOCKING MAX_CLIENTS MUTEX FOR DEC");
        return 1;
    }

    return 0;
}

int find_in_clients_arr(int cli_rand_num)
{
    int i, flag;
    for(i=0, flag=0; i<max_clients && flag==0; i++)
    {
        if(clients[i].cli_rand_num==cli_rand_num)
        {
            flag=1;
        }
    }

    if(flag==0)
    {
        return -1;
    }
    return i; 
}
