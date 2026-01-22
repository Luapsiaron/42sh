#include "pretty_printer.h"

#include <ctype.h>
#include <string.h>

static void pp_node(const struct ast *ast, FILE *out);
static void pp_list(const struct ast *ast, FILE *out);
static void pp_redir(const struct ast *ast, FILE *out);
static void pp_cmd(const struct ast *ast, FILE *out);
static void pp_pipeline(const struct ast *ast, FILE *out);
static void pp_negation(const struct ast *ast, FILE *out);
static void pp_if(const struct ast *ast, FILE *out);
static void pp_and_or(const struct ast *ast, FILE *out);
static void pp_while_until(const struct ast *ast, FILE *out);
static void pp_for(const struct ast *ast, FILE *out);
static void pp_assignment(const struct ast *ast, FILE *out);
static void pp_block(const struct ast *ast, FILE *out);
static void pp_funcdec(const struct ast *ast, FILE *out);
static void pp_redirwrap(const struct ast *ast, FILE *out);

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

static void pp_braces(const struct ast *ast, FILE *out)
{
    fputs("{ ", out);
    pp_node(ast, out);
    fputs(" }", out);
}

static void pp_list(const struct ast *ast, FILE *out)
{
    const struct ast *cur = ast;
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

static const char *redir_op(enum redir_type type)
{
    switch (type)
    {
    case REDIR_IN:
        return "<";
    case REDIR_OUT:
        return ">";
    case REDIR_APPEND:
        return ">>";
    case REDIR_CLOBBER:
        return ">|";
    case REDIR_DUP_IN:
        return "<&";
    case REDIR_DUP_OUT:
        return ">&";
    case REDIR_INOUT:
        return "<>";
    default:
        return "?";
    }
}
static void pp_redir(const struct ast *ast, FILE *out)
{
    const struct ast *cur = ast;
    while (cur)
    {
        const struct ast_redir *redir = &cur->data.ast_redir;
        fputc(' ', out);

        fprintf(out, "%d%s ", redir->io_number, redir_op(redir->type));
        fputc(' ', out);
        pp_ignore_quotes(redir->word, out);

        cur = redir->next;
    }
}

static void pp_cmd(const struct ast *ast, FILE *out)
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

static void pp_pipeline(const struct ast *ast, FILE *out)
{
    fputs("pipeline { ", out);
    pp_node(ast->data.ast_pipeline.left, out);
    fprintf(out, " | ");
    pp_node(ast->data.ast_pipeline.right, out);
    fputs(" }", out);
}

static void pp_negation(const struct ast *ast, FILE *out)
{
    fputs("negation ", out);
    pp_node(ast->data.ast_negation.child, out);
}

static void pp_if(const struct ast *ast, FILE *out)
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

static void pp_and_or(const struct ast *ast, FILE *out)
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

static void pp_while_until(const struct ast *ast, FILE *out)
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

static void pp_for(const struct ast *ast, FILE *out)
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

static void pp_assignment(const struct ast *ast, FILE *out)
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

static void pp_block(const struct ast *ast, FILE *out)
{
    fputs("block { ", out);
    pp_node(ast->data.ast_block.body, out);
    fputs(" }", out);
}

static void pp_funcdec(const struct ast *ast, FILE *out)
{
    fputs("funcdec ", out);
    pp_ignore_quotes(ast->data.ast_funcdec.name, out);
    fputs(" () ", out);
    
    pp_node(ast->data.ast_funcdec.body, out);

    if(ast->data.ast_funcdec.redirs)
    {
        pp_redir(ast->data.ast_funcdec.redirs, out);
    }
}

static void pp_redirwrap(const struct ast *ast, FILE *out)
{
    fputs("redirwrap { ", out);
    pp_node(ast->data.ast_redirwrap.shell_command, out);
    fputs(" }", out);

    if(ast->data.ast_redirwrap.redirections)
    {
        pp_redir(ast->data.ast_redirwrap.redirections, out);
    }
}

static void pp_node(const struct ast *ast, FILE *out)
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
    case AST_BLOCK:
        pp_block(ast, out);
        break;
    case AST_FUNCDEC:
        pp_funcdec(ast, out);
        break;
    case AST_REDIRWRAP:
        pp_redirwrap(ast, out);
        break;
    default:
        fputs("/* Unknown AST node */", out);
        break;
    }
}

void ast_pretty_print(const struct ast *ast, FILE *out)
{
    pp_node(ast, out);
    fputc('\n', out);
}
