#ifndef REDIR_PIPE_H
#define REDIR_PIPE_H

#include "../ast/ast.h"
#include "../expansion/hashmap.h"

struct saved_fd
{
    int target; // 0,1,2 or any io_number
    int backup; // dup(target)
    struct saved_fd *next;
};

struct pipe_stage_args
{
    struct ast *cmd;
    struct hash_map *hm;
    int prev_read;
};

void restore_fds(struct saved_fd *s);
int apply_redirs(struct ast *redir_list, struct saved_fd **saved);
int exec_pipeline(struct ast *pipe_node, struct hash_map *hm);

#endif /* ! REDIR_PIPE_H */