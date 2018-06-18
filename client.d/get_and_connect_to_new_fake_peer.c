#include"get_and_connect_to_new_fake_peer.h"
#include"connect_to_fake_peer.h"
#include"get_rand_peer.h"

int get_and_connect_to_new_fake_peer(struct peer_combo *pc)
{
    if((pc->rand_sno=get_rand_sno())==-1)
    {
        return 1;
    }

    if(get_rand_peer(&pc->p, pc->rand_sno, NULL))
    {
        return 1;
    }

    if(connect_to_fake_peer(&pc->p))
    {
        return 1;
    }

    return 0;
}
