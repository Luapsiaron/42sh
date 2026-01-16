#include "redir_pipe.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../ast/ast.h"
#include "execution.h"

#define MAX_PIPELINE_CMDS 128

static int save_fd(struct saved_fd **s, int target) // Save current fd state
{
    int b = dup(target);
    if (b < 0)
        return -1;

    struct saved_fd *n = malloc(sizeof(*n));
    if (!n)
    {
        close(b);
        return -1;
    }

    n->target = target;
    n->backup = b;
    n->next = *s;
    *s = n;
    return 0;
}

void restore_fds(struct saved_fd *s) // Restore saved file descriptors
{
    while (s)
    {
        dup2(s->backup, s->target);
        close(s->backup);
        struct saved_fd *tmp = s;
        s = s->next;
        free(tmp);
    }
}

static int
apply_one_redir(const struct ast_redir *r) // Apply a single redirection
{
    int fd = -1;
    int target = r->io_number;
    if (target < 0)
    {
        if (r->type == REDIR_IN)
            target = 0;
        else
            target = 1;
    }
    switch (r->type)
    {
    case REDIR_IN:
        fd = open(r->word, O_RDONLY);
        break;
    case REDIR_OUT:
        fd = open(r->word, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        break;
    case REDIR_APPEND:
        fd = open(r->word, O_WRONLY | O_CREAT | O_APPEND, 0644);
        break;
    case REDIR_CLOBBER:
        fd = open(r->word, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        break;
    }
    if (fd < 0)
    {
        return 1;
    }
    if (dup2(fd, target) < 0)
    {
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}

int apply_redirs(
    ast_t *redir_list,
    struct saved_fd **saved) // Apply all redirections with save/restore
{
    for (ast_t *n = redir_list; n; n = n->data.ast_redir.next)
    {
        const struct ast_redir *r = &n->data.ast_redir;
        int target = r->io_number;
        if (target < 0)
        {
            if (r->type == REDIR_IN)
                target = 0;
            else
                target = 1;
        }
        if (save_fd(saved, target) < 0) // save before overwriting
            return 1;
        if (apply_one_redir(r) != 0)
            return 1;
    }
    return 0;
}

static void
pipeline_collect(ast_t *p, ast_t **arr,
                 int *len) // Collect commands from pipeline into array
{
    if (!p)
        return;
    if (p->type != AST_PIPELINE)
    {
        arr[(*len)++] = p;
        return;
    }
    pipeline_collect(p->data.ast_pipeline.left, arr, len);
    pipeline_collect(p->data.ast_pipeline.right, arr, len);
}

int exec_pipeline(ast_t *pipe_node, struct hash_map *hm) // Execute a pipeline of commands
{
    ast_t *cmds[MAX_PIPELINE_CMDS];
    int n = 0;
    pipeline_collect(pipe_node, cmds, &n);
    if (n == 0)
        return 0;

    int prev_read = -1;
    pid_t pids[MAX_PIPELINE_CMDS];

    for (int i = 0; i < n; i++)
    {
        int fds[2] = { -1, -1 };
        if (i != n - 1)
        {
            if (pipe(fds) < 0)
                return 1;
        }

        pid_t pid = fork();
        if (pid < 0)
            return 1;

        if (pid == 0)
        {
            // stdin from previous pipe read
            if (prev_read != -1)
            {
                dup2(prev_read, 0);
            }
            // stdout to current pipe write (unless last)
            if (i != n - 1)
            {
                dup2(fds[1], 1);
            }
            // close fds we don't need in child
            if (prev_read != -1)
                close(prev_read);
            if (i != n - 1)
            {
                close(fds[0]);
                close(fds[1]);
            }
            // Apply command’s redirections AFTER pipe dup2
            // (so redirs override the pipe if both exist)
            if (cmds[i]->type == AST_CMD)
            {
                struct saved_fd *saved = NULL;
                if (apply_redirs(cmds[i]->data.ast_cmd.redirs, &saved) != 0)
                    _exit(1);
            }
            // execute command node
            _exit(child_exec_command(cmds[i], hm));
        }
        // parent
        pids[i] = pid;
        if (prev_read != -1)
            close(prev_read);
        if (i != n - 1)
        {
            close(fds[1]); // parent closes write end
            prev_read = fds[0]; // keep read end for next cmd
        }
    }
    // parent: close last prev_read if any
    if (prev_read != -1)
        close(prev_read);
    // wait all; status is last command
    int last_status = 0;
    for (int i = 0; i < n; i++)
    {
        int st = wait_status(pids[i]);
        if (i == n - 1)
            last_status = st;
    }
    return last_status;
}
