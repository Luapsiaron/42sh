#define _POSIX_C_SOURCE 200809L

#include "expand.h"

#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../utils/str/str.h"
#include "hashmap.h"

int last_exit_code = 0;

/**
 * Push char, realloc if needed
 */
static void char_append(struct buffer *buff, char c)
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

static void str_append(struct buffer *buff, char *str)
{
    if (!str)
        return;
    for (size_t i = 0; str[i] != '\0'; i++)
    {
        char_append(buff, str[i]);
    }
}

/**
 * Count the number of arguments by iterating, for $#
 */
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

static char *format_out(int n)
{
    char *res = malloc(16);
    if (res == NULL)
    {
        return NULL;
    }
    sprintf(res, "%d", n);
    return res;
}

static char *random_value(void)
{
    static bool seeded = false;
    if (seeded == false)
    {
        srand(getpid() ^ time(NULL));
        seeded = true;
    }
    // 0-32767 range in bash rand
    return format_out(rand() % 32768);
}


static char *dollar_arobase(char *sep, struct hash_map *hm) // Concatenate the arguments
{
    struct buffer buff;
    buff.capacity = 128;
    buff.buff = malloc(buff.capacity);
    buff.idx = 0;
    if(buff.buff == NULL)
    {
        return NULL;
    }

    int i = 1; // Go through arguments
    char key[32];
    while(true)
    {
        sprintf(key, "%d", i);
        char *val = hash_map_get(hm, key);

        if(val == NULL)
        {
            break;
        }
        if(i > 1 && sep != NULL)
        {
            str_append(&buff, sep);
        }
        str_append(&buff, val);
        i++;

    }
    char_append(&buff, '\0');
    return buff.buff;
}

static char *dollar_star(struct hash_map *hm)
{
    char *ifs = hash_map_get(hm,"IFS");
    char sep[2] = {' ', '\0'}; // 2 because i use str_append in dollar_arobase
    if(ifs != NULL)
    {
        if(ifs[0] == '\0') // Empty IFS
        {
            sep[0] = '\0';
        }
        else
        {
            sep[0] = ifs[0];
        }
    }
    return dollar_arobase(sep, hm);

}
/**
 * Handles special variables or look for them in the hash map
 */
static char *handle_specials(struct hash_map *hm,
                             char *var_name) // var name = key in hashmap
{
    if (strcmp(var_name, "?") == 0) // Last exit code
    {
        return format_out(last_exit_code);
    }
    if (strcmp(var_name, "$") == 0) // PID
    {
        return format_out(getpid());
    }
    if (strcmp(var_name, "RANDOM") == 0) // RANDOM VALUE
    {
        return random_value();
    }
    if (strcmp(var_name, "*") == 0) // al arguments concatenated
    {
        return dollar_star(hm);
    }
    if (strcmp(var_name, "@") == 0) // all arguments, separated
    {
        return dollar_arobase(" ",hm);
    }
    if (strcmp(var_name, "#") == 0) // ARG NUMBER
    {
        return dollar_hashtag(hm);
    }
    if (strcmp(var_name, "UID") == 0) // user ID
    {
        return format_out(getuid());
    }
    if (strcmp(var_name, "IFS") == 0) // Internal Field Separator
    {
        char *val = hash_map_get(hm, "IFS");
        if (val != NULL)
        {
            return xstrdup(val);
        }
        return xstrdup("");
    }
    if (strcmp(var_name, "PWD") == 0) // PWD - working directory
    {
        char *val = hash_map_get(hm, "PWD");
        if (val)
        {
            return xstrdup(val);
        }
        return xstrdup("");
    }
    if (strcmp(var_name, "OLDPWD") == 0) // OLDPWD
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

/**
 * Expand variables $VAR and ${VAR}
 */
static void handle_dollar(struct buffer *buff, size_t *index, char *word,
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
            var_name = strndup(word + start, len_name);
        }
        else
        {
            errx(2, "42sh: can't expand value");
        }
    }
    else // $VAR
    {
        // Check for special variables
        if (word[*index] == '?' || word[*index] == '!' || word[*index] == '$'
            || word[*index] == '#' || word[*index] == '*'
            || word[*index] == '@')
        {
            len_name = 1;
            (*index)++;
        }
        else
        {
            // standard variables
            while (word[*index] != '\0'
                   && (word[*index] == '_' || isalnum(word[*index])))
            {
                (*index)++;
            }
        }
        len_name = *index - start;
        if (len_name == 0)
        {
            char_append(buff, '$');
            (*index)--;
            return;
        }

        var_name = strndup(word + start, len_name);

        (*index)--; // Expand word func do i++, so one too far
    }

    // Get value and add it to the buffer
    if (var_name != NULL)
    {
        char *value = handle_specials(hm, var_name);
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

/**
 * Expand a list of string
 * Go inside every string in argv and expand variables and quotes in each word
 */
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
        res[count] = NULL;
    }

    return res;
}

/**
 *  Escape handling in double quotes
 */
static void handle_escaped(struct buffer *buff, char *word, size_t *i)
{
    if (word[*i + 1] == '\\' || word[*i + 1] == '$' || word[*i + 1] == '"'
        || word[*i + 1] == '\n' || word[*i + 1] == '`')
    {
        (*i)++;
        char_append(buff, word[*i]);
    }
    else
    {
        char_append(buff, '\\');
    }
}

/**
 * Expand a word string:
 * Single quotes: preserve content
 * Double quotes: var expansions and escape
 */
char *expand_word(char *word, struct hash_map *hm)
{
    bool in_squote = false;
    bool in_dquote = false;

    struct buffer buff;
    buff.capacity = strlen(word) * 2;
    buff.buff = malloc(buff.capacity);
    buff.idx = 0;

    for (size_t i = 0; word[i] != '\0'; i++)
    {
        char c = word[i];
        if (in_squote) // In single quotes
        {
            if (c == '\'') // close signe quote
            {
                in_squote = false;
            }
            else
            {
                char_append(&buff, c);
            }
        }
        else if (in_dquote) // In double quotes
        {
            if (c == '"') // close double quote
            {
                in_dquote = false;
            }
            else if (c == '$')
            {
                handle_dollar(&buff, &i, word, hm); // expand variable
            }
            else if (c == '\\')
            {
                handle_escaped(&buff, word, &i);
            }
            else
            {
                char_append(&buff, c); // "normal" character
            }
        }
        else // outside quotes
        {
            if (c == '\'') // enter single quote
            {
                in_squote = true;
            }
            else if (c == '"') // enter double quote
            {
                in_dquote = true;
            }
            else if (c == '$')
            {
                handle_dollar(&buff, &i, word, hm); // expand variable
            }
            else if (c == '\\')
            {
                if (word[i + 1] != '\0') // escape next char if exists
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
