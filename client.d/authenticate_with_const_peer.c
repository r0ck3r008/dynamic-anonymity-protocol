#include"authenticate_with_const_peer.h"
#include"allocate.h"
#include"get_rand_sno.h"
#include"get_rand_peer.h"
#include"get_and_connect_to_new_fake_peer.h"
#include"snd_rcv.h"
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<openssl/rsa.h>
#include<openssl/err.h>
#include<errno.h>

int authenticate_with_const_peer(char *fname)
{
    struct peer_combo fake_peer;
    char *cmds=(char *)allocate("char", 2048);
    char *cmds_en=(char *)allocate("char", 2048);
    FILE *f;

    if(get_and_connect_to_new_fake_peer(&fake_peer))
    {
        return 1;
    }

    if((f=fopen(fname, "r"))==NULL);
    {
        fprintf(stderr, "\n[-]Error in opening %s for fake_connect: %s\n", fname, strerror(errno));
        return 1;
    }
    for(int i=0; !feof(f); i++)
    {
        fscanf(f, "%c", &cmds[i]);
    }

    if(RSA_public_encrypt(RSA_size(const_peer.p.ku)-11, cmds, cmds_en, const_peer.p.ku, RSA_PKCS1_PADDING)<0)
    {
        fprintf(stderr, "\n[-]Error in encrypting public key in ku: %s\n", ERR_get_error());
        return 1;
    }
    sprintf(cmds_en, "0:%s:%s", inet_ntoa(const_peer.p.addr.sin_addr), cmds_en);    //zero means its not the main recepient

    if(snd(fake_peer.p.sock, cmds_en, "send the public key to const_peer\n"))
    {
        return 1;
    }

    fclose(f);
    close(fake_peer.p.sock);
    free(cmds);
    return 0;
}

