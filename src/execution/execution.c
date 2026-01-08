#include "execution.h"
#include "../builtins/echo.h"
#include "../ast/ast.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

// command not found = 127
// command not executable = 126
// else = 1-125

static bool is_builtin(char *str)
{
    return (strcmp(str, "echo") == 0 ||
            strcmp(str, "true") == 0 ||
            strcmp(str, "false") == 0);
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


int exec_ast(ast_t *ast)
{
    if(!ast)
        return 2;
    switch(ast->type)
    {
        case AST_LIST:
            return exec_list(ast);
        case AST_IF:
            return exec_if(ast);
        case AST_CMD:
            return exec_cmd(ast->data.ast_cmd.argv);
        default:
            fprintf(stderr, "Ast Type Not supported: %d\n", ast->type);
            return 2;
    }
}

int exec_list(ast_t *ast)
{
    int exit_code = 0;
    while(ast && ast->type == AST_LIST)
    {
        if(ast->data.ast_list.child)
        {
            exit_code = exec_ast(ast->data.ast_list.child);
        }
        ast = ast->data.ast_list.next;
    }
    return exit_code;
}

int exec_if(ast_t *ast)
{
    if (exec_ast(ast->data.ast_if.condition) == 0)
    {
        return exec_ast(ast->data.ast_if.then_body); // THEN
    }
    else if (ast->data.ast_if.else_body)
    {
        return exec_ast(ast->data.ast_if.else_body); // ELSE
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

