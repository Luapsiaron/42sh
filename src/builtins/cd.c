#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../expansion/hashmap.h"
#include "../utils/str/str.h"

#ifndef PATH_MAX
#    define PATH_MAX 4096
#endif

static char *concatenate_path(char *curr, char *path)
{
    if (path != NULL && path[0] == '/') // already absolute
    {
        return xstrdup(path);
    }

    size_t len = strlen(curr) + strlen(path) + 2; // +2 for '/' and '\0'
    char *res = malloc(len);
    if (!res)
    {
        return NULL;
    }
    sprintf(res, "%s/%s", curr, path);
    return res;
}

static char *normalize_logical(char *path)
{
    char buff[PATH_MAX];
    char *levels[512]; // little stack for path levels
    size_t count = 0;

    strncpy(buff, path, PATH_MAX - 1); // copy because strtok mdify string
    buff[PATH_MAX - 1] = '\0';

    char *tok = strtok(buff, "/");
    while (tok != NULL)
    {
        if (strcmp(tok, ".") == 0)
        {
            // ignored because current dir
        }
        else if (strcmp(tok, "..") == 0)
        {
            if (count > 0)
            {
                count--;
            }
        }
        else
        {
            levels[count] = tok; // push valid dir
            count++;
        }
        tok = strtok(NULL, "/");
    }
    // build path from stack
    char *res = calloc(PATH_MAX, 1);
    if (count == 0)
    {
        strcpy(res, "/"); // stack empty, root
    }
    else
    {
        for (size_t i = 0; i < count; i++)
        {
            strcat(res, "/");
            strcat(res, levels[i]);
        }
    }
    return res;
}

static char *get_target(char *arg, struct hash_map *hm, bool *toprint)
{
    if (arg == NULL)
    {
        char *home = hash_map_get(hm, "HOME"); // no arg -> HOME
        if (home == NULL)
        {
            fprintf(stderr, "HOME not found\n");
            return home;
        }
        return home;
    }
    else if (strcmp(arg, "-") == 0)
    {
        char *old = hash_map_get(hm, "OLDPWD"); // '-' means go to old dir
        if (old == NULL)
        {
            fprintf(stderr, "OLDPWD not found\n");
            return old;
        }
        else
        {
            *toprint = true;
        }
        return old;
    }
    return arg;
}

int builtin_cd(char **argv, struct hash_map *hm)
{
    if (argv[1] != NULL && argv[2] != NULL)
    {
        fprintf(stderr, "cd: too much args\n");
        return 1;
    }
    bool toprint = false;
    char *target = get_target(argv[1], hm, &toprint);
    if (target == NULL)
    {
        return 1;
    }

    char *raw = hash_map_get(hm, "PWD"); // logical
    char *curpath = xstrdup(raw); // dup not to modify hashmap data

    char *logical = concatenate_path(curpath, target);
    char *symlink_path = normalize_logical(logical);
    free(logical);

    if (chdir(symlink_path) != 0)
    {
        if (chdir(target) != 0) // if logical fail then physical
        {
            fprintf(stderr, "cd: can't reach %s\n", target);
            free(curpath);
            free(symlink_path);
            return 1;
        }
        free(symlink_path);

        char cwd[PATH_MAX];
        if (getcwd(cwd, PATH_MAX))
        {
            symlink_path = xstrdup(cwd);
        }
        else
        {
            symlink_path = xstrdup(target);
        }
    }

    hash_map_insert(hm, "OLDPWD", curpath, NULL); // set OLDPWD to old PWD
    hash_map_insert(hm, "PWD", symlink_path, NULL);

    if (toprint)
    {
        printf("%s\n", symlink_path);
    }
    free(symlink_path);
    free(curpath);
    return 0;
}
