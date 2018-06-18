#include"wait_thr_run.h"
#include"init_send.h"
#include<unistd.h>
#include<pthread.h>

void *wait_thr_run(void *w)
{
    struct wait_thr_combo *wtc=(struct wait_thr_combo *)w;

    sleep(wtc->rand_time);

    wtc->a=0;

    pthread_exit("SUCCESS");
}
