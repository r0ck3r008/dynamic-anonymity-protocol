#include"regex_check.h"
#include"common_headers/allocate.h"
#include<stdio.h>
#include<regex.h>
#include<sys/types.h>
#include<string.h>

int regex_check(char *haystack, char *needle)
{
    regex_t regex;
    int stat;
    char *retval=(char *)allocate("char", 128);

    if((stat=regcomp(&regex, needle, 0))!=0)
    {
        fprintf(stderr, "\n[-]%s\n", strerror(stat));
        return 0;
    }

    if((stat=regexec(&regex, haystack, 0, NULL, 0))!=0)
    {
        fprintf(stderr, "\n[-]No match\n", strerror(stat));;
        return 0;
    }

    regfree(&regex);
    return 1;
}
