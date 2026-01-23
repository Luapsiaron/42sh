#include "hashmap.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/str/str.h"

/**
 * Hash function that use the DJB2 Algorithm
 */
static size_t hash(const char *str)
{
    size_t hash = 5381; // Magic value for DJB2 algo
    int c;
    while ((c = *str++))
    {
        hash = hash * 33 + c;
    }
    return hash;
}

/**
 * Search a specific key in a linked list
 */
static struct pair_list *find_at(const struct hash_map *hash_map,
                                 size_t hash_value, const char *key)
{
    size_t index = hash_value % hash_map->size;
    struct pair_list *pair = hash_map->data[index];

    while (pair && strcmp(pair->key, key) != 0) // Traverse the linked list
        pair = pair->next;

    return pair;
}

/**
 * Initialize a new hash mp, allocates memory fot the structure and array
 */
struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *new_hash_map = malloc(sizeof(struct hash_map));
    if (!new_hash_map)
        return NULL;
    
    // Allocate the array, everything is set to zero
    struct pair_list **pair_list = calloc(size, sizeof(struct pair_list *));

    if (!pair_list)
    {
        free(new_hash_map);
        return NULL;
    }

    new_hash_map->size = size;
    new_hash_map->data = pair_list;
    return new_hash_map;
}

/**
 * Inserts/updates a KVP (key value pair) in the hash map
 */
bool hash_map_insert(struct hash_map *hash_map, const char *key, char *value,
                     bool *updated)
{
    if (!hash_map || hash_map->size == 0 || !key)
        return false;
    size_t hash_value = hash(key);
    size_t index = hash_value % hash_map->size;

    // Try to find existing key
    struct pair_list *existing = find_at(hash_map, hash_value, key);
    if (existing)
    {
        if (updated)
            *updated = true;

        free(existing->value);
        existing->value = xstrdup(value);
        return true;
    }

    //Key not found, insert new node at the beginning
    struct pair_list *new_pair_list = malloc(sizeof(struct pair_list));
    if (!new_pair_list)
        return false;
    new_pair_list->key = xstrdup(key);
    new_pair_list->value = xstrdup(value);
    new_pair_list->next = hash_map->data[index];

    hash_map->data[index] = new_pair_list;
    if (updated)
        *updated = false;

    return true;
}


/**
 * Free all memory used/associated with the hash map 
 */
void hash_map_free(struct hash_map *hash_map)
{

    if (!hash_map)
        return;

    if (hash_map->data)
    {
        for (size_t i = 0; i < hash_map->size; i++)
        {
            struct pair_list *cur = hash_map->data[i];
            while (cur)
            {
                struct pair_list *next = cur->next;

                free((char *)cur->key);
                free(cur->value);

                free(cur);
                cur = next;
            }
            hash_map->data[i] = NULL;
        }
        free(hash_map->data);
    }
    free(hash_map);
}

/**
 * Prints the content of the hash map
 * Used for debug purpose
 */
void hash_map_dump(struct hash_map *hash_map)
{
    if (!hash_map)
        return;

    for (size_t i = 0; i < hash_map->size; i++)
    {
        struct pair_list *first = hash_map->data[i];
        if (!first)
            continue;

        // Print first without comma
        printf("%s: %s", first->key, first->value);

        // Then the rest with preceding comma
        for (struct pair_list *current = first->next; current;
             current = current->next)
        {
            printf(", %s: %s", current->key, current->value);
        }
        putchar('\n');
    }
}

/**
 * Get the value associated to a certain key
 */
char *hash_map_get(const struct hash_map *hash_map, const char *key)
{
    if (!hash_map || !key || hash_map->size == 0 || !hash_map->data)
        return NULL;

    size_t h = hash(key);
    size_t idx = h % hash_map->size;

    struct pair_list *cur = hash_map->data[idx];
    while (cur)
    {
        if (strcmp(cur->key, key) == 0)
            return cur->value;
        cur = cur->next;
    }
    return NULL;
}

/**
 * Removes a KVP(key value pair) from the hash map
 * Free the associated memory
 */
bool hash_map_remove(struct hash_map *hash_map, const char *key)
{
    if (!hash_map || !key || hash_map->size == 0 || !hash_map->data)
        return false;

    size_t h = hash(key);
    size_t idx = h % hash_map->size;

    struct pair_list *cur = hash_map->data[idx];
    struct pair_list *prev = NULL;

    while (cur)
    {
        if (strcmp(cur->key, key) == 0)
        {
            // Found node to remove
            if (prev)
                prev->next = cur->next;
            else
                hash_map->data[idx] = cur->next; // Update head

            free((char *)cur->key);
            free(cur->value);

            free(cur);
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false; // Not found
}
