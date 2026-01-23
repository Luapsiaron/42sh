#include "parser_internal.h"

/*
    Parse a single command
    Grammar: command = simple_command | if_command | while_command | for_command
   | until_command

    Examples:
    - if true; then echo A; fi -> TOKEN_IF -> parse_if
    - while false; do echo B; done -> TOKEN_WHILE -> parse_while
    - A=1 echo hello -> TOKEN_ASSIGNMENT_WORD -> parse_simple_command
    - >out echo test -> TOKEN_GREAT -> parse_simple_command
*/
struct ast *parse_command(struct parser *p)
{
    if (peek(p) == TOKEN_WORD && peek_next(p) == TOKEN_LPAREN)
    {
        return parse_funcdec(p);
    }
    struct ast *shell_cmd = parse_shell_command(p);
    if (shell_cmd)
    {
        struct ast *redirs = parse_redir_list(p);
        if (redirs)
        {
            struct ast *wrap = ast_redirwrap_init(shell_cmd, redirs);
            if (!wrap)
            {
                ast_free(shell_cmd);
                ast_free(redirs);
                return NULL;
            }
            return wrap;
        }
        return shell_cmd;
    }

    if (peek(p) == TOKEN_WORD || peek(p) == TOKEN_ASSIGNMENT_WORD
        || peek(p) == TOKEN_IONUMBER || is_redirection_token(peek(p)))
    {
        return parse_simple_command(p);
    }
    return NULL;
}
