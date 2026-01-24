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

int builtin_echo(char **argv) // executes the echo builtin
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
        bool all_valid = true;
        for (int k = 1; argv[i][k] != '\0'; k++)
        {
            char c = argv[i][k];
            if (c == 'n')
                opt_n = false;
            else if (c == 'e')
                opt_e = true;
            else if (c == 'E')
                opt_e = false;
            else
            {
                all_valid = false;
                break;
            }
        }
        if (!all_valid)
            break;
        i++;
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
