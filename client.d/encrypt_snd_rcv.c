#include"encrypt_snd_rcv.h"
#include"snd_rcv.h"
#include"allocate.h"
#include"get_rand_sno.h"
#include"gen_keys.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<openssl/err.h>
#include<errno.h>

int e_snd(int sock, char *cmds, char *reason)
{
    char *cmds_en=(char *)allocate("char", 2048);
    if(RSA_public_encrypt(RSA_size(const_peer.p.ku)-11, cmds, cmds_en, const_peer.p.ku, RSA_PKCS1_PADDING)<0)
    {
        fprintf(stderr, "\n[-]Error in encrypting %s for %s: %s\n", cmds, reason, ERR_get_error());
        return 1;
    }

    if(snd(sock, cmds_en, reason))
    {
        return 1;
    }

    free(cmds);
    return 0;
}

char *d_rcv(int sock, char *reason)
{
    char *cmdr_en;
    char *cmdr=(char *)allocate("char", 2048);

    if((cmdr_en=rcv(sock, reason))==NULL)
    {
        return NULL;
    }
    
    if(RSA_private_decrypt(RSA_size(kv), cmdr_en, cmdr, kv, RSA_PKCS1_PADDING)<0)
    {
        fprintf(stderr, "\n[-]Error in decrypting for %s: %s\n", reason, ERR_get_error());
        return NULL;
    }

    free(cmdr_en);
    return cmdr;
}
