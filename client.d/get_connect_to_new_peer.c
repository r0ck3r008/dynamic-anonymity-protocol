#define NEEDS_STRUCT_GLOBALS

#include"get_connect_to_new_peer.h"
#include"get_rand_sno.h"
#include"get_rand_peer.h"
#include"connect_to_new_peer.h"

struct peer_combo get_connect_to_new_peer(int *a, char *ku_fname)
{
    struct peer_combo pc;
    if((pc.rand_sno=get_rand_sno())==-1)
    {
        *a=1;
    }

    if(get_rand_peer(&pc.p, pc.rand_sno, ku_fname))
    {
        *a=1;
    }

    if(ku_fname!=NULL)
    {
        if(connect_to_new_peer(&pc.p))
        {
            *a=1;
        }
    }
    return pc;
}
