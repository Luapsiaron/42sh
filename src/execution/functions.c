#define _POSIX_C_SOURCE 200809L
#include "functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/str/str.h"
#include "execution.h"
#include "redir_pipe.h"

static struct sh_function *g_funcs;

static char **argv_dup(char **argv) // duplicates an argv array
{
    size_t n = 0;
    while (argv && argv[n])
        n++;
    char **out = calloc(n + 1, sizeof(char *));
    if (!out)
        return NULL;
    for (size_t i = 0; i < n; i++)
        out[i] = xstrdup(argv[i]);
    return out;
}

static struct ast *redir_dup(struct ast *r) // duplicates a redirection AST list
{
    struct ast *head = NULL;
    struct ast *tail = head;
    for (; r; r = r->data.ast_redir.next)
    {
        struct ast *n =
            ast_redir_init(r->data.ast_redir.io_number, r->data.ast_redir.type,
                           r->data.ast_redir.word, NULL);
        if (!n)
            return ast_free(head), NULL;
        if (!head)
            head = n;
        else
            tail->data.ast_redir.next = n;
        tail = n;
    }
    return head;
}

static struct ast *ast_dup(struct ast *n); // forward declaration

static struct ast *list_dup(struct ast *l) // duplicates a list AST
{
    struct ast *head = NULL;
    struct ast *tail = NULL;
    while (l && l->type == AST_LIST)
    {
        struct ast *c = ast_dup(l->data.ast_list.child);
        struct ast *nn = ast_list_init(NULL, c);
        if (!nn)
            return ast_free(head), NULL;
        if (!head)
            head = nn;
        else
            tail->data.ast_list.next = nn;
        tail = nn;
        l = l->data.ast_list.next;
    }
    return head;
}

static struct ast *cmd_dup(struct ast *n) // duplicates a command AST
{
    struct ast *c = ast_cmd_init(argv_dup(n->data.ast_cmd.argv));
    if (!c)
        return NULL;
    c->data.ast_cmd.redirs = redir_dup(n->data.ast_cmd.redirs);
    c->data.ast_cmd.assignments = NULL;
    return c;
}

static struct ast *ast_dup(struct ast *n) // duplicates an AST node recursively
{
    if (!n)
        return NULL;
    if (n->type == AST_LIST)
        return list_dup(n);
    if (n->type == AST_CMD)
        return cmd_dup(n);
    if (n->type == AST_IF)
        return ast_if_init(ast_dup(n->data.ast_if.condition),
                           ast_dup(n->data.ast_if.then_body),
                           ast_dup(n->data.ast_if.else_body));
    if (n->type == AST_AND_OR)
        return ast_and_or_init(n->data.ast_and_or.operator,
                               ast_dup(n->data.ast_and_or.left),
                               ast_dup(n->data.ast_and_or.right));
    if (n->type == AST_PIPELINE)
        return ast_pipeline_init(ast_dup(n->data.ast_pipeline.right),
                                 ast_dup(n->data.ast_pipeline.left));
    if (n->type == AST_NEGATION)
        return ast_negation_init(ast_dup(n->data.ast_negation.child));
    if (n->type == AST_WHILE_UNTIL)
        return ast_while_until_init(ast_dup(n->data.ast_while_until.condition),
                                    ast_dup(n->data.ast_while_until.body),
                                    n->data.ast_while_until.type);
    if (n->type == AST_FOR)
        return ast_for_init(ast_dup(n->data.ast_for.first_arg),
                            ast_dup(n->data.ast_for.second_arg),
                            ast_dup(n->data.ast_for.body));
    if (n->type == AST_BLOCK)
        return ast_block_init(ast_dup(n->data.ast_block.body));
    if (n->type == AST_REDIRWRAP)
        return ast_redirwrap_init(
            ast_dup(n->data.ast_redirwrap.shell_command),
            redir_dup(n->data.ast_redirwrap.redirections));
    return NULL;
}

static struct sh_function *
find_func(const char *name) // finds a function by name
{
    for (struct sh_function *it = g_funcs; it; it = it->next)
        if (strcmp(it->name, name) == 0)
            return it;
    return NULL;
}

int functions_register(const struct ast *funcdec) // registers a new function
                                                  // from a FUNCDEC AST node
{
    if (!funcdec || funcdec->type != AST_FUNCDEC)
        return 1;
    struct sh_function *ex = find_func(funcdec->data.ast_funcdec.name);
    struct ast *body = ast_dup(funcdec->data.ast_funcdec.body);
    struct ast *rd = redir_dup(funcdec->data.ast_funcdec.redirs);
    if (!body && funcdec->data.ast_funcdec.body)
        return 1;
    if (ex)
    {
        ast_free(ex->body);
        ast_free(ex->redirs);
        ex->body = body;
        ex->redirs = rd;
        return 0;
    }
    struct sh_function *n = calloc(1, sizeof(*n));
    if (!n)
        return ast_free(body), ast_free(rd), 1;
    n->name = xstrdup(funcdec->data.ast_funcdec.name);
    n->body = body;
    n->redirs = rd;
    n->next = g_funcs;
    g_funcs = n;
    return 0;
}

const struct sh_function *
functions_lookup(const char *name) // looks up a function by name
{
    return find_func(name);
}

void functions_free_all(void) // frees all registered functions
{
    while (g_funcs)
    {
        struct sh_function *nx = g_funcs->next;
        free(g_funcs->name);
        ast_free(g_funcs->body);
        ast_free(g_funcs->redirs);
        free(g_funcs);
        g_funcs = nx;
    }
}

static void
pos_clear(struct hash_map *hm) // clears positional parameters from the hash map
{
    for (int i = 1;; i++)
    {
        char k[16];
        sprintf(k, "%d", i);
        if (!hash_map_get(hm, k))
            break;
        hash_map_remove(hm, k);
    }
}

static char **
pos_save(struct hash_map *hm) // saves positional parameters from the hash map
{
    int n = 0;
    char k[16];
    while (1)
    {
        sprintf(k, "%d", n + 1);
        if (!hash_map_get(hm, k))
            break;
        n++;
    }
    char **old = calloc(n + 1, sizeof(char *));
    for (int i = 0; i < n; i++)
    {
        sprintf(k, "%d", i + 1);
        old[i] = xstrdup(hash_map_get(hm, k));
    }
    return old;
}

static void
pos_restore(struct hash_map *hm,
            char **old) // restores positional parameters to the hash map
{
    pos_clear(hm);
    for (int i = 0; old && old[i]; i++)
    {
        char k[16];
        sprintf(k, "%d", i + 1);
        hash_map_insert(hm, k, old[i], NULL);
        free(old[i]);
    }
    free(old);
}

static void pos_set_from_argv(
    struct hash_map *hm,
    char **argv) // sets positional parameters in the hash map from argv
{
    pos_clear(hm);
    for (int i = 1; argv && argv[i]; i++)
    {
        char k[16];
        sprintf(k, "%d", i);
        hash_map_insert(hm, k, argv[i], NULL);
    }
}

static int redirs_apply_both(
    struct sh_function const *fn, struct fn_call *call,
    struct saved_fd *
        *saved) // applies redirections for both function definition and call
{
    if (apply_redirs(fn->redirs, saved) != 0)
        return 1;
    if (apply_redirs(call->redirs, saved) != 0)
        return 1;
    return 0;
}

int exec_function_call(const struct sh_function *fn,
                       struct fn_call *call) // executes a function call
{
    struct saved_fd *saved = NULL;
    if (redirs_apply_both(fn, call, &saved) != 0)
        return restore_fds(saved), 1;

    char **old = pos_save(call->hm);
    pos_set_from_argv(call->hm, call->argv);
    int st = exec_ast(fn->body, call->hm);
    pos_restore(call->hm, old);

    restore_fds(saved);
    return st;
}
