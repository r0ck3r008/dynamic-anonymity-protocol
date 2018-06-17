#include"get_passwd.h"
#include"allocate.h"
#include<stdio.h>
#include<string.h>
#include<termios.h>
#include<errno.h>

char *get_passwd()
{
    char *passwd=(char *)allocate("char", 50);
    struct termios a, b;
    explicit_bzero(&a, sizeof(a));
    explicit_bzero(&b, sizeof(b));

    if(tcgetattr(fileno(stdin), &a)<0)
    {
        fprintf(stderr, "\n[-]Error in getting attributes: %s\n", strerror(errno));
        return NULL;
    }

    b=a;
    a.c_lflag &= ~ECHO;
    a.c_lflag |= ECHONL;

    if(tcsetattr(fileno(stdin), TCSANOW, &a)<0)
    {
        fprintf(stderr, "\n[-]Error in setting no echo flag :%s\n", strerror(errno));
        return NULL;
    }

    fgets(passwd, sizeof(char)*50, stdin);

    if(tcsetattr(fileno(stdin), TCSANOW, &b)<0)
    {
        fprintf(stderr, "\n[-]Error in makin stdin normal: %s\n", strerror(errno));
        return NULL;
    }

    return passwd;
}


