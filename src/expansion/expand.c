#define _POSIX_C_SOURCE 200809L

#include "expand.h"

#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../utils/str/str.h"
#include "hashmap.h"

struct exit_info exit_code = { .last = 0 };

static void char_append(buffer_t *buff, char c)
{
    if (buff->idx + 1 >= buff->capacity)
    {
        buff->capacity = (buff->capacity == 0) ? 16 : buff->capacity * 2;
        buff->buff = realloc(buff->buff, buff->capacity);
        if (!buff->buff)
            return;
    }
    buff->buff[buff->idx++] = c;
}

static void str_append(buffer_t *buff, char *str)
{
    if (!str)
        return;
    for (size_t i = 0; str[i] != '\0'; i++)
    {
        char_append(buff, str[i]);
    }
}

static char *dollar_hashtag(struct hash_map *hm)
{
    int c = 0;
    char key[16];
    while (1)
    {
        sprintf(key, "%d", c + 1);
        if (!hash_map_get(hm, key))
        {
            break;
        }
        c++;
    }
    char *res = malloc(16);
    sprintf(res, "%d", c);
    return res;
}

static char *handle_specials(struct hash_map *hm,
                             char *var_name) // var name = key in hashmap
{
    if (strcmp(var_name, "?") == 0) // EXIT CODE
    {
        char *res = malloc(16);
        sprintf(res, "%d", exit_code.last);
        return res;
    }
    if (strcmp(var_name, "$") == 0) // PID
    {
        char *res = malloc(16);
        sprintf(res, "%d", getpid());
        return res;
    }
    if (strcmp(var_name, "RANDOM") == 0) // RANDOM VALUE
    {
        char *res = malloc(16);
        sprintf(res, "%d", rand() % 32768); // 0-32767 range in bash rand
        return res;
    }
    if (strcmp(var_name, "*") == 0) //
    {
    }
    if (strcmp(var_name, "@") == 0) //
    {
    }
    if (strcmp(var_name, "#") == 0) // ARG NUMBER
    {
        return dollar_hashtag(hm);
    }
    if (strcmp(var_name, "UID") == 0)
    {
        char *res = malloc(16);
        sprintf(res, "%d", getuid());
        return res;
    }
    if (strcmp(var_name, "IFS") == 0)
    {
        char *val = hash_map_get(hm, "IFS");
        if (val != NULL)
        {
            return xstrdup(val);
        }
        return xstrdup("");
    }
    if (strcmp(var_name, "PWD") == 0)
    {
        char *val = hash_map_get(hm, "PWD");
        if (val)
        {
            return xstrdup(val);
        }
        return xstrdup("");
    }
    if (strcmp(var_name, "OLDPWD") == 0)
    {
        char *val = hash_map_get(hm, "OLDPWD");
        if (val != NULL)
        {
            return xstrdup(val);
        }
        return xstrdup("");
    }

    char *val = hash_map_get(hm, var_name);
    if (val != NULL)
    {
        return xstrdup(val);
    }
    return NULL;
}

static void handle_dollar(buffer_t *buff, size_t *index, char *word,
                          struct hash_map *hm)
{
    (*index)++; // skip first $

    size_t len_name = 0;
    size_t start = *index;
    char *var_name = NULL;

    if (word[*index] == '{') // ${VAR}
    {
        (*index)++;
        start = *index;

        while (word[*index] != '\0' && word[*index] != '}')
        {
            (*index)++;
        }
        if (word[*index] == '}')
        {
            len_name = *index - start;
            var_name = strndup(word + start, len_name); // STRNDUP a implem
        }
        else
        {
            errx(2, "42sh: can't expand value");
        }
    }
    else // $VAR
    {
        if (word[*index] == '?' || word[*index] == '!' || word[*index] == '$')
        {
            len_name = 1;
            (*index)++;
        }
        else
        {
            while (word[*index] != '\0'
                   && (word[*index] == '_' || isalnum(word[*index])))
            {
                (*index)++;
            }
        }
        len_name = *index - start;
        var_name = strndup(word + start, len_name);

        (*index)--; // Expand word func do i++, so one too far
    }

    if (var_name != NULL)
    {
        char *value = handle_specials(hm, var_name); // GET VALUE
        if (value != NULL)
        {
            str_append(buff, value);
            free(value);
        }
        free(var_name);
    }
    else
    {
        char_append(buff, '$');
    }
}

char **expand_argv(char **argv, struct hash_map *hm)
{
    char **res = NULL;
    size_t count = 0;
    for (size_t i = 0; argv[i] != NULL; i++)
    {
        char *word = argv[i];
        char *expanded = expand_word(word, hm);

        count++;
        res = realloc(res, sizeof(char *) * (count + 1));
        res[count - 1] = expanded;
        res[count] = '\0';
    }

    return res;
}

static void handle_escaped(buffer_t *buff, char *word, size_t *i)
{
    if (word[*i + 1] == '\\' || word[*i + 1] == '$' || word[*i + 1] == '"'
        || word[*i + 1] == '\n')
    {
        (*i)++;
        char_append(buff, word[*i]);
    }
    else
    {
        char_append(buff, '\\');
    }
}
char *expand_word(char *word, struct hash_map *hm)
{
    bool in_squote = false;
    bool in_dquote = false;

    buffer_t buff;
    buff.capacity = strlen(word) * 2;
    buff.buff = malloc(buff.capacity);
    buff.idx = 0;

    for (size_t i = 0; word[i] != '\0'; i++)
    {
        char c = word[i];
        if (in_squote)
        {
            if (c == '\'')
            {
                in_squote = false;
            }
            else
            {
                char_append(&buff, c);
            }
        }
        else if (in_dquote)
        {
            if (c == '"')
            {
                in_dquote = false;
            }
            else if (c == '$')
            {
                handle_dollar(&buff, &i, word, hm);
            }
            else if (c == '\\')
            {
                handle_escaped(&buff, word, &i);
            }
            else
            {
                char_append(&buff, c);
            }
        }
        else
        {
            if (c == '\'')
            {
                in_squote = true;
            }
            else if (c == '"')
            {
                in_dquote = true;
            }
            else if (c == '$')
            {
                handle_dollar(&buff, &i, word, hm);
            }
            else if (c == '\\')
            {
                if (word[i + 1] != '\0')
                {
                    i++;
                    char_append(&buff, word[i]);
                }
            }
            else
            {
                char_append(&buff, c);
            }
        }
    }
    char_append(&buff, '\0');
    return buff.buff;
}