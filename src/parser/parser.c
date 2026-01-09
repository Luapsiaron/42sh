#include "parser.h"

#include <stdlib.h>

#include "../lexer/lexer.h"
#include "../utils/str/str.h"

static lexer_t c_lexer;
static token_t *c_token;

static token_type_t peek(void)
{
    // printf("type: %s\n", token_type_name(c_token->type));
    if (!c_token)
    {
        return TOKEN_EOF;
    }
    return c_token->type;
}

static void pop(void)
{
    if (c_token)
    {
        token_free(c_token);
    }
    c_token = lexer_next(&c_lexer);
}
static int is_semicolon_newline(token_type_t t)
{
    return t == TOKEN_SEMICOLON || t == TOKEN_NEWLINE;
}

static void skip_semicolon_newline(void)
{
    while (is_semicolon_newline(peek()))
    {
        pop();
    }
}

static ast_t *parse_simple_command(void)
{
    skip_semicolon_newline();
    if (peek() != TOKEN_WORD)
    {
        return NULL;
    }

    char **argv = calloc(16, sizeof(char *));
    if (!argv)
    {
        return NULL;
    }

    int i = 0;
    // echo if then fi else elif
    while (!is_semicolon_newline(peek()) && peek() != TOKEN_EOF)
    {
        token_type_t token_type = peek();
        if (token_type == TOKEN_WORD)
        {
            argv[i] = xstrdup(c_token->lexeme);
        }
        else if (token_type == TOKEN_IF)
        {
            argv[i] = xstrdup("if");
        }
        else if (token_type == TOKEN_THEN)
        {
            argv[i] = xstrdup("then");
        }
        else if (token_type == TOKEN_ELIF)
        {
            argv[i] = xstrdup("elif");
        }
        else if (token_type == TOKEN_ELSE)
        {
            argv[i] = xstrdup("else");
        }
        else if (token_type == TOKEN_FI)
        {
            argv[i] = xstrdup("fi");
        }

        if (!argv[i])
        {
            free_argv(argv);
            return NULL;
        }
        i++;
        pop();
    }
    argv[i] = NULL;

    return ast_cmd_init(argv);
}

ast_t *parse_condition(void)
{
    ast_t *list = ast_list_init(NULL, parse_simple_command());;
    ast_t *tmp = list;

    while (peek() != TOKEN_THEN)
    {
        if (peek() == TOKEN_EOF)
        {
            return NULL;
        }
        tmp->data.ast_list.next = ast_list_init(NULL, parse_simple_command());
        tmp = tmp->data.ast_list.next;
        skip_semicolon_newline();
    }
    return list;
}

ast_t *parse_then(void)
{
    ast_t *list = ast_list_init(NULL, parse_simple_command());;
    ast_t *tmp = list;

    while (peek() != TOKEN_FI && peek() != TOKEN_ELSE && peek() != TOKEN_ELIF)
    {
        if (peek() == TOKEN_EOF)
        {
            return NULL;
        }
        tmp->data.ast_list.next = ast_list_init(NULL, parse_simple_command());
        tmp = tmp->data.ast_list.next;
        skip_semicolon_newline();
    }
    return list;
}

ast_t *parse_else(void)
{
    ast_t *list = ast_list_init(NULL, parse_simple_command());;
    ast_t *tmp = list;

    while (peek() != TOKEN_FI)
    {
        if (peek() == TOKEN_EOF)
        {
            return NULL;
        }
        tmp->data.ast_list.next = ast_list_init(NULL, parse_simple_command());
        tmp = tmp->data.ast_list.next;
        skip_semicolon_newline();
    }
    return list;
}

static ast_t *parse_elif(void)
{
    pop();

    skip_semicolon_newline();
    ast_t *condition = parse_condition();
    if (!condition)
    {
        return NULL;
    }

    skip_semicolon_newline();
    if (peek() != TOKEN_THEN)
    {
        ast_free(condition);
        return NULL;
    }
    pop();

    ast_t *then_body = parse_then();
    if (!then_body)
    {
        ast_free(condition);
        return NULL;
    }

    skip_semicolon_newline();

    ast_t *elif_body = NULL;
    if (peek() == TOKEN_ELIF)
    {
        elif_body = parse_elif_command();
        if (!elif_body)
        {
            ast_free(condition);
            ast_free(then_body);
            return NULL;
        }
    }

    skip_semicolon_newline();

    ast_t *else_body = NULL;
    if (peek() == TOKEN_ELSE)
    {
        pop();
        skip_semicolon_newline();
        else_body = parse_else();
        if (!else_body)
        {
            ast_free(condition);
            ast_free(then_body);
            return NULL;
        }
    }

    if (elif_body)
    {
        return ast_if_init(condition, then_body, elif_body);
    }
    return ast_if_init(condition, then_body, else_body);
}

static ast_t *parse_if(void)
{
    pop();

    skip_semicolon_newline();
    ast_t *condition = parse_condition();
    if (!condition)
    {
        return NULL;
    }

    skip_semicolon_newline();
    if (peek() != TOKEN_THEN)
    {
        ast_free(condition);
        return NULL;
    }
    pop();

    skip_semicolon_newline();
    ast_t *then_body = parse_then();
    if (!then_body)
    {
        ast_free(condition);
        return NULL;
    }

    skip_semicolon_newline();

    ast_t *elif_body = NULL;
    if (peek() == TOKEN_ELIF)
    {
        elif_body = parse_elif_command();
        skip_semicolon_newline();
        if (!elif_body)
        {
            ast_free(condition);
            ast_free(then_body);
            return NULL;
        }
    }

    skip_semicolon_newline();

    ast_t *else_body = NULL;
    if (peek() == TOKEN_ELSE)
    {
        pop();
        skip_semicolon_newline();
        else_body = parse_else();
        if (!else_body)
        {
            ast_free(condition);
            ast_free(then_body);
            return NULL;
        }
    }
    skip_semicolon_newline();
    if (peek() != TOKEN_FI)
    {
        ast_free(condition);
        ast_free(then_body);
        ast_free(else_body);
        return NULL;
    }
    pop();

    if (elif_body)
    {
        return ast_if_init(condition, then_body, elif_body);
    }
    return ast_if_init(condition, then_body, else_body);
}

ast_t *parse_elif_command(void)
{
    if (peek() == TOKEN_ELIF)
    {
        return parse_elif();
    }
    return NULL;
}

ast_t *parse_command(void)
{
    if (peek() == TOKEN_IF)
    {
        return parse_if();
    }
    else if (peek() == TOKEN_WORD)
    {
        return parse_simple_command();
    }
    return NULL;
}

static ast_t *parse_list(void)
{
    skip_semicolon_newline();

    ast_t *child = parse_command();
    if (!child)
    {
        return NULL;
    }
    ast_t *head = ast_list_init(NULL, child);
    if (!head)
    {
        ast_free(child);
        return NULL;
    }

    ast_t *tail = head;

    while (1)
    {
        if (!is_semicolon_newline(peek()))
        {
            break;
        }

        skip_semicolon_newline();

        if (peek() == TOKEN_EOF)
        {
            break;
        }

        ast_t *next_child = parse_command();
        if (!next_child)
        {
            ast_free(head);
            return NULL;
        }

        ast_t *next_list = ast_list_init(NULL, next_child);
        if (!next_list)
        {
            ast_free(next_child);
            ast_free(head);
            return NULL;
        }

        tail->data.ast_list.next = next_list;
        tail = next_list;
    }
    return head;
}

ast_t *parse_input(FILE *f)
{
    lexer_init(&c_lexer, f);
    c_token = lexer_next(&c_lexer);
    if (!c_token)
    {
        return NULL;
    }

    skip_semicolon_newline();
    if (peek() == TOKEN_EOF)
    {
        token_free(c_token);
        c_token = NULL;
        return NULL;
    }

    ast_t *root = parse_list();
    if (!root)
    {
        return NULL;
    }

    skip_semicolon_newline();
    if (peek() != TOKEN_EOF)
    {
        ast_free(root);
        return NULL;
    }

    token_free(c_token);
    c_token = NULL;
    return root;
}

/* ==================================================================================
 */
/*
file f = null
char * string = null
parser_input(get_file(f, string))
static FILE *get_file(FILE *f, char *string)
{
    if (!f)
    {
        if (!string)
        {
            f = io_stdin_to_file();
        }
        else
        {
            f = io_string_to_file();
        }
    }
    return f;
}

ast_t parser_init(FILE *f, char *string)
{
    f = get_file(f, string);

    lexer_t lx = NULL;
    lexer_init(lx, f);

    return parser(lx);
}

static char **parser_create_argv(lexer_t lx, token_t token)
{
    int size = 2;
    char **argv = calloc(sizeof(char *) * size);
    if (!argv)
    {
        return NULL;
    }

    argv[0] = token.lexeme;
    argv[1] = NULL;
    free(token);

    int is_word = 1;
    while (is_word)
    {
        token = lexer_next(lx);
        if (!token)
        {
            free_argv(argv);
        }
        // prendre tt les mots prcq si on verif que word "echo if" marchera pas
        // car if -> TOEKN_IF
        if (token.type != TOKEN_SEMICOLON && token.type != TOKEN_NEWLINE
            && token.type != TOKEN_EOF)
        {
            size++;
            argv = realloc(argv, sizeof(char *) * size);
            if (!argv)
            {
                return NULL;
            }
            // size est minimum de taille 3 ici
            argv[size - 2] = token.lexeme;
            argv[size - 1] = NULL;
        }
        else
        {
            is_word = 0;
        }
    }
    return argv;
}

static ast_t *parser_if(lexer_t lx)
{
    ast_t *new = ast_if_init(NULL, NULL, NULL);
    if (!new)
    {
        return NULL;
    }

    token_t *token = lexer_next(lx);
    if (!token || token->type != TOKEN_WORD)
    {
        ast_free(new);
        return NULL;
    }

    new->data.ast_if.condition = parser_create_argv(lx, token);
}

ast_t *parser(lexer_t lx)
{
    token_t *token = NULL;
    // a changer le while on peut pas verif si le token a pas foirer son calloc
    // aussi c quoi la struct tt en haut une ast_list? comme ça on peut alterner
    // echo bonjour;
    // if ...
    // then
    //    echo 1;
    //    echo 2
    // dans le même input
    token = lexer_next(lx);
    if (!token)
    {
        return NULL;
    }

    while (token->type != TOKEN_EOF)
    {
        if (token.type == TOKEN_IF)
        {
            return parser_if(lx);
        }

        if (token)
        {
            free(token);
        }
        token = lexer_next(lx);

        if (!token)
        {
            return NULL;
        }
    }
}
*/