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
#include "../builtins/echo.h"
#include "redir_pipe.h"
#include "loop.h"
#include "condition.h"
#include "../expansion/expand.h"

// command not found = 127
// command not executable = 126
// else = 1-125

static bool is_builtin(char *str)
{
    return (strcmp(str, "echo") == 0 || strcmp(str, "true") == 0
            || strcmp(str, "false") == 0);
}

static int exec_builtin(char **argv)
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
    return 127;
}


int wait_status(pid_t pid)
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

int exec_ast(ast_t *ast, struct hash_map *hm)
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
        return exec_cmd_node(ast,hm);
    case AST_PIPELINE:
        return exec_pipeline(ast, hm);
    case AST_AND_OR:
        return eval_condition(ast);
    case AST_WHILE_UNTIL:
        return exec_while_until(ast);
    case AST_FOR:
        return exec_for(ast);
    case AST_NEGATION: {
        int st = exec_ast(ast->data.ast_negation.child,hm);
        if (st == 0)
            return 1;
        else if (st == 1)
            return 0;
        return st;
    }
    default:
        fprintf(stderr, "Ast Type Not supported: %d\n", ast->type);
        return 2;
    }
}

int exec_list(ast_t *ast, struct hash_map *hm)
{
    int exit_code = 0;
    while (ast && ast->type == AST_LIST)
    {
        if (ast->data.ast_list.child)
        {
            exit_code = exec_ast(ast->data.ast_list.child, hm);
        }
        ast = ast->data.ast_list.next;
    }
    return exit_code;
}

int exec_if(ast_t *ast, struct hash_map *hm)
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

int exec_cmd(char **argv)
{
    if (!argv || !argv[0])
        return 0;

    if (is_builtin(argv[0]))
        return exec_builtin(argv);

    pid_t pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "err: fork FAILED\n");
        return 1;
    }

    if (pid == 0)
    {
        execvp(argv[0], argv);
        int err = errno; // execvp error
        if (err == 2)
            _exit(127);
        _exit(126);
    }
    return wait_status(pid);
}
static bool hm_assign(struct hash_map *hm, char **argv)
{
    char *eq = strchr(argv[0], '=');
    if(eq && argv[1] == NULL)
    {
        *eq = '\0';
        char *key = argv[0];
        char *val = eq + 1;
        hash_map_insert(hm, key, val, NULL);
        
        *eq = '=';
        
        return 0;

    }
    return 1;


}
int exec_cmd_node(ast_t *cmd, struct hash_map *hm)
/*
    If builtin: apply redirs in parent, run builtin, restore
    If external: fork, apply redirs in child, exec, wait
*/
{
    char **before_argv = cmd->data.ast_cmd.argv;
    if (!before_argv || !before_argv[0])
        return 0;

    bool assigned = hm_assign(hm, before_argv);
    if(!assigned)
    {
        return 1;
    }
    
    char **argv = expand_argv(before_argv, hm);
    if(!argv || !argv[0])
    {
        free_argv((argv));
        return 0;
    }

    if (is_builtin(argv[0]))
    {
        struct saved_fd *saved = NULL;
        if (apply_redirs(cmd->data.ast_cmd.redirs, &saved) != 0)
        {
            restore_fds(saved);
            free_argv(argv);
            return 1;
        }
        int st = exec_builtin(argv);
        fflush(stdout); // subject reminder for builtins
        restore_fds(saved);

        free_argv(argv);
        return st;
    }

    pid_t pid = fork();
    if (pid < 0)
        return 1;

    if (pid == 0)
    {
        // child: apply redirs, no need to restore
        struct saved_fd *saved = NULL;
        if (apply_redirs(cmd->data.ast_cmd.redirs, &saved) != 0)
        {
            free_argv(argv);
            _exit(1);
        }

        execvp(argv[0], argv);
        free_argv(argv);
        _exit(errno == ENOENT ? 127 : 126);
    }
    free_argv(argv);
    return wait_status(pid);
}

int child_exec_command(ast_t *node, struct hash_map *hm)
{
    if (!node)
        return 0;
    if (node->type == AST_CMD)
    {
        char **argv = node->data.ast_cmd.argv;
        if (!argv || !argv[0])
            return 0;
        if (is_builtin(argv[0]))
            return exec_builtin(argv);
        execvp(argv[0], argv);
        // execvp failed:
        int err = errno;
        if (err == ENOENT)
            _exit(127);
        _exit(126);
    }
    return exec_ast(node, hm);
}
