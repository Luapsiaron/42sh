#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static int argc_count(char **argv)
{
    int count = 0;
    while (argv[count] != NULL)
        count++;
    return count;
}

static void
echo_print_escaped(const char *s) // Print string without \? transformed
{
    for (size_t i = 0; s[i]; i++)
    {
        if (s[i] == '\\' && s[i + 1] != '\0')
        {
            char c = s[i + 1];
            if (c == 'n')
            {
                putchar('\n');
                i++;
                continue;
            }
            if (c == 't')
            {
                putchar('\t');
                i++;
                continue;
            }
            if (c == '\\')
            {
                putchar('\\');
                i++;
                continue;
            }
        }
        putchar(s[i]);
    }
}

int builtin_echo(char **argv)
{
    int argc = argc_count(argv);
    bool opt_n = true;
    bool opt_e = false;
    int i = 1;
    while (i < argc && argv[i] && argv[i][0] == '-' && argv[i][1] != '\0')
    {
        if (strcmp(argv[i], "--") == 0)
        {
            i++;
            break;
        }
        if (strcmp(argv[i], "-n") == 0)
        {
            opt_n = false;
            i++;
            continue;
        }
        if (strcmp(argv[i], "-e") == 0)
        {
            opt_e = true;
            i++;
            continue;
        }
        if (strcmp(argv[i], "-E") == 0)
        {
            opt_e = false;
            i++;
            continue;
        }
        break;
    }
    for (int j = i; j < argc; j++)
    {
        if (j > i)
            putchar(' ');

        if (opt_e)
            echo_print_escaped(argv[j]);
        else
            fputs(argv[j], stdout);
    }
    if (opt_n)
        putchar('\n');
    fflush(stdout);
    return 0;
}
