#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <unistd.h>

FILE *io_stdin_to_file(void);

static int argv_len(char **argv)
{
    int len = 0;
    while (argv[len] != NULL)
        len++;
    return len;
}

static void redirect_all_stdout(void)
{
    cr_redirect_stdout();
}

// Newline tests

Test(io_stdin_to_file, simple_echo, .init = redirect_all_stdout)
{
    FILE *f = tmpfile();
    fputs("echo test", f);
    rewind(f);

    int old_stdin = dup(STDIN_FILENO);
    dup2(fileno(f), STDIN_FILENO);

    FILE *f3 = io_stdin_to_file();
    fseek(f3, 0, SEEK_SET);
    char buf[100];
    fgets(buf, sizeof(buf), f3);

    dup2(old_stdin, STDIN_FILENO);
    close(old_stdin);
    fclose(f);
    cr_assert_str_eq(buf, "echo test");
}

Test(io_stdin_to_file, empty_string, .init = redirect_all_stdout)
{
    FILE *f = tmpfile();
    fputs("", f);
    rewind(f);

    int old_stdin = dup(STDIN_FILENO);
    dup2(fileno(f), STDIN_FILENO);

    FILE *f3 = io_stdin_to_file();
    fseek(f3, 0, SEEK_SET);
    char buf[100];
    fgets(buf, sizeof(buf), f3);

    dup2(old_stdin, STDIN_FILENO);
    close(old_stdin);
    fclose(f);
    cr_assert_str_eq(buf, "");
}
