#define NEEDS_ALL

#include"encrypt_snd_rcv.h"
#include"allocate.h"
#include"global_defs.h"
#include"snd_rcv.h"
#include<stdio.h>
#include<openssl/err.h>

int e_snd(int sock, char *cmds, char *reason)
{
    char *cmds_en=(char *)allocate("char", 2048);

    if(RSA_public_encrypt(RSA_size(const_peer.p.ku)-11, cmds, cmds_en, const_peer.p.ku, RSA_PKCS1_PADDING)<0)
    {
        fprintf(stderr, "\n[-]Error in encrypting %s for reason %s: %s\n", cmds, reason, ERR_get_error());
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
    char *cmdr_en, *cmdr=(char *)allocate("char", 2048);

    if((cmdr_en=rcv(sock, reason))==NULL)
    {
        return NULL;
    }

    if(RSA_private_decrypt(RSA_size(kv), cmdr_en, cmdr, kv, RSA_PKCS1_PADDING)<0)
    {
        fprintf(stderr, "\n[-]Error in decrypting for reason %s: %s\n", reason, ERR_get_error());
        return NULL;
    }

    free(cmdr_en);
    return cmdr;
}
