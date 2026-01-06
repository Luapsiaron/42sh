#ifndef AST_H
#define AST_H
/* transform tokens into AST nodes */
enum node_type 
{
    NODE_COMMAND,
    NODE_PIPE,
    NODE_IF,
    NODE_AND,
    NODE_REDIRECT
};

struct ast // ex for: echo toto tata > file.txt
{
    node_type type;  // NODE_COMMAND
    char **args;        // Simple command ex: = [echo], [toto], [tata], NULL
    struct ast *left;   // If condition / Left part of pipe etc
    struct ast *right;  // If body / Right part of Pipe etc
    struct ast *else_branch; // NULL
    char *redir_file; // file.txt
    int  redir_fd; // 1 (stdout)
};

#endif /* ! AST_H */