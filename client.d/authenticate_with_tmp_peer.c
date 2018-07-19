#define NEEDS_STRUCT_GLOBALS

#include"authenticate_with_tmp_peer.h"
#include"get_connect_to_new_peer.h"
#include"common_headers/allocate.h"
#include"global_defs.h"
#include"common_headers/snd_rcv.h"
#include<openssl/err.h>
#include<string.h>

int authenticate_with_tmp_peer(char *cmds)
{
    int stat=0;
    char *cmds_en1=(char *)allocate("char", 2048), *cmds_en=(char *)allocate("char", 2048);
    tmp_peer=get_connect_to_new_peer(&stat, NULL);
    if(RSA_public_encrypt(RSA_size(const_peer.p.ku)-11, cmds, cmds_en1, const_peer.p.ku, RSA_PKCS1_PADDING)<0)
    {
        fprintf(stderr, "\n[-]Error in encrypting %s to authenticate with tmp_peer: %s\n", cmds, strerror(ERR_get_error()));
        return 1;
    }

    sprintf(cmds_en, "%s:%d:%s", inet_ntoa(const_peer.p.addr.sin_addr), my_rand_num, cmds_en1);
    if(snd(tmp_peer.p.sock, cmds_en, "send to tmp_peer for auth"))
    {
        return 1;
    }
    
    free(cmds_en1);
    free(cmds);
    return 0;
}
