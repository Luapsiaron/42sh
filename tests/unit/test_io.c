#include <criterion/criterion.h>
#include <criterion/redirect.h>

FILE *io_string_to_file(char *string);

static void assert_file_equals_string(FILE *f, const char *expected)
{
    cr_assert_not_null(f, "io_string_to_file returned NULL");

    char buf[4096];
    size_t n = fread(buf, 1, sizeof(buf) - 1, f);
    buf[n] = '\0';

    cr_assert_eq(ferror(f), 0, "Error while reading FILE*");
    cr_assert_str_eq(buf, expected);
}

Test(io_string_to_file, simple_echo)
{
    char* str = "echo test";
    FILE *f = io_string_to_file(str);
    assert_file_equals_string(f, str);
    fclose(f);
}

Test(io_string_to_file, empty_string)
{
    char* str = "";
    FILE *f = io_string_to_file(str);
    assert_file_equals_string(f, str);
    fclose(f);
}

Test(io_string_to_file, newline_inside)
{
    char* str = "echo\n test";
    FILE *f = io_string_to_file(str);
    assert_file_equals_string(f, str);
    fclose(f);
}

Test(io_string_to_file, null_string)
{
    FILE *f = io_string_to_file(NULL);
    cr_assert_null(f, "io_string_to_file should return NULL for NULL input");
}

