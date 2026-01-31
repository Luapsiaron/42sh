#include "execution.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../ast/ast.h"
#include "../builtins/break.h"
#include "../builtins/cd.h"
#include "../builtins/echo.h"
#include "../builtins/exit.h"
#include "../expansion/expand.h"
#include "../expansion/hashmap.h"
#include "condition.h"
#include "functions.h"
#include "loop.h"
#include "redir_pipe.h"
// command not found = 127
// command not executable = 126
// else = 1-125

static bool is_builtin(char *str) // checks if command is a builtin
{
    return (strcmp(str, "echo") == 0 || strcmp(str, "true") == 0
            || strcmp(str, "false") == 0)
        || strcmp(str, "cd") == 0 || strcmp(str, "exit") == 0
        || strcmp(str, "break") == 0;
}

static int exec_builtin(char **argv,
                        struct hash_map *hm) // executes a builtin command
{
    if (strcmp(argv[0], "echo") == 0)
    {
        return builtin_echo(argv);
    }
    else if (strcmp(argv[0], "true") == 0)
    {
        return 0;
    }
    else if (strcmp(argv[0], "false") == 0)
    {
        return 1;
    }
    else if (strcmp(argv[0], "cd") == 0)
    {
        return builtin_cd(argv, hm);
    }
    else if (strcmp(argv[0], "exit") == 0)
    {
        return builtin_exit(argv);
    }
    else if (strcmp(argv[0], "break") == 0)
    {
        return builtin_break(argv, hm);
    }
    return 127;
}

int wait_status(
    pid_t pid) // waits for a child process and returns its exit status
{
    int st = 0;
    if (waitpid(pid, &st, 0) < 0)
        return 1;
    if (WIFEXITED(st))
        return WEXITSTATUS(st);
    if (WIFSIGNALED(st))
        return 128 + WTERMSIG(st);
    return 1;
}

static int exec_redirwrap(struct ast *ast, struct hash_map *hm)
{
    struct saved_fd *saved = NULL;
    if(apply_redirs(ast->data.ast_redirwrap.redirections, &saved) != 0)
    {
        restore_fds(saved);
        return 1;
    }

    int status = exec_ast(ast->data.ast_redirwrap.shell_command, hm);
    restore_fds(saved);

    return status;
}

int exec_ast(struct ast *ast,
             struct hash_map *hm) // executes an AST node based on its type
{
    if (!ast)
        return 2;
    switch (ast->type)
    {
    case AST_LIST:
        return exec_list(ast, hm);
    case AST_IF:
        return exec_if(ast, hm);
    case AST_CMD:
        return exec_cmd_node(ast, hm);
    case AST_PIPELINE:
        return exec_pipeline(ast, hm);
    case AST_AND_OR:
        return eval_condition(ast, hm);
    case AST_WHILE_UNTIL:
        return exec_while_until(ast, hm);
    case AST_FOR:
        return exec_for(ast, hm);
    case AST_NEGATION: {
        int st = exec_ast(ast->data.ast_negation.child, hm);
        if (st == 0)
            return 1;
        else if (st == 1)
            return 0;
        return st;
    }
    case AST_FUNCDEC:
        return functions_register(ast);
    case AST_BLOCK:
        return exec_ast(ast->data.ast_block.body, hm);
    case AST_REDIRWRAP:
        return exec_redirwrap(ast,hm);
    default:
        fprintf(stderr, "Ast Type Not supported: %d\n", ast->type);
        return 2;
    }
}

static int is_break_status(int st) // checks if status code indicates a break
{
    return st >= RET_BREAK_BASE && st < RET_BREAK_BASE + RET_BREAK_MAX;
}

int exec_list(struct ast *ast,
              struct hash_map *hm) // executes a list of AST nodes sequentially
{
    int last = 0;
    struct ast *it = ast;

    while (it && it->type == AST_LIST)
    {
        last = exec_ast(it->data.ast_list.child, hm);
        if (is_break_status(last))
            return last;
        it = it->data.ast_list.next;
    }

    if (it)
    {
        last = exec_ast(it, hm);
        if (is_break_status(last))
            return last;
    }
    return last;
}

int exec_if(struct ast *ast, struct hash_map *hm) // executes an if AST node
{
    if (exec_ast(ast->data.ast_if.condition, hm) == 0)
    {
        return exec_ast(ast->data.ast_if.then_body, hm); // THEN
    }
    else if (ast->data.ast_if.else_body)
    {
        return exec_ast(ast->data.ast_if.else_body, hm); // ELSE
    }
    return 0; // false condition and no else, should continue
}

static void
exec_assignments(struct ast *assn,
                 struct hash_map *hm) // executes variable assignments
{
    while (assn)
    {
        if (assn->type == AST_ASSIGNMENT)
        {
            char *key = assn->data.ast_assignment.var_name;
            char *val = assn->data.ast_assignment.value;
            hash_map_insert(hm, key, val, NULL);
            break;
        }
        else if (assn->type == AST_LIST)
        {
            struct ast *child = assn->data.ast_list.child;
            if (child && child->type == AST_ASSIGNMENT)
            {
                char *key = assn->data.ast_assignment.var_name;
                char *val = assn->data.ast_assignment.value;
                hash_map_insert(hm, key, val, NULL);
            }
            assn = assn->data.ast_list.next;
        }
        else
        {
            break;
        }
    }
}

static int exec_cmd_apply_assign(struct ast *cmd,
                                 struct hash_map *hm) // call assignments
{
    if (!cmd->data.ast_cmd.assignments)
        return 0;
    exec_assignments(cmd->data.ast_cmd.assignments, hm);
    return 0;
}

static char **exec_cmd_expand(struct ast *cmd,
                              struct hash_map *hm) // expand argv
{
    char **raw = cmd->data.ast_cmd.argv;
    if (!raw || !raw[0])
        return NULL;
    return expand_argv(raw, hm);
}

static int exec_cmd_try_function(struct ast *cmd, struct hash_map *hm,
                                 char **argv) // try to execute as function
{
    const struct sh_function *fn = functions_lookup(argv[0]);
    if (!fn)
        return -1;

    struct fn_call call = { .hm = hm,
                            .argv = argv,
                            .redirs = cmd->data.ast_cmd.redirs };
    int st = exec_function_call(fn, &call);
    if (st == -3)
    {
        return last_exit_code;
    }
    last_exit_code = st;
    return st;
}

static int exec_cmd_run_builtin(struct ast *cmd, struct hash_map *hm,
                                char **argv) // run builtin with redirs
{
    struct saved_fd *saved = NULL;
    if (apply_redirs(cmd->data.ast_cmd.redirs, &saved) != 0)
        return restore_fds(saved), 1;
    int st = exec_builtin(argv, hm);
    fflush(stdout);
    restore_fds(saved);
    if (st == -3)
    {
        return last_exit_code;
    }
    last_exit_code = st;
    return st;
}

static int exec_cmd_run_external(struct ast *cmd,
                                 char **argv) // run external command
{
    pid_t pid = fork();
    if (pid < 0)
        return 1;
    if (pid == 0)
    {
        struct saved_fd *saved = NULL;
        if (apply_redirs(cmd->data.ast_cmd.redirs, &saved) != 0)
            _exit(1);
        execvp(argv[0], argv);
        fprintf(stderr, "42sh: non existant file");
        _exit(errno == ENOENT ? 127 : 126);
    }
    int st = wait_status(pid);
    last_exit_code = st;
    return st;
}

int exec_cmd_node(struct ast *cmd,
                  struct hash_map *hm) // executes a command AST node
/*
    If builtin: apply redirs in parent, run builtin, restore
    If external: fork, apply redirs in child, exec, wait
*/
{
    exec_cmd_apply_assign(cmd, hm);

    char **argv = exec_cmd_expand(cmd, hm);
    if (!argv || !argv[0])
        return free_argv(argv), 0;

    int st = exec_cmd_try_function(cmd, hm, argv);
    if (st != -1)
        return free_argv(argv), st;

    if (is_builtin(argv[0]))
        st = exec_cmd_run_builtin(cmd, hm, argv);
    else
        st = exec_cmd_run_external(cmd, argv);

    free_argv(argv);
    return st;
}

int child_exec_command(
    struct ast *node,
    struct hash_map *hm) // executes a command in a child process
{
    if (!node)
        return 0;
    if (node->type != AST_CMD)
        return exec_ast(node, hm);

    char **argv = expand_argv(node->data.ast_cmd.argv, hm);
    if (!argv || !argv[0])
        return free_argv(argv), 0;

    const struct sh_function *fn = functions_lookup(argv[0]);
    if (fn)
    {
        struct fn_call call = { .hm = hm, .argv = argv, .redirs = NULL };
        int st = exec_function_call(fn, &call);
        free_argv(argv);
        return st;
    }

    if (is_builtin(argv[0]))
        return exec_builtin(argv, hm);

    execvp(argv[0], argv);
    fprintf(stderr, "42sh: non existant file");
    _exit(errno == ENOENT ? 127 : 126);
}
