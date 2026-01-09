#include <criterion/criterion.h>
#include <criterion/redirect.h>

int builtin_echo(char **argv);

static void redirect_all_stdout(void)
{
    cr_redirect_stdout();
}

// Newline tests

Test(echo, no_args_prints_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("\n");
}

Test(echo, one_args_prints_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "hello", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\n");
}

Test(echo, multiple_args_prints_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "hello", "world", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello world\n");
}

Test(echo, empty_args_prints_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "", "world", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str(" world\n");
}

// No newline tests

Test(echo, useless_prints_no_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-n", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("");
}

Test(echo, one_args_prints_no_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-n", "hello", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello");
}

Test(echo, multiple_args_prints_no_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-n", "hello", "world", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello world");
}

Test(echo, empty_args_prints_no_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-n", "", "world", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str(" world");
}

// Interpretation enabled tests

Test(echo, option_e_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-e", "hello\\nworld", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\nworld\n");
}

Test(echo, option_e_tab, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-e", "hello\\tworld", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\tworld\n");
}

Test(echo, option_e_backslash, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-e", "hello\\\\world", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\\world\n");
}

Test(echo, option_e_all, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-e", "\\nhello\\\\world\\t", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("\nhello\\world\t\n");
}

// Interpretation disabled tests

Test(echo, option_E_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-E", "hello\\nworld", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\\nworld\n");
}

Test(echo, option_E_tab, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-E", "hello\\tworld", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\\tworld\n");
}

Test(echo, option_E_backslash, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-E", "hello\\\\world", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\\\\world\n");
}

Test(echo, option_E_all, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-E", "\\nhello\\\\world\\t", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("\\nhello\\\\world\\t\n");
}

// Both -e and -E provided (last one takes precedence)

Test(echo, option_eE_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-e", "-E", "hello\\nworld", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\\nworld\n");
}

Test(echo, option_eEe_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-e", "-E", "-e", "hello\\nworld", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\nworld\n");
}

Test(echo, option_Ee_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-E", "-e", "hello\\nworld", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\nworld\n");
}

Test(echo, option_EeE_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-E", "-e", "-E", "hello\\nworld", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\\nworld\n");
}

// Everything combined tests

Test(echo, option_E_no_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-E", "-n", "hello\\nworld", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\\nworld");
}

Test(echo, option_e_no_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-e", "-n", "hello\\nworld", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\nworld");
}

Test(echo, option_eE_no_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-e", "-E", "-n", "hello\\nworld", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\\nworld");
}

Test(echo, option_Ee_no_newline, .init = redirect_all_stdout)
{
    char *argv[] = { "echo", "-E", "-e", "-n", "hello\\nworld", NULL };
    builtin_echo(argv);
    cr_assert_stdout_eq_str("hello\nworld");
}