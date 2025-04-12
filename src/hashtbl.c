#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "hashtbl.h"
#include "tools.h"

struct HashTable_t* hashT_ctor (size_t size)
{
    struct HashTable_t* hsh_t = calloc (1, sizeof (struct HashTable_t));
    if (hsh_t == NULL)
    {
        ERROR_MESSAGE (NULL_PTR_ERR)
        return NULL;
    }

    hsh_t->buckets = calloc (size, sizeof (struct Node_t*));
    if (hsh_t->buckets == NULL)
    {
        ERROR_MESSAGE (NULL_PTR_ERR)
        return NULL;
    }

    hsh_t->size = size;

    return hsh_t;
}

int hashT_dtor (struct HashTable_t* hsh_t)
{
    assert (hsh_t);

    for (size_t i = 0; i < hsh_t->size; i++)
    {
        if (hsh_t->buckets[i] != NULL)
            list_dtor (hsh_t->buckets[i]);
    }

    free (hsh_t->buckets);
    free (hsh_t);

    return NO_ERRORS;
}
