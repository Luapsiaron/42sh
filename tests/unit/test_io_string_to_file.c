#include <criterion/criterion.h>
#include <criterion/redirect.h>

FILE *io_string_to_file(char *string);

static void redirect_all_stdout(void)
{
    cr_redirect_stdout();
}

Test(io_string_to_file, simple_echo, .init = redirect_all_stdout)
{
    FILE *f = io_string_to_file("echo test");
    fseek(f, 0, SEEK_SET);
    char buf[4096] = { 0 };

    if (fgets(buf, sizeof(buf), f))
    {}

    fclose(f);
    cr_assert_str_eq(buf, "echo test");
}

Test(io_string_to_file, empty_string, .init = redirect_all_stdout)
{
    FILE *f = io_string_to_file("");
    fseek(f, 0, SEEK_SET);
    char buf[4096] = { 0 };

    if (fgets(buf, sizeof(buf), f))
    {}

    fclose(f);
    cr_assert_str_eq(buf, "");
}
