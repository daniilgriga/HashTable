#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "hashtbl.h"
#include "file.h"
#include "list.h"
#include "tools.h"

uint32_t DJB2_hash (const char* str)
{
    uint32_t hash = 5381;
    unsigned char c;

    while ((c = (unsigned char) *str++) != 0)
        hash = (((hash << 5) + hash) + (uint32_t) c);

    return hash;
}

struct HashTable_t* hashT_ctor (long size)
{
    struct HashTable_t* hsh_t = calloc (1, sizeof (struct HashTable_t));
    if (hsh_t == NULL)
    {
        ERROR_MESSAGE (NULL_PTR_ERR)
        return NULL;
    }

    hsh_t->buckets = calloc ((size_t) size, sizeof (struct Node_t*));
    if (hsh_t->buckets == NULL)
    {
        ERROR_MESSAGE (NULL_PTR_ERR)
        return NULL;
    }

    hsh_t->size = size;

    return hsh_t;
}

int hashT_dtor (struct HashTable_t* hashT_ptr)
{
    assert (hashT_ptr);

    for (long i = 0; i < hashT_ptr->size; i++)
    {
        if (hashT_ptr->buckets[i] != NULL)
            list_dtor (hashT_ptr->buckets[i]);
    }

    free (hashT_ptr->buckets);
    free (hashT_ptr);

    return NO_ERRORS;
}

int hashT_fill (struct HashTable_t* hashT_ptr, const char* filename)
{
    assert (hashT_ptr);

    long numb_symb = 0;
    char* buffer = MakeBuffer (filename, &numb_symb); // ! free (buffer)

    long i = 0;
    while (i < numb_symb)
    {
        char word[32] = {};
        long n = 0;

        sscanf (buffer, " %s %ln ", word, &n);
        i += n;

        int status = hashT_search (hashT_ptr, word);
        if (!status)
            hashT_insert ();

        // maybe increments some value
    }

    // ...
}

int hashT_search (struct HashTable_t* hashT_ptr, const char* data)
{
    assert (hashT_ptr);
    assert (data);

    uint32_t hash = DJB2_hash (data);
    hash %= (uint32_t) hashT_ptr->size;

    struct Node_t* node = hashT_ptr->buckets[hash];
    if (node == NULL)
        return 1;

    int status = list_search (node, data);
    if (status)
        return 1;

    return 0;
}

// int hashT_insert (struct HashTable_t* hashT_ptr, const char* data)
