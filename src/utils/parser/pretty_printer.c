#include "pretty_printer.h"

#include <ctype.h>
#include <string.h>

static void pp_node(const ast_t *ast, FILE *out);
static void pp_list(const ast_t *ast, FILE *out);
static void pp_redir(const ast_t *ast, FILE *out);
static void pp_cmd(const ast_t *ast, FILE *out);
static void pp_pipeline(const ast_t *ast, FILE *out);
static void pp_negation(const ast_t *ast, FILE *out);
static void pp_if(const ast_t *ast, FILE *out);
static void pp_and_or(const ast_t *ast, FILE *out);
static void pp_while_until(const ast_t *ast, FILE *out);

static void pp_ignore_quotes(const char *str, FILE *out)
{
    fputc('"', out);
    for (size_t i = 0; str && str[i]; ++i)
    {
        if (str[i] == '"' || str[i] == '\\')
        {
            fputc('\\', out);
        }
        fputc(str[i], out);
    }
    fputc('"', out);
}

static void pp_braces(const ast_t *ast, FILE *out)
{
    fputs("{ ", out);
    pp_node(ast, out);
    fputs(" }", out);
}

static void pp_list(const ast_t *ast, FILE *out)
{
    const ast_t *cur = ast;
    while (cur)
    {
        pp_node(cur->data.ast_list.child, out);
        if (cur->data.ast_list.next)
        {
            fprintf(out, "; ");
        }
        cur = cur->data.ast_list.next;
    }
}

static void pp_redir(const ast_t *ast, FILE *out)
{
    const struct ast_redir *ast_tmp = &ast->data.ast_redir;
    if (ast_tmp->type == REDIR_IN)
    {
        if (ast_tmp->next)
        {
            pp_node(ast_tmp->next, out);
        }
        fprintf(out, " < ");
        fputs(ast_tmp->word, out);
        return;
    }
    else if (ast_tmp->type == REDIR_OUT)
    {
        fprintf(out, " > ");
    }
    else if (ast_tmp->type == REDIR_APPEND)
    {
        fprintf(out, " >> ");
    }
    else if (ast_tmp->type == REDIR_CLOBBER)
    {
        fprintf(out, " >| ");
    }
    fputs(ast_tmp->word, out);
    if (ast_tmp->next)
    {
        pp_node(ast_tmp->next, out);
    }
}

static void pp_cmd(const ast_t *ast, FILE *out)
{
    char **argv = ast->data.ast_cmd.argv;

    fputs("command", out);

    for (size_t i = 0; argv && argv[i]; ++i)
    {
        fputc(' ', out);
        pp_ignore_quotes(argv[i], out);
    }
    if (ast->data.ast_cmd.redirs)
    {
        pp_redir(ast->data.ast_cmd.redirs, out);
    }
}

static void pp_pipeline(const ast_t *ast, FILE *out)
{
    fputs("pipeline { ", out);
    pp_node(ast->data.ast_pipeline.left, out);
    fprintf(out, " | ");
    pp_node(ast->data.ast_pipeline.right, out);
    fputs(" }", out);
}

static void pp_negation(const ast_t *ast, FILE *out)
{
    fputs("negation ", out);
    pp_node(ast->data.ast_negation.child, out);
}

static void pp_if(const ast_t *ast, FILE *out)
{
    const struct ast_if *if_node = &ast->data.ast_if;
    fputs("if ", out);
    pp_braces(if_node->condition, out);
    fputs("; then ", out);
    pp_braces(if_node->then_body, out);
    if (if_node->else_body)
    {
        if (if_node->else_body->type == AST_IF)
        {
            fputs("; elif ", out);
            const struct ast_if *elif = &if_node->else_body->data.ast_if;
            pp_braces(elif->condition, out);
            fputs("; then ", out);
            pp_braces(elif->then_body, out);

            if (elif->else_body)
            {
                if (elif->else_body->type == AST_IF)
                {
                    fputs("; ", out);
                    pp_node(elif->else_body, out);
                    return;
                }
                fputs("; else ", out);
                pp_braces(elif->else_body, out);
            }
        }
        else
        {
            fputs("; else ", out);
            pp_braces(if_node->else_body, out);
        }
    }
    fputs("; fi", out);
}

static void pp_and_or(const ast_t *ast, FILE *out)
{
    fputs("and_or { ", out);
    pp_node(ast->data.ast_and_or.left, out);
    if (ast->data.ast_and_or.operator== AND_OP)
    {
        fprintf(out, " && ");
    }
    else
    {
        fprintf(out, " || ");
    }
    pp_node(ast->data.ast_and_or.right, out);
    fputs(" }", out);
}

static void pp_while_until(const ast_t *ast, FILE *out)
{
    const struct ast_while_until *ast_tmp = &ast->data.ast_while_until;
    if (ast_tmp->type == LOOP_WHILE)
    {
        fprintf(out, "while ");
    }
    else
    {
        fprintf(out, "until ");
    }
    pp_braces(ast_tmp->condition, out);
    fputs("; do ", out);
    pp_braces(ast_tmp->body, out);
    fputs("; done ", out);
}

static void pp_for(const ast_t *ast, FILE *out)
{
    const struct ast_for *ast_tmp = &ast->data.ast_for;
    fputs("for ", out);
    pp_node(ast_tmp->first_arg, out);
    if (ast_tmp->second_arg)
    {
        fputs(" in ", out);
        pp_node(ast_tmp->second_arg, out);
    }
    fputs("; do ", out);
    pp_braces(ast_tmp->body, out);
    fputs("; done ", out);
}

static void pp_assignment(const ast_t *ast, FILE *out)
{
    const struct ast_assignment *ast_tmp = &ast->data.ast_assignment;
    fputs("assignment ", out);
    pp_ignore_quotes(ast_tmp->var_name, out);
    fputc('=', out);
    pp_ignore_quotes(ast_tmp->value, out);
    if (ast_tmp->next)
    {
        fputc(' ', out);
        pp_assignment(ast_tmp->next, out);
    }
}

static void pp_node(const ast_t *ast, FILE *out)
{
    if (!ast)
    {
        fputs("/* NULL AST node */", out);
        return;
    }

    switch (ast->type)
    {
    case AST_CMD:
        pp_cmd(ast, out);
        break;
    case AST_LIST:
        pp_list(ast, out);
        break;
    case AST_PIPELINE:
        pp_pipeline(ast, out);
        break;
    case AST_NEGATION:
        pp_negation(ast, out);
        break;
    case AST_IF:
        pp_if(ast, out);
        break;
    case AST_AND_OR:
        pp_and_or(ast, out);
        break;
    case AST_WHILE_UNTIL:
        pp_while_until(ast, out);
        break;
    case AST_FOR:
        pp_for(ast, out);
        break;
    case AST_ASSIGNMENT:
        pp_assignment(ast, out);
        break;
    default:
        fputs("/* Unknown AST node */", out);
        break;
    }
}

void ast_pretty_print(const ast_t *ast, FILE *out)
{
    pp_node(ast, out);
    fputc('\n', out);
}
