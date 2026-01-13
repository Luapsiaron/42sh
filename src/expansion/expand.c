#include "expand.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <err.h>

static void char_append(char **buff, size_t *idx, size_t *capacity, char c)
{
    if(*idx+1 < *capacity)
    {
        (*buff)[*idx] = c;
        (*idx)++;
    }
    else
    {
        *capacity *=2;
        *buff = realloc(*buff, *capacity);
    }
}

static void str_append(char **buff, size_t *idx, size_t *capacity, char *str)
{
    if(!str)
        return;
    for(size_t i =0; str[i] != NULL ; i++)
    {
        char_append(buff, idx, capacity, str[i]);
    }

}


static char *handle_dollar(char **buff, size_t *idx, size_t *capacity, char *word)
{
    (*idx) +=1;

    size_t len_name = 0;
    char *var_name = NULL;

    if(buff[*idx] == '{') // ${VAR}
    {
        *idx += 1;
        size_t start = *idx;

        while(word[*idx] && word[*idx] != '}')
        {
            (*idx)++;
        }
        if(word[*idx] == '}')
        {
            len_name = *idx - start;
            var_name = strndup(word + start, len_name); // STRNDUP a implem
        }
        else
        {
            errx(2,"42sh: can't expand value");
        }
    }
    else // $VAR
    {
        size_t start = *idx;
        while(word[*idx] && )
    }

}


char **expand_argv(char **argv)
{
    char **res = NULL;
    size_t count = 0;
    for(size_t i=0; argv[i]!= NULL;  i++)
    {
        char *word = argv[i];
        char *expanded = expand_word(word);

        count++;
        res = realloc(res, sizeof(char *) * (count + 1));
        res[count -1 ] = expanded;
        res[count] = '\0';
    }   
    
    return res;
}

char *expand_word(char *word)
{
    size_t word_len = strlen(word);
    
    bool in_squote = false;
    bool in_dquote = false;
    
    size_t capacity = strlen(word);
    char *buff = malloc(capacity);
    size_t idx = 0;


    for(size_t i = 0; word[i] != NULL; i++)
    {
        char c = word[i];
        if(in_squote)
        {
            if(c == '\'')
            {
                in_squote = false;
            }
            else
            {
                buff[idx + 1] = c;
            }

        }
        else if(in_dquote)
        {
            if(c =='"')
            {
                in_dquote = false;
            }
            else if (c == '$')
            {
                handle_dollar(&buff, &idx, &capacity, word);
            }
            else if (c == '\\')
            {
                
            }
            else
            {
                buff = realloc(buff, strlen(buff) + 1);
                buff[idx++] = c;
            }
        }
        else
        {
            if(c == '\'')
            {
                in_squote = true;
            }
            else if(c = '"')
            {
                in_dquote = true;
            }
            else if(c == '$')
            {
                handle_dollar(&buff, &idx, &capacity, word);
            }
            else if (c == '\\')
            {
                if(word[i+1] != NULL)
                {
                    buff[idx++] = word[i + 1];
                    i++;
                }
            }
            else
            {
                append_char(&buff, &idx, &capacity, c);
            }
        }


    }
    append_char(&buff, &idx, &capacity, '\0');
    return buff;
}