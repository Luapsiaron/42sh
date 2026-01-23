#include <stdlib.h>
#include <stdio.h>

#include "exit.h"
#include "../expansion/expand.h"

// change last_exit_code and return -3 to catch the exit after in the code
int builtin_exit(char **argv)
{
    if (argv[0] == NULL || argv[1] == NULL)
    {
        fprintf(stderr, "exit: no arg\n");
        return 1;
    }
    if (argv[2] != NULL)
    {
        fprintf(stderr, "exit: too much arg\n");
        return 1;
    }
    int exit_value = atoi(argv[1]);
    if (exit_value == 0 && !(argv[1][0] == '0' && argv[1][1] == 0))
    {
        fprintf(stderr, "exit: non numeric arg\n");
        return 1;
    }
    if (exit_value < 0 || exit_value > 255)
    {
        fprintf(stderr, "exit: arg not between 0 and 255\n");
        return 1;
    }
    last_exit_code = exit_value;
    return -3;
}