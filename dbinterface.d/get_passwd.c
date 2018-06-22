#include"get_passwd.h"
#include"allocate.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<termios.h>
#include<errno.h>

char *get_passwd()
{
    char *passwd=(char *)allocate("char", 64);
    struct termios a, b;
    explicit_bzero(&a, sizeof(a));
    explicit_bzero(&b, sizeof(b));

    if(tcgetattr(fileno(stdin), &a)==-1)
    {
        fprintf(stderr, "\n[-]Error in getting stdin attr: %s\n", strerror(errno));
        return NULL;
    }

    b=a;
    a.c_lflag &= ~ECHO;
    a.c_lflag |= ECHONL;

    if(tcsetattr(fileno(stdin), TCSANOW, &a)==-1)
    {
        fprintf(stderr, "\n[-]Error in setting quite flag in stdin: %s\n", strerror(errno));
        return NULL;
    }

    fgets(passwd, sizeof(char)*64, stdin);

    if(tcsetattr(fileno(stdin), TCSANOW, &b)==-1)
    {
        fprintf(stderr, "\n[-]Error in normalising stdin back: %s\n", strerror(errno));
        return NULL;
    }

    return passwd;
}
