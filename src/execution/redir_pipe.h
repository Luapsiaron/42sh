#ifndef REDIR_PIPE_H
#define REDIR_PIPE_H

#include "../ast/ast.h"

struct saved_fd
{
    int target; // 0,1,2 or any io_number
    int backup; // dup(target)
    struct saved_fd *next;
};

void restore_fds(struct saved_fd *s);
int apply_redirs(ast_t *redir_list, struct saved_fd **saved);
int exec_pipeline(ast_t *pipe_node);

#endif /* ! REDIR_PIPE_H */